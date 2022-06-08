// This example is meant to be used as a source for a piping example,
// like 'stream_averaging'.
//
// Run the example like this:
//
//    $ generate_infinite_points | stream_averaging

#include "spoc.h"
#include <random>

int main (int argc, char **argv)
{
    using namespace std;
    using namespace spoc;

    try
    {
        // Create a header with effectively infinite points
        const string wkt = "Test WKT";
        const size_t extra_fields = 4;
        const size_t total_points = numeric_limits<size_t>::max ();
        const bool compressed = false;
        header h (wkt, extra_fields, total_points, compressed);

        // Write it
        write_header (cout, h);

        // RNG
        default_random_engine g;
        uniform_real_distribution<double> d (-500.0, 500.0);

        clog << "Generating points to stdout" << endl;
        clog << "Press CRTL-C to stop" << endl;

        // Infinite loop
        for/*ever*/ (;;)
        {
            // Create a point record with a random location
            point_record p (extra_fields);

            // Assign a random location
            p.x = d (g);
            p.y = d (g);
            p.z = d (g);

            // Write it
            write_point_record (cout, p);
        }

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
