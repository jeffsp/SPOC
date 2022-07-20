#include "merge.h"
#include "merge_cmd.h"
#include "spoc.h"
#include <iostream>
#include <stdexcept>

int main (int argc, char **argv)
{
    using namespace std;
    using namespace spoc::extent;
    using namespace spoc::io;
    using namespace spoc::merge_app;
    using namespace spoc::merge_cmd;

    try
    {
        // Parse command line
        const args args = get_args (argc, argv,
                string (argv[0]) + " [options] spocfile1 spocfile2 [...]");

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

        // Show args
        if (args.verbose)
        {
            clog << "verbose\t" << args.verbose << endl;
            clog << "quiet\t" << args.quiet << endl;
            clog << "point-id\t" << args.point_id << endl;
            clog << "filenames\t" << args.fns.size () << endl;
        }

        // Check args
        if (args.fns.size () < 2)
            throw runtime_error ("At least two input files are required to merge");

        // The result goes here
        spoc_file g;

        // Set the compression bit
        //
        // Assume that all of the inputs are compressed
        g.set_compressed (true);

        double area_sum = 0.0;

        for (size_t i = 0; i < args.fns.size (); ++i)
        {
            // Get the filename
            const auto fn = args.fns[i];

            // Read it in
            if (args.verbose)
                clog << "Reading " << fn << endl;

            ifstream ifs (fn);

            if (!ifs)
                throw runtime_error ("Could not open file for reading");

            // Read into spoc_file struct
            spoc_file f = read_spoc_file (ifs);

            // Set the compression bit
            //
            // If any of the inputs are uncompressed, the output will be
            // uncompressed. Otherwise, if all inputs are compressed,
            // the output will be compressed.
            if (f.get_compressed () == false)
                g.set_compressed (false);

            // Set the wkt to the first file's wkt
            if (i == 0)
                g.set_wkt (f.get_wkt ());

            // Get the xyz's
            const auto p = f.get_point_records ();

            // Get the extent
            const auto e = get_extent (p);

            // Get the area
            const auto a = (e.maxp.x - e.minp.x) * (e.maxp.y - e.minp.y);

            // Sum areas
            area_sum += a;

            // Get the point ID
            const auto id = args.point_id < 0 ? i : args.point_id;

            // Add it to the result
            append (f, g, id, args.quiet);
        }

        // Check the area ratios
        if (!args.quiet)
        {
            // Get the points
            const auto p = g.get_point_records ();

            // Get the extent
            const auto e = get_extent (p);

            // Get the area
            const auto a = (e.maxp.x - e.minp.x) * (e.maxp.y - e.minp.y);

            // What is the ratio of the total final area to the sum
            // of the areas of each individual file?
            const double r = a / area_sum;

            // If the area grew by too much, give a warning
            if (r > 100)
                clog
                    << "WARNING: 99% of the final merged area does not contain any points"
                    << endl;
        }

        if (args.verbose)
            clog << "Writing to stdout" << endl;

        // Write it out
        write_spoc_file (cout, g);

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
