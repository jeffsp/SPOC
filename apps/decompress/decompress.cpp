#include "app_utils.h"
#include "spoc.h"
#include "decompress_cmd.h"
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace spoc;

int main (int argc, char **argv)
{
    using namespace std;
    using namespace spoc;

    try
    {
        // Parse command line
        const cmd::args args = cmd::get_args (argc, argv,
                string (argv[0]) + " [options] [input] [output]");

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

        // Get the input stream
        app_utils::input_stream is (args.verbose, args.input_fn);

        // Read the input file
        spoc_file f = read_spoc_file_compressed (is ());

        // Get the output stream
        app_utils::output_stream os (args.verbose, args.output_fn);

        // Unset the compression bit
        f.set_compressed (false);

        // Write it out
        spoc::write_spoc_file_uncompressed (os (), f);

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
