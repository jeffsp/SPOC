#include "app_utils.h"
#include "spoc.h"
#include "util.h"
#include "tool_cmd.h"
#include <cctype>
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

        // Check the arguments
        if (args.command.name.empty ())
            throw std::runtime_error ("No command was specified");

        // Show args
        if (args.verbose)
            std::clog << "command: " << args.command.name << args.command.params << std::endl;

        // Get the input stream
        app_utils::input_stream is (args.verbose, args.input_fn);

        // Read the header
        const header h = read_header (is ());

        // Check compression flag
        if (h.compressed)
            throw std::runtime_error ("Expected an uncompressed file");

        // Get the output stream
        app_utils::output_stream os (args.verbose, args.output_fn);

        using namespace spoc::util;

        if (args.command.name == "recenter-xy")
            recenter (is (), os (), h);
        else if (args.command.name == "recenter-xyz")
            recenter (is (), os (), h, true);
        else if (args.command.name == "subtract-min-xy")
            subtract_min (is (), os (), h);
        else if (args.command.name == "subtract-min-xyz")
            subtract_min (is (), os (), h, true);
        else
            throw std::runtime_error ("An unknown command was encountered");

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
