#include "spoc.h"
#include "tile.h"
#include "tile_cmd.h"
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
                string (argv[0]) + " [options] < spocfile");

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

        // Show args
        if (args.verbose)
        {
            clog << "verbose\t" << args.verbose << endl;
            clog << "force\t" << args.force << endl;
            clog << "tiles\t" << args.tiles << endl;
            clog << "digits\t" << args.digits << endl;
            clog << "tile-size\t" << args.tile_size << endl;
            clog << "prefix\t'" << args.prefix << "'" << endl;
        }

        if (args.verbose)
            clog << "Reading " << args.fn << endl;

        ifstream ifs (args.fn);

        if (!ifs)
            throw runtime_error ("Could not open file for reading");

        // Read into spoc_file struct
        spoc_file f = read_spoc_file_uncompressed (ifs);

        if (args.verbose)
            clog << "Total points " << f.get_point_records ().size () << endl;

        // Get the length of one side of a tile
        const auto x = get_x (f.get_point_records ());
        const auto y = get_y (f.get_point_records ());
        const double tile_size = args.tile_size > 0.0
            ? args.tile_size
            : spoc::tile::get_tile_size (x, y, args.tiles);

        if (args.verbose)
            clog << "Tiles are " << tile_size << " X " << tile_size << endl;

        // Get the tile index of each point in the point cloud
        const auto indexes = spoc::tile::get_tile_indexes (x, y, tile_size);

        // Map each tile index to a vector of point cloud indexes
        const auto tile_map = spoc::tile::get_tile_map (indexes);

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
            t.set_wkt (f.get_header ().wkt);

            // Set the number of points in the file
            t.resize (v.size ());

            // For each point index in this tile...
            for (size_t j = 0; j < v.size (); ++j)
            {
                // Get the point from the input file
                const auto p = f.get_point_record (j);

                // Set it in the output file
                t.set_point_record (j, p);
            }

            // Get the filename extension
            const string ext = std::filesystem::path (args.fn).extension();

            // Generate the filename
            std::stringstream sfn;
            sfn << prefix;
            sfn << std::setw(args.digits) << std::setfill('0') << tile;
            sfn << ext;

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
            spoc::write_spoc_file_uncompressed (ofs, t);
        }

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
