#include "spoc/app_utils.h"
#include "spoc/spoc.h"
#include "text2spoc.h"
#include "text2spoc_cmd.h"
#include <iostream>
#include <sstream>

int main (int argc, char **argv)
{
    using namespace std;
    using namespace spoc;
    using namespace spoc::file;
    using namespace spoc::io;
    using namespace spoc::point_record;
    using namespace spoc::text2spoc_cmd;

    try
    {
        // Parse command line
        const args args = get_args (argc, argv,
            string (argv[0]) + " [options] < textfile > spocfile");

        // If version was requested, print it and exit
        if (args.version)
        {
            cout << "Version "
                << static_cast<int> (MAJOR_VERSION)
                << "."
                << static_cast<int> (MINOR_VERSION)
                << endl;
            return 0;
        }

        // If you are getting help, exit without an error
        if (args.help)
            return 0;

        // Read SRS
        string wkt;
        getline (cin, wkt);

        if (args.verbose)
        {
            clog << "read " << wkt.size () << " byte WKT" << endl;
            clog << "Reading point records from stdin" << endl;
        }

        // Read point records
        vector<point_record::point_record> point_records;

        for (string line; getline (cin, line ); )
        {
            // Parse the line
            stringstream ss (line);
            point_record::point_record p;
            ss >> p.x; ss >> p.y; ss >> p.z;
            ss >> p.c; ss >> p.p; ss >> p.i;
            ss >> p.r; ss >> p.g; ss >> p.b;
            point_records.push_back (p);
        }

        if (args.verbose)
        {
            clog << point_records.size () << " point records read" << endl;
            clog << "Writing records to stdout" << endl;
        }

        write_spoc_file_uncompressed (cout, spoc_file (wkt, point_records));

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
