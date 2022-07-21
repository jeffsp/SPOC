#include "spoc.h"
#include "srs_cmd.h"
#include <iostream>
#include <stdexcept>

int main (int argc, char **argv)
{
    using namespace std;
    using namespace spoc::io;
    using namespace spoc::srs_cmd;

    try
    {
        // Parse command line
        const args args = get_args (argc, argv,
                string (argv[0]) + " [options] [spocfile] [spocfile] [...]");

        // If version was requested, print it and exit
        if (args.version)
        {
            cout << "Version "
                << static_cast<int> (spoc::MAJOR_VERSION)
                << "."
                << static_cast<int> (spoc::MINOR_VERSION)
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
            clog << "srs\t'" << args.srs << "'" << endl;
            clog << "filenames\t" << args.fns.size () << endl;
        }

        if (args.fns.empty ())
        {
            if (args.verbose)
                clog << "Reading from stdin" << endl;

            // Read into spoc_file struct
            spoc_file f = read_spoc_file_uncompressed (cin);

            if (args.set_srs)
            {
                // Set SRS
                f.set_wkt (args.srs);

                if (args.verbose)
                    clog << "Writing to stdout" << endl;

                write_spoc_file_uncompressed (cout, f);
            }
            else
            {
                cout << f.get_wkt () << endl;
            }
        }
        else if (args.set_srs)
        {
            if (args.fns.size () != 2)
                throw runtime_error (
                    "When setting the SRS, you need to specify one input file and one output file");

            if (args.verbose)
                clog << "Reading " << args.fns[0] << endl;

            ifstream ifs (args.fns[0]);

            if (!ifs)
                throw runtime_error ("Could not open file for reading");

            // Read into spoc_file struct
            spoc_file f = read_spoc_file (ifs);

            // Set SRS
            f.set_wkt (args.srs);

            if (args.verbose)
                clog << "Writing " << args.fns[1] << endl;

            ofstream ofs (args.fns[1]);

            if (!ofs)
                throw runtime_error ("Could not open file for writing");

            write_spoc_file (ofs, f);
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
                spoc_file f = read_spoc_file (ifs);

                cout << f.get_wkt () << endl;
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
