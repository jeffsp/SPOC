#include "diff.h"
#include "diff_cmd.h"
#include <iostream>
#include <stdexcept>

int main (int argc, char **argv)
{
    using namespace std;
    using namespace spoc::diff_app;
    using namespace spoc::diff_cmd;
    using namespace spoc::io;

    try
    {
        // Parse command line
        const args args = get_args (argc, argv,
                string (argv[0]) + " [options] spocfile1 spocfile2");

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
            clog << "header-only\t" << args.header_only << endl;
            clog << "data-only\t" << args.data_only << endl;
            clog << "fields\t" << args.fields.size () << endl;
            clog << "reverse\t" << args.reverse << endl;
            clog << "filename1\t'" << args.fn1 << "'" << endl;
            clog << "filename2\t'" << args.fn2 << "'" << endl;
        }

        if (args.header_only + args.data_only + !args.fields.empty () > 2)
            throw runtime_error ("Can't specify more than 1 of 'header-only', 'data-only', and specific fields");

        if (args.verbose)
            clog << "Reading " << args.fn1 << endl;

        ifstream ifs1 (args.fn1);

        if (!ifs1)
            throw runtime_error ("Could not open file for reading");

        if (args.verbose)
            clog << "Reading " << args.fn2 << endl;

        ifstream ifs2 (args.fn2);

        if (!ifs2)
            throw runtime_error ("Could not open file for reading");

        // Read into spoc_file structs
        spoc_file f1 = read_spoc_file_uncompressed (ifs1);
        spoc_file f2 = read_spoc_file_uncompressed (ifs2);

        const int return_code =
            diff (f1, f2,
                args.header_only, args.data_only, args.fields, args.reverse);

        if (args.verbose)
            clog << "The files are "
                << (return_code == 0 ? "the same" : "different")
                << endl;

        return return_code;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
