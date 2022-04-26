#include "spoc2las.h"
#include "spoc2las_cmd.h"
#include "spoc.h"
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string>

int main (int argc, char **argv)
{
    using namespace std;
    using namespace spoc;

    try
    {
        // Parse command line
        const cmd::args args = cmd::get_args (argc, argv,
                string (argv[0]) + " [options] spocfile lasfile");

        // If you are getting help, exit without an error
        if (args.help)
            return 0;

        if (args.verbose)
            clog << "reading " << args.input_fn << endl;

        ifstream ifs (args.input_fn);

        if (!ifs)
            throw runtime_error ("Can't open file for reading");

        // Read spoc file
        string wkt;
        auto point_records = read_spoc_file (ifs, wkt);

        if (args.verbose)
        {
            clog << point_records.size () << " point records read" << endl;
            clog << "read " << wkt.size () << " byte WKT" << endl;
        }

        // Get min x, y, z
        double min_x = numeric_limits<double>::max ();
        double min_y = numeric_limits<double>::max ();
        double min_z = numeric_limits<double>::max ();
        for (auto &p : point_records)
        {
            min_x = std::min (min_x, p.x);
            min_y = std::min (min_y, p.y);
            min_z = std::min (min_z, p.z);
        }

        // Create the header
        LASheader lasheader;
        strcpy (lasheader.system_identifier, "SPOC by Zetamon XYZ");
        strcpy (lasheader.generating_software, "spoc2las");
        const auto now = std::chrono::system_clock::now ();
        time_t tt = std::chrono::system_clock::to_time_t(now);
        tm local_tm = *localtime(&tt);
        lasheader.file_creation_day = local_tm.tm_yday;
        lasheader.file_creation_year = local_tm.tm_year + 1900;
        lasheader.x_scale_factor = 1.0;
        lasheader.y_scale_factor = 1.0;
        lasheader.z_scale_factor = 1.0;
        lasheader.x_offset = min_x;
        lasheader.y_offset = min_y;
        lasheader.z_offset = min_z;
        lasheader.point_data_format = 2;
        lasheader.point_data_record_length = 26;

        if (!wkt.empty ())
            lasheader.set_geo_ogc_wkt (wkt.size (), wkt.data ());

        // Initialize the point
        LASpoint laspoint;
        laspoint.init(&lasheader, lasheader.point_data_format, lasheader.point_data_record_length, 0);

        // Write them to specified file
        if (args.verbose)
            clog << "writing records to " << args.output_fn << endl;

        spoc2las::las_writer l (args.output_fn, lasheader);

        for (size_t i = 0; i < point_records.size (); ++i)
        {
            const auto p = point_records[i];
            laspoint.set_X (p.x - min_x);
            laspoint.set_Y (p.y - min_y);
            laspoint.set_Z (p.z - min_z);
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
