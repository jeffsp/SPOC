#include "spoc.h"
#include "tile.h"
#include "tile_cmd.h"
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>

int main (int argc, char **argv)
{
    using namespace std;
    using namespace spoc::extent;
    using namespace spoc::file;
    using namespace spoc::io;
    using namespace spoc::tile_app;
    using namespace spoc::tile_cmd;

    try
    {
        // Parse command line
        const args args = get_args (argc, argv,
                string (argv[0]) + " [options] < spocfile");

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
            clog << "force\t" << args.force << endl;
            clog << "tiles\t" << args.tiles << endl;
            clog << "digits\t" << args.digits << endl;
            clog << "tile-size\t" << args.tile_size << endl;
            clog << "tile-size-x\t" << args.tile_size_x << endl;
            clog << "tile-size-y\t" << args.tile_size_y << endl;
            clog << "prefix\t'" << args.prefix << "'" << endl;
            clog << "Reading " << args.fn << endl;
        }

        ifstream ifs (args.fn);

        if (!ifs)
            throw runtime_error ("Could not open file for reading");

        // Read into spoc_file struct
        spoc_file f = read_spoc_file (ifs);

        if (args.verbose)
            clog << "Total points " << f.get_point_records ().size () << endl;

        // Check the arguments
        if (args.tile_size > 0.0 && args.tile_size_x > 0.0)
            throw runtime_error ("You can't specify both 'tile-size' and 'tile-size-x'");
        if (args.tile_size > 0.0 && args.tile_size_y > 0.0)
            throw runtime_error ("You can't specify both 'tile-size' and 'tile-size-y'");

        // Get the extent
        const auto e = get_extent (f.get_point_records ());

        // Get independent sizes
        double tile_size_x = -1.0;
        double tile_size_y = -1.0;

        // Get the tile size when X and Y are not independent
        if (args.tile_size > 0.0)
        {
            tile_size_x = args.tile_size;
            tile_size_y = args.tile_size;
        }
        else if (args.tile_size_x > 0.0 || args.tile_size_y > 0.0)
        {
            tile_size_x = args.tile_size_x;
            tile_size_y = args.tile_size_y;
        }
        else
        {
            const auto tile_size = get_tile_size (e, args.tiles);
            tile_size_x = tile_size;
            tile_size_y = tile_size;

        }

        if (args.verbose)
            clog << "Tiles are " << tile_size_x << " X " << tile_size_y << endl;

        // Get the tile index of each point in the point cloud
        const auto indexes = get_tile_indexes (f.get_point_records (), e, tile_size_x, tile_size_y);

        // Map each tile index to a vector of point cloud indexes
        const auto tile_map = get_tile_map (indexes);

        // Write each tile
        if (args.verbose)
            clog << "Writing tiles" << endl;

        // Check the prefix
        const auto prefix = args.prefix.empty ()
            ? string (filesystem::path (args.fn).stem ())
            : args.prefix;

        assert (!prefix.empty ());

        // Get the map keys
        vector<size_t> keys;
        for (const auto &i : tile_map)
            keys.push_back (i.first); // cppcheck-suppress useStlAlgorithm

        // Sort the map keys
        sort (begin (keys), end (keys));

        // For each tile number
        for (const auto tile : keys)
        {
            // Get the vector of point cloud indexes in this tile
            const auto &v = tile_map.at (tile);

            // It won't have an entry in the map if the vector is empty
            assert (!v.empty ());

            // The file to write
            spoc_file t;

            // Set the SRS
            t.set_wkt (f.get_wkt ());

            // Set the compression bit
            t.set_compressed (f.get_compressed ());

            // Set the number of points in the file
            t.resize (v.size ());

            // For each point index in this tile...
            for (size_t j = 0; j < v.size (); ++j)
            {
                // Get the point from the input file
                const auto p = f.get_point_record (v[j]);

                // Set it in the output file
                t.set (j, p);
            }

            // Get the filename extension
            const string ext = filesystem::path (args.fn).extension();

            // Generate the filename
            stringstream sfn;
            sfn << prefix;
            sfn << setw(args.digits) << setfill('0') << tile;
            sfn << ext;

            // Check if file already exists
            if (!args.force)
            {
                if (args.verbose)
                    clog << "Checking if '" << sfn.str () << "' exists" << endl;
                ifstream tmp_ifs (sfn.str ());
                if (tmp_ifs.good())
                    throw runtime_error ("File already exists. "
                        "Use the force option to overwrite. "
                        "Aborting.");
            }

            if (args.verbose)
                clog << "Writing " << sfn.str () << endl;

            ofstream ofs (sfn.str ());

            if (!ofs)
                throw runtime_error ("Could not open file for writing");

            // Write it out
            write_spoc_file (ofs, t);
        }

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
