#include "app_utils.h"
#include "las2spoc.h"
#include "las2spoc_cmd.h"
#include "spoc.h"
#include "spoc_conversion.h"
#include <iostream>
#include <stdexcept>
#include <string>

const std::string OGC_WKT_WARNING =
"WARNING: The LAS file's spatial coordinate system is either missing,\n"
"or it is not specified in OGC WKT format. This application can only\n"
"process spatial coordinate systems if they are specified in OGC WKT\n"
"format. To convert the LAS file's spatial coordinate system to OGC\n"
"WKT format, you can use LAStools.\n"
"\n"
"For example, you can use the LAStools 'las2las' command:\n"
"\n"
"    las2las -set_ogc_wkt -i input.las -o output.las\n"
"\n";

int main (int argc, char **argv)
{
    using namespace std;
    using namespace spoc::app_utils;
    using namespace spoc::header;
    using namespace spoc::las2spoc_cmd;
    using namespace spoc::point_record;
    using namespace spoc_conversion;

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
            if (!args.nowarn)
                clog << OGC_WKT_WARNING;
        }
        else
            wkt = string (l.lasreader->header.vlr_geo_ogc_wkt);

        // Allocate the header
        const bool compressed = false;
        const uint8_t extra_fields = 0;
        header h (wkt, extra_fields, l.lasreader->npoints, compressed);

        if (args.verbose)
        {
            clog << h.total_points << " total points" << endl;

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
            // Note that based on my understanding of the extended_classification field in the laspoint object,
            // extended_classification will always be the same as classification if classification is <= 31.
            // Otherwise, classification will be 0, and extended_classification will be > 31. However, this
            // seems to only be true if the is_extended_point_type function returns true, otherwise the
            // extended_classification is always zero.
            if (l.lasreader->point.is_extended_point_type())
                p.c = l.lasreader->point.get_extended_classification();
            else
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
