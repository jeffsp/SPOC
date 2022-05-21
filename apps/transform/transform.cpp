#include "spoc.h"
#include "transform.h"
#include "transform_cmd.h"
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace spoc;

// Check arguments
void check (const cmd::args &args)
{
    // Show args
    if (args.verbose)
    {
        clog << "verbose\t" << args.verbose << endl;
        clog << "field-name\t'" << args.field_name << "'" << endl;
        if (args.set_flag)
            clog << "set\t" << args.set_value << endl;
        for (auto i : args.replace_pairs)
            clog << "replace\t" << i.first << "->" << i.second << endl;
    }

    if (args.set_flag && !args.replace_pairs.empty ())
        throw runtime_error ("You can't use the set option together with the replace option");
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
            f = read_spoc_file (cin);
        }
        else
        {
            if (args.verbose)
                clog << "Reading " << args.input_fn << endl;

            ifstream ifs (args.input_fn);

            if (!ifs)
                throw runtime_error ("Could not open file for reading");

            // Read into spoc_file struct
            f = read_spoc_file (ifs);
        }

        // The result goes here
        spoc_file g;

        if (args.set_flag)
            g = spoc::transform::run_set_command (f, args.field_name, args.set_value);
        if (!args.replace_pairs.empty ())
            g = spoc::transform::run_replace_command (f, args.field_name, args.replace_pairs);
        if (args.recenter)
            g = spoc::transform::recenter (f);
        if (args.recenter_xyz)
            g = spoc::transform::recenter (f, true);

        // Write the output file
        if (args.output_fn.empty ())
        {
            if (args.verbose)
                clog << "Writing to stdout" << endl;

            // Write it out
            spoc::write_spoc_file (cout, g);
        }
        else
        {
            if (args.verbose)
                clog << "Writing " << args.output_fn << endl;

            ofstream ofs (args.output_fn);

            if (!ofs)
                throw runtime_error ("Could not open file for writing");

            // Write it out
            spoc::write_spoc_file (ofs, g);
        }

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
