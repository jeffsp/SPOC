#include "extent.h"
#include "merge.h"
#include "merge_cmd.h"
#include "spoc.h"
#include <iostream>
#include <stdexcept>

int main (int argc, char **argv)
{
    using namespace std;
    using namespace spoc;

    try
    {
        // Parse command line
        const cmd::args args = cmd::get_args (argc, argv,
                string (argv[0]) + " [options] spocfile1 spocfile2 [...]");

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
            throw std::runtime_error ("At least two input files are required to merge");

        // The result goes here
        spoc_file g;

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
            spoc_file f = read_spoc_file_uncompressed (ifs);

            // Set the wkt to the first file's wkt
            if (i == 0)
                g.set_wkt (f.get_header ().wkt);

            // Get the xyz's
            const auto p = f.get_point_records ();

            // Get the extent
            const auto e = spoc::extent::get_extent (p);

            // Get the area
            const auto a = (e.maxp.x - e.minp.x) * (e.maxp.y - e.minp.y);

            // Sum areas
            area_sum += a;

            // Get the point ID
            const auto id = args.point_id < 0 ? i : args.point_id;

            // Add it to the result
            merge::append (f, g, id, args.quiet);
        }

        // Check the area ratios
        if (!args.quiet)
        {
            // Get the points
            const auto p = g.get_point_records ();

            // Get the extent
            const auto e = spoc::extent::get_extent (p);

            // Get the area
            const auto a = (e.maxp.x - e.minp.x) * (e.maxp.y - e.minp.y);

            // What is the ratio of the total final area to the sum
            // of the areas of each individual file?
            const double r = a / area_sum;

            // If the area grew by too much, give a warning
            if (r > 100)
                std::clog
                    << "WARNING: 99% of the final merged area does not contain any points"
                    << std::endl;
        }

        if (args.verbose)
            clog << "Writing to stdout" << endl;

        // Write it out
        spoc::write_spoc_file_uncompressed (cout, g);

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
