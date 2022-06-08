#include "info.h"
#include "info_cmd.h"
#include <iostream>
#include <stdexcept>

int main (int argc, char **argv)
{
    using namespace std;
    using namespace spoc;

    try
    {
        // Parse command line
        const cmd::args args = cmd::get_args (argc, argv,
                string (argv[0]) + " [options] [spocfile] [spocfile] [...]");

        // If version was requested, print it and exit
        if (args.version)
        {
            clog << "Version "
                << static_cast<int> (MAJOR_VERSION)
                << "."
                << static_cast<int> (MINOR_VERSION)
                << endl;
            return 0;
        }

        // If you are getting help, exit without an error
        if (args.help)
            return 0;

        // Show args
        if (args.verbose)
        {
            clog << "verbose\t" << args.verbose << endl;
            clog << "json\t" << args.json << endl;
            clog << "header-info\t" << args.header_info << endl;
            clog << "summary-info\t" << args.summary_info << endl;
            clog << "classifications\t" << args.classifications << endl;
            clog << "compact\t" << args.compact << endl;
            clog << "filenames\t" << args.fns.size () << endl;
        }

        if (args.fns.empty ())
        {
            if (args.verbose)
                clog << "Reading from stdin" << endl;

            // Read the file
            spoc_file f = read_spoc_file_uncompressed (cin);

            info::process (cout, f,
                args.json, args.header_info, args.summary_info,
                args.classifications, args.compact);
        }
        else
        {
            for (auto fn : args.fns)
            {
                if (args.verbose)
                    clog << "Reading " << fn << endl;

                ifstream ifs (fn);

                if (!ifs)
                    throw runtime_error ("Could not open file for reading");

                // Read into spoc_file struct
                spoc_file f = read_spoc_file_uncompressed (ifs);

                info::process (cout, f,
                    args.json, args.header_info, args.summary_info,
                    args.classifications, args.compact);
            }
        }

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
