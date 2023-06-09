#include "spoc/app_utils.h"
#include "spoc/spoc.h"
#include "info.h"
#include "info_cmd.h"
#include <iostream>
#include <stdexcept>

int main (int argc, char **argv)
{
    using namespace std;
    using namespace spoc::file;
    using namespace spoc::info_app;
    using namespace spoc::info_cmd;
    using namespace spoc::io;

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
            clog << "json\t" << args.json << endl;
            clog << "header-info\t" << args.header_info << endl;
            clog << "summary-info\t" << args.summary_info << endl;
            clog << "classification-info\t" << args.classification_info << endl;
            clog << "metric-info\t" << args.metric_info << endl;
            clog << "compact\t" << args.compact << endl;
            clog << "quartiles\t" << args.quartiles << endl;
            clog << "filenames\t" << args.fns.size () << endl;
        }

        if (args.fns.empty ())
        {
            if (args.verbose)
                clog << "Reading from stdin" << endl;

            // Read the file
            spoc_file f = read_spoc_file (cin);

            process (cout, f,
                args.json, args.header_info, args.summary_info,
                args.classification_info, args.metric_info,
                args.compact, args.quartiles);
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

                process (cout, f,
                    args.json, args.header_info, args.summary_info,
                    args.classification_info, args.metric_info,
                    args.compact, args.quartiles);
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
