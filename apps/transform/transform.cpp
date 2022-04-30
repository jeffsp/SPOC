#include "spoc.h"
#include "transform.h"
#include "transform_cmd.h"
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <limits>
#include <sstream>
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

        // If you are getting help, exit without an error
        if (args.help)
            return 0;

        // Show args
        if (args.verbose)
        {
            clog << "verbose\t" << args.verbose << endl;
            clog << "input-pipe-name\t'" << args.input_pipe_name << "'" << endl;
            clog << "output-pipe-name\t'" << args.output_pipe_name << "'" << endl;
        }

        // Input file
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

        // Open the pipes
        if (args.input_pipe_name.empty ())
            throw runtime_error ("No input pipe name was specified");

        if (args.output_pipe_name.empty ())
            throw runtime_error ("No output pipe name was specified");

        // The transformer writes to the filter's input pipe
        if (args.verbose)
            clog << "Opening " << args.input_pipe_name << " for writing" << endl;

        ofstream ips (args.input_pipe_name);

        if (!ips)
            throw runtime_error ("Could not open file for writing");

        // The transformer reads from the filter's output pipe
        if (args.verbose)
            clog << "Opening " << args.output_pipe_name << " for reading" << endl;

        ifstream ops (args.output_pipe_name);

        if (!ops)
            throw runtime_error ("Could not open file for reading");

        if (args.verbose)
            clog << "Processing " << f.get_npoints () << " point records" << endl;

        spoc_file g = spoc::transform::process (f, ips, ops);

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
