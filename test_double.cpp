#include "compress.h"
#include "spc.h"
#include "test_utils.h"
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <vector>
#include <stdexcept>

int main (int argc, char **argv)
{
    using namespace std;
    using namespace spc;

    try
    {
        if (argc != 1)
            throw runtime_error ("Usage: pgm < filename");

        clog << "Reading from stdin" << endl;

        // Read SRS
        string wkt;
        getline (cin, wkt);

        clog << "read " << wkt.size () << " byte WKT" << endl;

        // Read point records
        vector<point_record> point_records;

        for (string line; getline (cin, line); )
        {
            // Parse the line
            stringstream ss (line);
            point_record p;
            ss >> p.x; ss >> p.y; ss >> p.z;
            ss >> p.c; ss >> p.p; ss >> p.i;
            ss >> p.r; ss >> p.g; ss >> p.b;
            point_records.push_back (p);
        }

        clog << setprecision (12);
        clog << point_records.size () << " point records read" << endl;

        // Get vector of doubles
        vector<double> x (point_records.size ());
        for (size_t i = 0; i < x.size (); ++i)
            //x[i] = point_records[i].x;
            //x[i] = point_records[i].y;
            x[i] = point_records[i].z;

        clog << x.size () * 8 << " double bytes" << endl;

        {
        vector<uint8_t> xb (x.size () * 8);
        const uint8_t *p = reinterpret_cast<const uint8_t *> (&x[0]);
        std::copy (p, p + xb.size (), xb.begin ());
        const auto y = spc::compress (xb);
        const auto z = spc::decompress (y);
        verify (xb == z);

        clog << y.size () << " compressed double bytes" << endl;
        }

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
