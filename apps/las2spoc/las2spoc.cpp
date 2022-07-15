#include "las2spoc.h"
#include "las2spoc_cmd.h"
#include "spoc.h"
#include <iostream>
#include <stdexcept>
#include <string>

int main (int argc, char **argv)
{
    using namespace std;
    using namespace spoc::io;
    using namespace spoc::las2spoc_app;
    using namespace spoc::las2spoc_cmd;
    using namespace spoc::point_record;

    try
    {
        // Parse command line
        const args args = get_args (argc, argv,
                string (argv[0]) + " [options] lasfile spocfile");

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

        las_reader l (args.input_fn);

        // Check the coordinate system
        if (l.lasreader->header.vlr_geo_ogc_wkt == nullptr)
        {
            clog << "WARNING: No spatial coordinate system was found." << endl;
            clog << "The coordinate system should be specified in OGC WKT format" << endl;
        }

        // Read points and put them into point records
        vector<point_record> point_records;

        while (l.lasreader->read_point())
        {
            point_record p;
            p.x = l.lasreader->point.get_x();
            p.y = l.lasreader->point.get_y();
            p.z = l.lasreader->point.get_z();
            p.c = l.lasreader->point.get_classification();
            p.p = l.lasreader->point.get_point_source_ID();
            p.i = l.lasreader->point.get_intensity();
            p.r = l.lasreader->point.rgb[0];
            p.g = l.lasreader->point.rgb[1];
            p.b = l.lasreader->point.rgb[2];
            point_records.push_back (p);
        }

        if (args.verbose)
        {
            clog << point_records.size () << " point records read" << endl;

            // Write them to specified file
            clog << "writing records to " << args.output_fn << endl;
        }

        ofstream ofs (args.output_fn);
        if (!ofs)
            throw runtime_error ("Could not open file for writing");

        string wkt;
        if (l.lasreader->header.vlr_geo_ogc_wkt != nullptr)
            wkt = string (l.lasreader->header.vlr_geo_ogc_wkt);

        write_spoc_file_uncompressed (ofs, spoc_file (wkt, point_records));

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
