#include "spoc/app_utils.h"
#include "spoc/spoc.h"
#include "tool.h"
#include "tool_cmd.h"
#include <cctype>
#include <iostream>
#include <stdexcept>

int main (int argc, char **argv)
{
    using namespace std;
    using namespace spoc::app_utils;
    using namespace spoc::file;
    using namespace spoc::io;
    using namespace spoc::tool_app;
    using namespace spoc::tool_cmd;

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
        if (args.command.name.empty ())
            throw runtime_error ("No command was specified");

        // Show args
        if (args.verbose)
        {
            clog << "verbose\t" << args.verbose << endl;
            clog << "field_fn\t'" << args.field_fn << "'" << endl;
            clog << "input_fn\t'" << args.input_fn << "'" << endl;
            clog << "output_fn\t'" << args.output_fn << "'" << endl;
            clog << "resolution\t'" << args.resolution << "'" << endl;
            clog << "command: " << args.command.name << "=" << args.command.params << endl;
        }

        // Get the input stream
        input_stream is (args.verbose, args.input_fn);

        // Read the input file
        spoc_file f = read_spoc_file (is ());

        if (args.command.name == "get-field")
        {
            string s = args.command.params;
            const auto l = consume_field_name (s);
            get_field (f, cout, l);

            // Short-circuit so that the SPOC file is not written
            return 0;
        }
        else if (args.command.name == "recenter-xy")
            f = recenter (f);
        else if (args.command.name == "recenter-xyz")
            f = recenter (f, true);
        else if (args.command.name == "resize-extra")
        {
            const int new_size = std::stoi (args.command.params);
            if (new_size < 0)
                throw runtime_error ("The number of extra fields must be >= 0");
            if (new_size > 255)
                throw runtime_error ("The number of extra fields must be < 256");
            f = resize_extra (f, new_size);
        }
        else if (args.command.name == "set-field")
        {
            if (args.field_fn.empty ())
                throw runtime_error ("You must set the 'field-filename' option when using the 'set-field' command");
            if (args.verbose)
                clog << "Opening " << args.field_fn << " for reading" << endl;
            ifstream field_ifs (args.field_fn);
            if (!field_ifs)
                throw runtime_error ("Could not open file for reading");
            string s = args.command.params;
            const auto l = consume_field_name (s);
            f = set_field (f, field_ifs, l);
        }
        else if (args.command.name == "subtract-min-xy")
            f = subtract_min (f);
        else if (args.command.name == "subtract-min-xyz")
            f = subtract_min (f, true);
        else if (args.command.name == "upsample-classifications")
        {
            // Check the resolution parameter
            if (args.resolution <= 0.0)
                throw runtime_error ("The 'resolution' option must be set to a value > 0.0 when using the 'upsample' command");

            // Open the file
            const auto fn = args.command.params;
            if (args.verbose)
                clog << "Opening " << fn << " for reading" << endl;
            ifstream ifs (fn);
            if (!ifs)
                throw runtime_error ("Could not open file for reading");
            // Read the spoc file
            const auto g = read_spoc_file (ifs);
            f = upsample_classifications (f, g, args.resolution, args.verbose, std::clog);
        }
        else
            throw runtime_error ("An unknown command was encountered");

        // Get the output stream
        output_stream os (args.verbose, args.output_fn);

        // Write it out
        write_spoc_file (os (), f);

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
