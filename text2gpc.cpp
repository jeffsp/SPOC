#include <iostream>
#include <sstream>
#include "gpc.h"

int main (const int argc, const char **argv)
{
    using namespace std;
    using namespace gpc;

    try
    {
        // Check args
        if (argc > 1)
            throw std::runtime_error ("Too many argements on command line");

        // Read SRS
        string wkt;
        getline (cin, wkt);

        // Read point records
        vector<point_record> point_records;

        for (string line; getline (cin, line ); )
        {
            // Parse the line
            stringstream ss (line);
            point_record p;
            ss >> p.x; ss >> p.y; ss >> p.z;
            ss >> p.c; ss >> p.i;
            ss >> p.r; ss >> p.g; ss >> p.b;
            point_records.push_back (p);
        }

        clog << point_records.size () << " point records read" << endl;

        // Write them to stdout
        write_gpc_file (cout, point_records, wkt);

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
