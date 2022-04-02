#include "las2spoc_cmd.h"
#include "lasreader.hpp"
#include "spoc.h"
#include <iostream>
#include <stdexcept>
#include <string>

struct las
{
    las (const std::string &fn)
        : lasreader (nullptr)
    {
        lasreadopener.set_file_name (fn.c_str ());
        lasreader = lasreadopener.open ();
        if (lasreader == nullptr)
            throw std::runtime_error ("Could not open LASlib lasreader");
    }
    ~las ()
    {
        if (lasreader == nullptr)
            return;
        lasreader->close();
        delete lasreader;
    }
    LASreadOpener lasreadopener;
    LASreader *lasreader = lasreadopener.open();
};

int main (int argc, char **argv)
{
    using namespace std;
    using namespace spoc;

    try
    {
        // Parse command line
        cmd::args args = cmd::get_args (argc, argv,
                string (argv[0]) + " [options] < textfile > spocfile");

        // If you are getting help, exit without an error
        if (args.help)
            return 0;

        if (args.verbose)
            clog << "reading " << args.input_fn << endl;

        las l (args.input_fn);

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
            l.lasreader->point.set_point_source_ID(1020);
        }

        if (args.verbose)
            clog << point_records.size () << " point records read" << endl;

        // Write them to specified file
        if (args.verbose)
            clog << "writing records to " << args.output_fn << endl;

        ofstream ofs (args.output_fn);
        if (!ofs)
            throw runtime_error ("Could not open file for writing");

        //clog << "VLR_geo_ogc_wkt: " << l.lasreader->header.vlr_geo_ogc_wkt << endl;
        //const string wkt (l.lasreader->header.vlr_geo_ogc_wkt);
        string wkt ("WKT");
        write_spoc_file (ofs, point_records, wkt);

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}