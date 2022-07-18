#include "app_utils.h"
#include "compress.h"
#include "compress_cmd.h"
#include "spoc.h"
#include <iostream>
#include <stdexcept>

int main (int argc, char **argv)
{
    using namespace std;
    using namespace spoc::app_utils;
    using namespace spoc::compress_app;
    using namespace spoc::compress_cmd;
    using namespace spoc::io;

    try
    {
        // Parse command line
        const args args = get_args (argc, argv,
                string (argv[0]) + " [options] [input] [output]");

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

        // Show args
        if (args.verbose)
        {
            clog << "verbose\t" << args.verbose << endl;
            clog << "precision\t" << args.precision << endl;
            clog << "input_fn\t" << args.input_fn << endl;
            clog << "output_fn\t" << args.output_fn << endl;
        }

        if (args.precision > 52)
            throw runtime_error ("The precision must be less than 53");

        // If you are getting help, exit without an error
        if (args.help)
            return 0;

        // Get the input stream
        input_stream is (args.verbose, args.input_fn);

        // Read the input file
        spoc_file f = read_spoc_file_uncompressed (is ());

        // Get the output stream
        output_stream os (args.verbose, args.output_fn);

        // Set the compression bit
        f.set_compressed (true);

        // Set precision if specified
        if (args.precision != 0)
            change_precision (f, args.precision);

        // Write it out
        write_spoc_file_compressed (os (), f);

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
