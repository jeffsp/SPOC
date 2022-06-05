#include "spoc.h"
#include "tool.h"
#include "tool_cmd.h"
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace spoc;

const std::set<char> field_chars {'x', 'y', 'z', 'c', 'p', 'i', 'r', 'g', 'b'};

bool check_field (const char f)
{
    if (field_chars.find (f) == field_chars.end ())
        return false;
    return true;
}

char consume_field (std::string &s)
{
    // Get the field
    const char c = s[0];
    // Check it
    if (!check_field (c))
        throw std::runtime_error (std::string ("Invalid field name: ") + s);
    // Make sure the next char is a ','
    if (!check_field (c))
        throw std::runtime_error (std::string ("Invalid field specification: ") + s);
    s.erase (0, 2);
    return c;
}

int consume_int (std::string &s)
{
    size_t sz = 0;
    int v = 0.0;
    try { v = std::stoi (s, &sz); }
    catch (const std::invalid_argument &e) {
        throw std::runtime_error (std::string ("Could not parse field string: ") + s);
    }
    s.erase (0, sz + 1);
    return v;
}

double consume_double (std::string &s)
{
    size_t sz = 0;
    double v = 0.0;
    try { v = std::stod (s, &sz); }
    catch (const std::invalid_argument &e) {
        throw std::runtime_error (std::string ("Could not parse field string: ") + s);
    }
    s.erase (0, sz + 1);
    return v;
}

// Check arguments
void check (const cmd::args &args)
{
    if (args.command.name.empty ())
        throw std::runtime_error ("No command was specified");

    // Show args
    if (args.verbose)
        std::clog << "command: " << args.command.name << args.command.params << std::endl;
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

        // If you are getting help, exit without an error
        if (args.help)
            return 0;

        // Check the arguments
        check (args);

        // Read the input file
        spoc_file f;

        if (args.input_fn.empty ())
        {
            if (args.verbose)
                clog << "Reading from stdin" << endl;

            // Read into spoc_file struct
            f = read_spoc_file_uncompressed (cin);
        }
        else
        {
            if (args.verbose)
                clog << "Reading " << args.input_fn << endl;

            ifstream ifs (args.input_fn);

            if (!ifs)
                throw runtime_error ("Could not open file for reading");

            // Read into spoc_file struct
            f = read_spoc_file_uncompressed (ifs);
        }

        // The result goes here
        spoc_file g (f);

        using namespace spoc::transform;

        if (args.command.name == "set")
        {
            std::string s = args.command.params;
            const auto l = consume_field (s);
            const auto v = consume_double (s);
            g = spoc::transform::set (g, l, v);
        }
        else if (args.command.name == "replace")
        {
            std::string s = args.command.params;
            const auto l = consume_field (s);
            const auto v1 = consume_int (s);
            const auto v2 = consume_int (s);
            g = replace (g, l, v1, v2);
        }
        else if (args.command.name == "recenter-xy")
            g = recenter (g);
        else if (args.command.name == "recenter-xyz")
            g = recenter (g, true);
        else if (args.command.name == "subtract-min-xy")
            g = subtract_min (g);
        else if (args.command.name == "subtract-min-xyz")
            g = subtract_min (g, true);
        else if (args.command.name == "quantize-xyz")
        {
            std::string s = args.command.params;
            const auto v = consume_double (s);
            g = quantize (g, v);
        }
        else
            throw std::runtime_error ("An unknown command was encountered");

        // Write the output file
        if (args.output_fn.empty ())
        {
            if (args.verbose)
                clog << "Writing to stdout" << endl;

            // Write it out
            spoc::write_spoc_file_uncompressed (cout, g);
        }
        else
        {
            if (args.verbose)
                clog << "Writing " << args.output_fn << endl;

            ofstream ofs (args.output_fn);

            if (!ofs)
                throw runtime_error ("Could not open file for writing");

            // Write it out
            spoc::write_spoc_file_uncompressed (ofs, g);
        }

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
