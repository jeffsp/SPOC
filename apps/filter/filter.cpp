#include "app_utils.h"
#include "spoc.h"
#include "filter.h"
#include "filter_cmd.h"
#include <cctype>
#include <iostream>
#include <stdexcept>

int main (int argc, char **argv)
{
    using namespace std;
    using namespace spoc::app_utils;
    using namespace spoc::filter_app;
    using namespace spoc::filter_cmd;

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

        // If you are getting help, exit without an error
        if (args.help)
            return 0;

        // Get the input stream
        input_stream is (args.verbose, args.input_fn);

        // Get the output stream
        output_stream os (args.verbose, args.output_fn);

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
