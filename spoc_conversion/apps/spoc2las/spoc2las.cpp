#include "app_utils.h"
#include "spoc.h"
#include "spoc2las.h"
#include "spoc2las_cmd.h"
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string>

int main (int argc, char **argv)
{
    using namespace std;
    using namespace spoc::extent;
    using namespace spoc::io;
    using namespace spoc::spoc2las_app;
    using namespace spoc::spoc2las_cmd;

    try
    {
        // Parse command line
        const args args = get_args (argc, argv,
                string (argv[0]) + " [options] spocfile lasfile");

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

        if (args.verbose)
            clog << "reading " << args.input_fn << endl;

        ifstream ifs (args.input_fn);

        if (!ifs)
            throw runtime_error ("Can't open file for reading");

        // Read spoc file
        const auto f = read_spoc_file (ifs);

        if (args.verbose)
        {
            clog << f.get_point_records ().size () << " point records read" << endl;
            clog << "read " << f.get_header ().wkt.size () << " byte WKT" << endl;
        }

        // Get the extent
        const auto e = get_extent (f.get_point_records ());

        // Create the header
        LASheader lasheader;
        strcpy (lasheader.system_identifier, "SPOC by spocfile.xyz");
        strcpy (lasheader.generating_software, "spoc2las");
        const auto now = chrono::system_clock::now ();
        time_t tt = chrono::system_clock::to_time_t(now);
        tm local_tm = *localtime(&tt);
        lasheader.file_creation_day = local_tm.tm_yday;
        lasheader.file_creation_year = local_tm.tm_year + 1900;
        lasheader.x_offset = e.minp.x;
        lasheader.y_offset = e.minp.y;
        lasheader.z_offset = e.minp.z;
        
        // According to the asprs las specification
        // https://www.asprs.org/wp-content/uploads/2010/12/asprs_las_format_v12.pdf
        // X, Y, and Z scale factors: The scale factor fields contain a double floating point value that is used
        // to scale the corresponding X, Y, and Z long values within the point records. The corresponding
        // X, Y, and Z scale factor must be multiplied by the X, Y, or Z point record value to get the actual
        // X, Y, or Z coordinate. For example, if the X, Y, and Z coordinates are intended to have two
        // decimal point values, then each scale factor will contain the number 0.01.
        //
        // So we set the scale factor to be as low as we can while maintaining the range we need
        const double safety_factor = 2.0;
        const double max_val = std::numeric_limits<int32_t>().max();
        const double x_rng = e.maxp.x - e.minp.x;
        const double y_rng = e.maxp.y - e.minp.y;
        const double z_rng = e.maxp.z - e.minp.z;
        lasheader.x_scale_factor = safety_factor * x_rng / max_val;
        lasheader.y_scale_factor = safety_factor * y_rng / max_val;
        lasheader.z_scale_factor = safety_factor * z_rng / max_val;

        lasheader.point_data_format = 2;
        lasheader.point_data_record_length = 26;

        const auto wkt = f.get_header ().wkt;
        if (!wkt.empty ())
            lasheader.set_geo_ogc_wkt (wkt.size (), wkt.data ());

        // Initialize the point
        LASpoint laspoint;
        laspoint.init(&lasheader, lasheader.point_data_format, lasheader.point_data_record_length, 0);

        // Write them to specified file
        if (args.verbose)
            clog << "writing records to " << args.output_fn << endl;

        las_writer l (args.output_fn, lasheader);

        for (size_t i = 0; i < f.get_point_records ().size (); ++i)
        {
            const auto p = f.get_point_records ()[i];
            laspoint.set_x (p.x);
            laspoint.set_y (p.y);
            laspoint.set_z (p.z);
            laspoint.set_classification (p.c);
            laspoint.set_point_source_ID (p.p);
            laspoint.set_intensity (p.i);
            laspoint.rgb[0] = p.r;
            laspoint.rgb[1] = p.g;
            laspoint.rgb[2] = p.b;

            l.laswriter->write_point (&laspoint);
            l.laswriter->update_inventory (&laspoint);
        }

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
