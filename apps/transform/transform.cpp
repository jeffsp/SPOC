#include "app_utils.h"
#include "spoc.h"
#include "transform.h"
#include "transform_cmd.h"
#include <cctype>
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace spoc;

string consume_field_name (std::string &s)
{
    // Get the field
    size_t i = 0;
    char c = s[i];
    string field_name;
    while (isalnum (c))
    {
        field_name += c;
        ++i;
    }

    // Make sure it's a valid name
    if (!check_field_name (field_name))
        throw runtime_error (std::string ("Invalid field name: " + field_name));

    // Skip over the field name
    s.erase (0, i);

    return field_name;
}

int consume_int (std::string &s)
{
    size_t sz = 0;
    int v = 0.0;
    try { v = std::stoi (s, &sz); }
    catch (...) { throw std::runtime_error (std::string ("Could not parse field string: ") + s); }
    s.erase (0, sz + 1);
    return v;
}

double consume_double (std::string &s)
{
    size_t sz = 0;
    double v = 0.0;
    try { v = std::stod (s, &sz); }
    catch (...) { throw std::runtime_error (std::string ("Could not parse field string: ") + s); }
    s.erase (0, sz + 1);
    return v;
}

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

        using namespace spoc::transform;

        if (args.command.name == "quantize-xyz")
        {
            std::string s = args.command.params;
            const auto v = consume_double (s);
            quantize (is (), os (), h, v);
        }
        else if (args.command.name == "replace")
        {
            std::string s = args.command.params;
            const auto l = consume_field_name (s);
            const auto v1 = consume_int (s);
            const auto v2 = consume_int (s);
            replace (is (), os (), h, l, v1, v2);
        }
        else if (args.command.name == "set")
        {
            std::string s = args.command.params;
            const auto l = consume_field_name (s);
            const auto v = consume_double (s);
            spoc::transform::set (is (), os (), h, l, v);
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
