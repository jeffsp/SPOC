#include <iostream>
#include <sstream>
#include "spc.h"
#include "text2spc_cmd.h"

int main (int argc, char **argv)
{
    using namespace std;
    using namespace spc;

    try
    {
        // Parse command line
        cmd::args args = cmd::get_args (argc, argv,
            string (argv[0]) + " [options] < textfile > spcfile");

        // If you are getting help, exit without an error
        if (args.help)
            return 0;

        // Show the args
        if (args.verbose)
        {
            clog << boolalpha;
            clog << "sort = " << args.sort << endl;
            if (args.sort)
                clog << "sort_grid_resolution = " << args.sort_grid_resolution << "m" << endl;
        }

        // Read SRS
        string wkt;
        getline (cin, wkt);

        if (args.verbose)
            clog << "read " << wkt.size () << " byte WKT" << endl;

        if (args.verbose)
            clog << "reading point records" << endl;

        // Read point records
        vector<point_record> point_records;

        for (string line; getline (cin, line ); )
        {
            // Parse the line
            stringstream ss (line);
            point_record p;
            ss >> p.x; ss >> p.y; ss >> p.z;
            ss >> p.c; ss >> p.p; ss >> p.i;
            ss >> p.r; ss >> p.g; ss >> p.b;
            point_records.push_back (p);
        }

        if (args.verbose)
            clog << point_records.size () << " point records read" << endl;

        // Sort, if specified
        if (args.sort)
        {
            if (args.verbose)
                clog << "sorting records..." << endl;

            sort (point_records, args.sort_grid_resolution);
        }

        // Write them to stdout
        if (args.verbose)
            clog << "writing records to stdout" << endl;

        write_spc_file (cout, point_records, wkt);

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
