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

// Helper class to read from either cin or an input file
class input_stream
{
    public:
    input_stream (const bool verbose, const string &fn)
    {
        if (fn.empty ())
        {
            if (verbose)
                clog << "Reading from stdin" << endl;
            is = &cin;
        }
        else
        {
            if (verbose)
                clog << "Reading from " << fn << endl;
            ifs.open (fn);
            if (!ifs)
                throw runtime_error ("Could not open file for reading");
            is = &ifs;
        }
    }
    std::istream &operator() ()
    {
        return *is;
    }
    private:
    std::istream *is;
    std::ifstream ifs;
};

// Helper class to write to either cout or an output file
class output_stream
{
    public:
    output_stream (const bool verbose, const string &fn)
    {
        if (fn.empty ())
        {
            if (verbose)
                clog << "Writing to stdout" << endl;
            os = &cout;
        }
        else
        {
            if (verbose)
                clog << "Writing to " << fn << endl;
            ofs.open (fn);
            if (!ofs)
                throw runtime_error ("Could not open file for writing");
            os = &ofs;
        }
    }
    std::ostream &operator() ()
    {
        return *os;
    }
    private:
    std::ostream *os;
    std::ofstream ofs;
};

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

        // Get the input stream
        input_stream is (args.verbose, args.input_fn);

        // Read the header
        const header h = read_header (is ());

        // Check compression flag
        if (h.compressed)
            throw std::runtime_error ("Expected an uncompressed file");

        // Get the output stream
        output_stream os (args.verbose, args.output_fn);

        using namespace spoc::transform;

        if (args.command.name == "quantize-xyz")
        {
            std::string s = args.command.params;
            const auto v = consume_double (s);
            quantize (is (), os (), h, v);
        }
        else if (args.command.name == "recenter-xy")
            recenter (is (), os (), h);
        else if (args.command.name == "recenter-xyz")
            recenter (is (), os (), h, true);
        else if (args.command.name == "replace")
        {
            std::string s = args.command.params;
            const auto l = consume_field (s);
            const auto v1 = consume_int (s);
            const auto v2 = consume_int (s);
            replace (is (), os (), h, l, v1, v2);
        }
        else if (args.command.name == "set")
        {
            std::string s = args.command.params;
            const auto l = consume_field (s);
            const auto v = consume_double (s);
            spoc::transform::set (is (), os (), h, l, v);
        }
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
