#include "app_utils.h"
#include "spoc.h"
#include "transform.h"
#include "transform_cmd.h"
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
            cout << "Version "
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
            std::clog << "command: " << args.command.name << "\t" << args.command.params << std::endl;

        // Get the input stream
        app_utils::input_stream is (args.verbose, args.input_fn);

        // Get the output stream
        app_utils::output_stream os (args.verbose, args.output_fn);

        using namespace spoc::app_utils;
        using namespace spoc::transform;

        if (args.command.name == "add-x")
        {
            std::string s = args.command.params;
            const auto v = consume_double (s);
            add_x (is (), os (), v);
        }
        else if (args.command.name == "add-y")
        {
            std::string s = args.command.params;
            const auto v = consume_double (s);
            add_y (is (), os (), v);
        }
        else if (args.command.name == "add-z")
        {
            std::string s = args.command.params;
            const auto v = consume_double (s);
            add_z (is (), os (), v);
        }
        else if (args.command.name == "quantize-xyz")
        {
            std::string s = args.command.params;
            const auto v = consume_double (s);
            quantize (is (), os (), v);
        }
        else if (args.command.name == "replace")
        {
            std::string s = args.command.params;
            const auto l = consume_field_name (s);
            const auto v1 = consume_int (s);
            const auto v2 = consume_int (s);
            replace (is (), os (), l, v1, v2);
        }
        else if (args.command.name == "rotate-x")
        {
            std::string s = args.command.params;
            const auto v = consume_double (s);
            rotate_x (is (), os (), v);
        }
        else if (args.command.name == "rotate-y")
        {
            std::string s = args.command.params;
            const auto v = consume_double (s);
            rotate_y (is (), os (), v);
        }
        else if (args.command.name == "rotate-z")
        {
            std::string s = args.command.params;
            const auto v = consume_double (s);
            rotate_z (is (), os (), v);
        }
        else if (args.command.name == "scale-x")
        {
            std::string s = args.command.params;
            const auto v = consume_double (s);
            scale_x (is (), os (), v);
        }
        else if (args.command.name == "scale-y")
        {
            std::string s = args.command.params;
            const auto v = consume_double (s);
            scale_y (is (), os (), v);
        }
        else if (args.command.name == "scale-z")
        {
            std::string s = args.command.params;
            const auto v = consume_double (s);
            scale_z (is (), os (), v);
        }
        else if (args.command.name == "set")
        {
            std::string s = args.command.params;
            const auto l = consume_field_name (s);
            const auto v = consume_double (s);
            spoc::transform::set (is (), os (), l, v);
        }
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
