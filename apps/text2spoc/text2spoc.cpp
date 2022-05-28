#include <iostream>
#include <sstream>
#include "spoc.h"
#include "text2spoc_cmd.h"

int main (int argc, char **argv)
{
    using namespace std;
    using namespace spoc;

    try
    {
        // Parse command line
        const cmd::args args = cmd::get_args (argc, argv,
            string (argv[0]) + " [options] < textfile > spocfile");

        // If you are getting help, exit without an error
        if (args.help)
            return 0;

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

        // Write them to stdout
        if (args.verbose)
            clog << "writing records to stdout" << endl;

        write_spoc_file (cout, spoc_file (wkt, point_records));

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
