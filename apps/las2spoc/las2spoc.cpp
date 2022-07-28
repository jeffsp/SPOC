#include "spoc/app_utils.h"
#include "spoc/spoc.h"
#include "las2spoc.h"
#include "las2spoc_cmd.h"
#include <iostream>
#include <stdexcept>
#include <string>

int main (int argc, char **argv)
{
    using namespace std;
    using namespace spoc::app_utils;
    using namespace spoc::file;
    using namespace spoc::header;
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

        // Get the wkt
        string wkt;

        // Check the coordinate system
        if (l.lasreader->header.vlr_geo_ogc_wkt == nullptr)
        {
            clog << "WARNING: The LAS file's spatial coordinate system is either missing, ";
            clog << "or it is not specified in OGC WKT format." << endl;
            clog << "This application can only process spatial coordinate systems if they are specified in OGC WKT format." << endl;
            clog << "To convert the LAS file's spatial coordinate system to OGC WKT format, you can use LAStools." << endl;
            clog << "For example, you can use the LAStools 'las2las' command:" << endl;
            clog << endl;
            clog << "    las2las -set_ogc_wkt -i input.las -o output.las" << endl;
            clog << endl;
            clog << "The output SPOC file's header will not contain a WKT string in the header." << endl;;
        }
        else
        {
            wkt = string (l.lasreader->header.vlr_geo_ogc_wkt);
        }

        // Get header fields
        const size_t extra_fields = 0;
        const size_t total_points = l.lasreader->npoints;
        const bool compressed = false;

        // Allocate the header
        header h (wkt, extra_fields, total_points, compressed);

        if (args.verbose)
        {
            clog << total_points << " total points" << endl;

            // Write them to specified file
            clog << "writing " << args.output_fn << endl;
        }

        // Get the output stream
        output_stream os (args.verbose, args.output_fn);

        // Write the header
        write_header (os (), h);

        // Process the points
        for (size_t i = 0; i < h.total_points; ++i)
        {
            if (!l.lasreader->read_point())
                throw runtime_error (string ("Error reading point record #") + to_string (i));

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

            // Write it out
            write_point_record (os (), p);
        }

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
