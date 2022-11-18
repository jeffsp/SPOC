#include "spoc/app_utils.h"
#include "spoc/spoc.h"
#include "transform.h"
#include "transform_cmd.h"
#include <cctype>
#include <iostream>
#include <stdexcept>

int main (int argc, char **argv)
{
    using namespace std;
    using namespace spoc::app_utils;
    using namespace spoc::transform_app;
    using namespace spoc::transform_cmd;

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

        // Check the arguments
        if (args.commands.empty ())
            throw runtime_error ("No command was specified");

        // Show args
        if (args.verbose)
        {
            clog << "verbose\t" << args.verbose << endl;
            clog << "random-seed\t" << args.random_seed << endl;
            clog << "commands:" << endl;
            for (auto c : args.commands)
                clog << "\t" << c.name << "\t" << c.params << endl;
        }

        // Get the input stream
        input_stream is (args.verbose, args.input_fn);

        // Get the output stream
        output_stream os (args.verbose, args.output_fn);

        // Apply each command in order they appeared on command line
        apply (is (), os (), args.commands, args.random_seed);

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
