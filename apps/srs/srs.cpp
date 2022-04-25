#include "spoc.h"
#include "srs_cmd.h"
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
            spoc_file f = read_spoc_file (cin);

            if (args.set_srs)
            {
                f.set_wkt (args.srs);
                write_spoc_file (cout, f);
            }
            else
                cout << f.get_wkt () << endl;
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

                if (args.set_srs)
                {
                    f.set_wkt (args.srs);
                    write_spoc_file (cout, f);
                }
                else
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
