#include "spoc.h"
#include "tile_cmd.h"
#include <algorithm>
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
                string (argv[0]) + " [options] < spocfile");

        // If you are getting help, exit without an error
        if (args.help)
            return 0;

        // Show args
        if (args.verbose)
        {
            clog << "verbose\t" << args.verbose << endl;
            clog << "force\t" << args.force << endl;
            clog << "tiles\t" << args.tiles << endl;
            clog << "tile-size\t" << args.tile_size << endl;
            clog << "prefix\t'" << args.prefix << "'" << endl;
        }

        if (args.verbose)
            clog << "Reading " << args.fn << endl;

        ifstream ifs (args.fn);

        if (!ifs)
            throw runtime_error ("Could not open file for reading");

        // Read into spoc_file struct
        spoc_file f = read_spoc_file (ifs);

        if (args.verbose)
            clog << "Total points " << f.get_npoints () << endl;

        const auto x = f.get_x ();
        const auto y = f.get_y ();

        if (x.empty ())
            throw std::runtime_error ("No X values");
        if (y.empty ())
            throw std::runtime_error ("No Y values");

        // Get minimum and maximum X and Y values
        const double minx = *std::min_element (begin(x), end(x));
        const double maxx = *std::max_element (begin(x), end(x));
        const double miny = *std::min_element (begin(y), end(y));
        const double maxy = *std::max_element (begin(y), end(y));

        // Get difference in X and Y extents
        //
        // Add a small amount to handle rounding
        const double dx = maxx - minx + std::numeric_limits<float>::epsilon ();
        const double dy = maxy - miny + std::numeric_limits<float>::epsilon ();

        // Determine dimension of one side of a tile
        const double units_per_tile = (args.tile_size > 0.0)
            ? args.tile_size
            : ((dx > dy) ? dx / args.tiles : dy / args.tiles);

        if (args.verbose)
            clog << "Tiles are " << units_per_tile << " X " << units_per_tile << " tiles" << endl;

        // Determine number of tiles
        const size_t xtiles = dx / units_per_tile;
        const size_t ytiles = dy / units_per_tile;
        const size_t total_tiles = xtiles * ytiles;

        if (args.verbose)
            clog << xtiles << " X " << ytiles << " = " << total_tiles << " total tiles" << endl;

        // Count the number of points in each tile
        std::vector<size_t> point_counts (xtiles * ytiles);

        if (args.verbose)
            clog << "Counting points in each tile" << endl;

        // For each point in the file...
        for (size_t n = 0; n < f.get_npoints (); ++n)
        {
            // Get point 'n'
            const auto p = f.get (n);

            // Determine which tile the point belongs to
            size_t nx = (p.x - minx) / units_per_tile;
            size_t ny = (p.y - miny) / units_per_tile;

            assert (nx < xtiles);
            assert (ny < ytiles);

            // Map tile to vector index
            const size_t point_counts_index = ny * xtiles + nx;
            assert (point_counts_index < point_counts.size ());

            // Count it
            ++point_counts[point_counts_index];
        }

        // Allocate vector of spoc_files, one for each tile
        std::vector<spoc_file> spoc_files (point_counts.size ());

        // Resize each spoc_file according to its counts
        for (size_t i = 0; i < spoc_files.size (); ++i)
            spoc_files.resize (point_counts[i]);

        // Keep track of the current spoc_file point index
        std::vector<size_t> current_point_indexes (spoc_files.size ());

        if (args.verbose)
            clog << "Splitting spoc file into tiles" << endl;

        // For each point in the file...
        for (size_t n = 0; n < f.get_npoints (); ++n)
        {
            // Get point 'n'
            const auto p = f.get (n);

            // Determine which tile the point belongs to
            size_t nx = (p.x - minx) / units_per_tile;
            size_t ny = (p.y - miny) / units_per_tile;

            assert (nx < xtiles);
            assert (ny < ytiles);

            // Map tile to vector index
            const size_t spoc_file_index = ny * xtiles + nx;
            assert (spoc_file_index < spoc_files.size ());
            assert (spoc_file_index < current_point_indexes.size ());

            // Get the current index for this spoc file, and update the index
            const size_t index = current_point_indexes[spoc_file_index]++;
            // Set the point in the appropriate spoc file
            spoc_files[spoc_file_index].set (index, p);
        }

        // Check the prefix
        auto prefix = args.prefix;
        if (prefix.empty ())
        {
            // TODO: use C++ pathname header instead
            //
            // It's empty, so use the filename
            string fn = args.fn;
            // Get rid of the path
            fn = fn.substr (fn.find_last_of("/\\") + 1);
            // Get rid of extension
            prefix = fn.substr (0, fn.find_last_of ("."));
        }

        assert (!prefix.empty ());

        // Write each tile
        if (args.verbose)
            clog << "Writing tiles" << endl;

        for (size_t i = 0; i < spoc_files.size (); ++i)
        {
            // Get the filename extension
            const string &fn = args.fn;
            const string ext = fn.substr (fn.find_last_of ("."));

            // Generate the filename
            std::stringstream sfn;

            // TODO determine how many significant digits are needed
            sfn << prefix << i << ext;

            // Check if file already exists
            if (!args.force)
            {
                if (args.verbose)
                    clog << "Checking if '" << sfn.str () << "' exists" << endl;
                ifstream tmp_ifs (sfn.str ());
                if (tmp_ifs.good())
                    throw std::runtime_error ("File already exists. "
                        "Use the force option to overwrite. "
                        "Aborting.");
            }

            if (args.verbose)
                clog << "Writing " << sfn.str () << endl;

            std::ofstream ofs (sfn.str ());

            if (!ofs)
                throw std::runtime_error ("Could not open file for writing");

            // Write it out
            assert (i < spoc_files.size ());
            spoc::write_spoc_file (ofs, spoc_files[i]);
        }

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
