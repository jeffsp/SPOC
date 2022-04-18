#include "spoc.h"
#include "spoc_tile_cmd.h"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace spoc;

int main (int argc, char **argv)
{
    using namespace std;
    using namespace spoc;

    try
    {
        // Parse command line
        cmd::args args = cmd::get_args (argc, argv,
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

        /*
        // Open the las file
        using P = point3d<point_data>;
        using L = las_file<P>;

        if (args.verbose)
            clog << "Opening " << args.input_filename << endl;

        L las (args.input_filename);

        if (args.verbose)
            clog << "Reading points " << endl;

        auto pc = las.get_points ();

        if (args.verbose)
            clog << "Total points " << pc.size () << endl;

        const double resolution = 1.0;
        const auto min = get_rounded_min_coords (pc, resolution);
        const auto max = get_rounded_max_coords (pc, resolution);
        const auto delta_x = max.x - min.x;
        const auto delta_y = max.y - min.y;

        if (args.verbose)
        {
            write_las_info (clog, las);
            write_pc_info (clog, pc);
        }

        // Determine dimension of one side in meters -- always round up
        // to the next highest integral number of meters.
        double meters_per_tile = std::ceil (delta_x > delta_y ? delta_x / args.tiles : delta_y / args.tiles);

        if (args.verbose)
            clog << meters_per_tile << " X " << meters_per_tile << " meter tiles" << endl;

        // Determine number of tiles
        const size_t x_tiles = std::ceil (delta_x / meters_per_tile);
        const size_t y_tiles = std::ceil (delta_y / meters_per_tile);
        const size_t total_tiles = x_tiles * y_tiles;

        if (args.verbose)
            clog << x_tiles << " X " << y_tiles << " = " << total_tiles << " total tiles" << endl;

        // Allocate 2D grid of tiles
        raster<vector<P>> pcs (y_tiles, x_tiles);

        // Split into tiles
        if (args.verbose)
            clog << "Splitting point cloud into tiles" << endl;

        // For each point in the point cloud
        for (auto p : pc)
        {
            // Get the point
            const auto x = p.x;
            const auto y = p.y;

            assert (min.y <= y);
            assert (min.y <= y);

            // Determine grid cell
            size_t row = (y - min.y) / meters_per_tile;
            size_t col = (x - min.x) / meters_per_tile;

            // Handle special cases
            if (row > 0 && row == pcs.rows ())
                --row;
            if (col > 0 && col == pcs.cols ())
                --col;

            assert (row < pcs.rows ());
            assert (col < pcs.cols ());

            // Insert it in at this grid cell
            pcs (row, col).push_back (p);
        }

        // Check the prefix
        if (args.prefix.empty ())
        {
            // It's empty, so use the filename
            string fn = args.input_filename;
            // Get rid of the path
            fn = fn.substr (fn.find_last_of("/\\") + 1);
            // Get rid of extension
            args.prefix = fn.substr (0, fn.find_last_of ("."));
        }

        assert (!args.prefix.empty ());

        // Write each tile
        if (args.verbose)
            clog << "Writing tiles" << endl;

        for (size_t i = 0; i < total_tiles; ++i)
        {
            // Get the filename extension
            const string &fn = args.input_filename;
            const string ext = fn.substr (fn.find_last_of ("."));

            // Generate the filename
            stringstream sfn;
            sfn << args.prefix << i << ext;

            // Check if file already exists, but only if we might prompt
            if (!args.yes)
            {
                if (args.verbose)
                    clog << "Checking " << sfn.str () << endl;
                ifstream f (sfn.str ());
                if (f.good() && !yes_no_prompt ("File exists. Overwrite? [yn]"))
                    continue;
            }

            if (args.verbose)
                clog << "Writing " << sfn.str () << endl;

            // Set the points for this tile
            las.set_points (pcs[i]);

            // Write it
            las.write (sfn.str ());
        /////
        */

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
