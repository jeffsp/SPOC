#include "spoc.h"
#include "tile.h"
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

        // Get the length of one side of a tile
        const double tile_size = args.tile_size > 0.0
            ? args.tile_size
            : spoc::tile::get_tile_size (f.get_x (), f.get_y (), args.tiles);

        if (args.verbose)
            clog << "Tiles are " << tile_size << " X " << tile_size << endl;

        // Get the tile index of each point in the point cloud
        const auto indexes = spoc::tile::get_tile_indexes (f.get_x (), f.get_y (), tile_size);

        // Map each tile index to a vector of point cloud indexes
        const auto tile_map = spoc::tile::get_tile_map (indexes);

        // Write each tile
        if (args.verbose)
            clog << "Writing tiles" << endl;

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

        // For each map entry
        for (const auto &i : tile_map)
        {
            // Get tile number
            const auto n = i.first;

            // The the vector of point cloud indexes
            const auto &v = i.second;

            // The file to write
            spoc_file t;

            // Set the number of points in the file
            t.resize (v.size ());

            // For each point index in this tile...
            size_t point_index = 0;
            for (const auto j : v)
            {
                // Get point 'j'
                const auto p = f.get (j);
                t.set (point_index++, p);
            }

            // Get the filename extension
            const string &fn = args.fn;
            const string ext = fn.substr (fn.find_last_of ("."));

            // Generate the filename
            std::stringstream sfn;

            // TODO determine how many significant digits are needed
            sfn << prefix << n << ext;

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
            spoc::write_spoc_file (ofs, t);
        }

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
