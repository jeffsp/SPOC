// This example is meant to be used as a sink for a piping example.
//
// Run the example like this:
//
//    $ generate_infinite_points | stream_averaging

#include "spoc.h"
#include <chrono>
#include <locale>

int main (int argc, char **argv)
{
    using namespace std;
    using namespace spoc::io;

    try
    {
        // Start the timer
        auto start = chrono::steady_clock::now ();

        // Read data from anonymous pipe
        clog << "Reading SPOC file from stdin" << endl;

        // Read the header
        header h = read_header (cin);

        // Check compression flag
        if (h.compressed)
            throw std::runtime_error ("Expected an uncompressed file");

        // Save stats
        size_t total_points = 0;
        double sumx = 0.0;
        double sumy = 0.0;
        double sumz = 0.0;

        // Add comma separator to output
        clog.imbue (locale(""));

        // Print the column header
        clog << "Total\tavg_x\tavg_y\tavg_z" << endl;

        // Process the points
        for (size_t i = 0; i < h.total_points; ++i)
        {
            // Read a point
            const auto p = read_point_record (cin, h.extra_fields);

            // Update stats
            ++total_points;
            sumx += p.x;
            sumy += p.y;
            sumz += p.z;

            // Check the time
            const auto end = chrono::steady_clock::now ();
            const auto elapsed = chrono::duration_cast<chrono::seconds> (end - start).count ();

            // Update every second or so
            if (elapsed > 1)
            {
                clog << total_points << "\t"
                    << sumx / total_points << "\t"
                    << sumy / total_points << "\t"
                    << sumz / total_points
                    << endl;
                // Restart timer
                start = chrono::steady_clock::now ();
            }

        }

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
