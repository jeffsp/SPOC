#include "cmd.h"
#include "colors.h"
#include "palette.h"
#include "spoc/spoc.h"
#include "version.h"
#include "vtk_interactor.h"

using namespace std;

const std::string usage ("Usage: spoc_viewer input.spoc");

int main (int argc, char **argv)
{
    using namespace std;
    using namespace spoc_viewer::cmd;
    using namespace spoc_viewer::colors;
    using namespace spoc_viewer::palette;
    using namespace spoc::file;
    using namespace spoc::io;
    using namespace spoc::point_record;

    try
    {
        // Parse command line
        args args = get_args (argc, argv, usage);

        // If you are getting help, exit without an error
        if (args.help)
            return 0;

        // Show the args
        if (args.verbose)
        {
            clog << boolalpha;
            clog << "spoc viewer version "
                << static_cast<int> (spoc_viewer::MAJOR_VERSION)
                << "."
                << static_cast<int> (spoc_viewer::MINOR_VERSION)
                << std::endl;
            clog << "spoc version "
                << static_cast<int> (spoc::MAJOR_VERSION)
                << "."
                << static_cast<int> (spoc::MINOR_VERSION)
                << std::endl;
            clog << "SPOC filenames " << args.spoc_filenames.size () << endl;
            for (auto f : args.spoc_filenames)
                clog << "\t" << f << endl;
            clog << "Palette_filenames " << args.palette_filenames.size ()<< endl;
            for (auto p : args.palette_filenames)
                clog << "\t" << p << endl;
            clog << "Color_modes " << args.color_modes.size () << endl;
            for (auto c : args.color_modes)
                clog << "\t" << c << endl;
            clog << "Resolution " << args.resolution << endl;
            clog << "Camera_coordinates '" << args.camera_coordinates << "'" << endl;
        }

        // The points to view
        point_records prs;

        // The point colors
        vector<rgb_triplet> rgbs;

        // The current color mode
        char color_mode = 'c';

        // The current palette
        vector<rgb_triplet> palette;

        for (size_t n = 0; n < args.spoc_filenames.size (); ++n)
        {
            // Get the filename
            const auto fn = args.spoc_filenames[n];

            // Get the color mode
            if (n < args.color_modes.size ())
            {
                // Check the color mode
                if (args.color_modes[n].size () != 1)
                    throw runtime_error ("Invalid color-mode specification:" + args.color_modes[n]);

                // Accept upper- or lower-case specification for color_mode
                color_mode = ::tolower (args.color_modes[n][0]);
            }

            // Get the palette
            if (n < args.palette_filenames.size ())
            {
                if (args.verbose)
                    clog << "Reading palette " << args.palette_filenames[n] << endl;

                // Read from specified file
                palette = read_rgb_palette (args.palette_filenames[n]);
            }
            else
            {
                // Read the default palette for this color mode
                switch (color_mode)
                {
                    default:
                    throw runtime_error ("Unknown color-mode: " + color_mode);
                    case 's': // Classification shaded with intensity
                    case 'g': // Classification shaded with green band
                    case 'c': // Classification
                    palette = get_default_classification_palette ();
                    break;
                    case 'e': // Elevation
                    palette = get_default_elevation_palette ();
                    break;
                    case 'i': // Intensity
                    palette = get_default_intensity_palette ();
                    break;
                    case 'r': // Region
                    palette = get_default_region_palette ();
                    break;
                    case 'x': // Region
                    palette = get_default_region_palette_random ();
                    break;
                    case 'b': // RGB
                    palette = get_default_intensity_palette ();
                    break;
                    case '0': // Extra field
                    case '1': // Extra field
                    case '2': // Extra field
                    case '3': // Extra field
                    case '4': // Extra field
                    case '5': // Extra field
                    case '6': // Extra field
                    case '7': // Extra field
                    case '8': // Extra field
                    case '9': // Extra field
                    palette = get_default_elevation_palette ();
                    break;
                }
            }

            // Read the input file
            clog << "Reading " << fn << endl;
            ifstream ifs (fn);

            if (!ifs)
                throw runtime_error ("Could not open file for reading");

            spoc_file f = read_spoc_file (ifs);

            if (args.verbose)
                clog << f.get_point_records ().size () << " points read" << endl;

            // Get the new point records
            point_records new_prs;

            if (args.resolution > 0.0)
            {
                if (args.verbose)
                    clog << "Subsampling points" << endl;

                // Get the indexes into f
                const auto indexes = spoc::subsampling::get_subsample_indexes (f.get_point_records (), args.resolution, 123456);

                if (args.verbose)
                    clog << "Total voxelized points " << indexes.size () << endl;

                // Add them
                for (auto i : indexes)
                    new_prs.push_back (f.get_point_records ()[i]);
            }
            else
            {
                // Move them out of the point cloud
                new_prs = f.move_point_records ();
            }

            // Get RGB values for the new point records
            assert (!palette.empty ());

            if (args.verbose)
                clog << "Getting colors for color mode '" << color_mode << "'" << endl;

            // Get the colors for the new points
            decltype(rgbs) new_rgbs;
            switch (color_mode)
            {
                default:
                throw std::runtime_error ("Unknown color-mode");
                case 's': // Classification shaded with intensity
                new_rgbs = get_shaded_classification_colors (new_prs, palette);
                break;
                case 'c': // Classification
                new_rgbs = get_classification_colors (new_prs, palette);
                break;
                case 'e': // Elevation
                new_rgbs = get_elevation_colors (new_prs, palette);
                break;
                case 'i': // Intensity
                new_rgbs = get_intensity_colors (new_prs, palette);
                break;
                case 'r': // Region
                new_rgbs = get_region_colors (new_prs, palette, spoc_viewer::palette::region_palette_indexer);
                break;
                case 'x': // Region
                new_rgbs = get_region_colors_random (new_prs, palette);
                break;
                case 'g': // Classification shaded with green, in case intensity doesn't exist (photogrammetry)
                for (size_t i = 0; i < new_prs.size(); ++i)
                    new_prs[i].i = new_prs[i].g;
                new_rgbs = get_shaded_classification_colors (new_prs, palette);
                break;
                case 'b': // RGB
                new_rgbs = get_rgb_colors (new_prs);
                break;
                case '0': // Extra field
                case '1': // Extra field
                case '2': // Extra field
                case '3': // Extra field
                case '4': // Extra field
                case '5': // Extra field
                case '6': // Extra field
                case '7': // Extra field
                case '8': // Extra field
                case '9': // Extra field
                new_rgbs = get_extra_field_colors (new_prs, palette, color_mode - '0');
                break;
            }

            // There should be one RGB per point
            assert (new_prs.size () == new_rgbs.size ());

            // Add the new point records
            prs.insert (prs.end (), new_prs.begin (), new_prs.end ());

            // Add the new RGB values
            rgbs.insert (rgbs.end (), new_rgbs.begin (), new_rgbs.end ());
        }

        if (args.verbose)
            clog << "Total points " << prs.size () << endl;

        // There should be one RGB per point
        assert (prs.size () == rgbs.size ());

        const string fn = args.spoc_filenames.size () == 1
            ? args.spoc_filenames[0]
            : string ("...");

        // View them
        spoc_viewer::vtk_interactor::start_interactor (prs,
                rgbs,
                args.camera_coordinates,
                fn,
                args.resolution,
                args.box_mode,
                args.opacity,
                args.point_size);

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
