// This example reads a point cloud and writes it back out unchanged
//
// Run the example like this:
//
//    $ noop < input.spoc > output.spoc

#include "spoc.h"

int main (int argc, char **argv)
{
    using namespace std;
    using namespace spoc::io;
    using namespace spoc::point_record;

    try
    {
        // Read the input file
        spoc_file f;

        clog << "Reading SPOC file from stdin" << endl;

        // Read the header
        header h = read_header (cin);

        // Check compression flag
        if (h.compressed)
            throw std::runtime_error ("Expected an uncompressed file");

        clog << "Writing SPOC file to stdout" << endl;

        // Write the header
        write_header (cout, h);

        // Process the points
        for (size_t i = 0; i < h.total_points; ++i)
        {
            // Read a point
            const auto p = read_point_record (cin, h.extra_fields);

            // Do something to the point

            // Write it back out
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
