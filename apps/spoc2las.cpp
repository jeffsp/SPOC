#include "las2spoc_cmd.h"
#include "laswriter.hpp"
#include "spoc.h"
#include <iostream>
#include <stdexcept>
#include <string>

struct las
{
    explicit las (const std::string &fn, const LASheader &lasheader)
        : laswriter (nullptr)
    {
        laswriteopener.set_file_name(fn.c_str ());
        laswriter = laswriteopener.open(&lasheader);
        if (laswriter == nullptr)
            throw std::runtime_error ("Could not open LASlib laswriter");
    }
    ~las ()
    {
        if (laswriter == nullptr)
            return;
        laswriter->close();
        delete laswriter;
    }
    LASwriteOpener laswriteopener;
    LASwriter *laswriter;
};

int main (int argc, char **argv)
{
    using namespace std;
    using namespace spoc;

    try
    {
        // Parse command line
        cmd::args args = cmd::get_args (argc, argv,
                string (argv[0]) + " [options] spocfile lasfile");

        // If you are getting help, exit without an error
        if (args.help)
            return 0;

        if (args.verbose)
            clog << "reading " << args.input_fn << endl;

        // Read spoc file
        vector<point_record> point_records;
        string wkt;

        if (args.verbose)
            clog << "reading spoc file" << endl;

        read_spoc_file (cin, point_records, wkt);

        if (args.verbose)
            clog << point_records.size () << " point records read" << endl;

        if (args.verbose)
            clog << "read " << wkt.size () << " byte WKT" << endl;

        // Get min x, y, z
        double min_x = std::numeric_limits<double>::max ();
        double min_y = std::numeric_limits<double>::max ();
        double min_z = std::numeric_limits<double>::max ();
        for (auto &p : point_records)
        {
            min_x = std::min (min_x, p.x);
            min_y = std::min (min_y, p.y);
            min_z = std::min (min_z, p.z);
        }

        // Create the header
        LASheader lasheader;
        lasheader.x_scale_factor = 1.0;
        lasheader.y_scale_factor = 1.0;
        lasheader.z_scale_factor = 1.0;
        lasheader.x_offset = min_x;
        lasheader.y_offset = min_y;
        lasheader.z_offset = min_z;
        lasheader.point_data_format = xxx;
        lasheader.point_data_record_length = xxx;

        // Initialize the point
        LASpoint laspoint;
        laspoint.init(&lasheader, lasheader.point_data_format, lasheader.point_data_record_length, 0);

        // Write them to specified file
        if (args.verbose)
            clog << "writing records to " << args.output_fn << endl;

        las l (args.output_fn, lasheader);

        for (size_t i = 0; i < point_records.size (); ++i)
        {
            const auto p = point_records[i];
            laspoint.set_x (p.x - min_x);
            laspoint.set_y (p.y - min_y);
            laspoint.set_z (p.z - min_z);
            laspoint.set_classification (p.c);
            laspoint.set_point_source_ID (p.p);
            laspoint.set_intensity (p.i);
            laspoint.set_red (p.r);
            laspoint.set_green (p.g);
            laspoint.set_blue (p.b);

            l.laswriter->write_point (&laspoint);
            l.laswriter->update_inventory (&laspoint);
        }

        // Done
        laswriter->update_header(&lasheader, true);

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
