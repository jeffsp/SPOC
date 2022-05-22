#include "transformer.h"

// Initialize the transformer
void init (const size_t npoints)
{
}

// Optionally, you can preprocess the points
void preprocess (const size_t index, const spoc::point_record &p)
{
    // Preprocessing is required if you need information about the point
    // cloud before you begin preprocessing. For example, if you wanted
    // to center a point cloud about the average X, Y, and Z values, you
    // would need to compute the average X, Y, and Z value during
    // preprocessing. You would then subtract these values from X, Y,
    // and Z during preocessing. Of course, the spoc tools already have
    // a transformer operation that recenters a point cloud, so you
    // should use that instead of writing your own...
}

// Process the point 'p' and return the result
spoc::point_record process (const size_t index, const spoc::point_record &p)
{
    // Copy the point
    auto q (p);

    // This No-Op transformer just copies the input to the output.
    //
    // Typically, you would change 'q' here in some way before
    // returning it, for example:
    //
    //   q.z += 10; // Add 10m to elevation
    //   q.c = (q.c == 3) ? 4 : q.c; // Change low veg to medium veg
    //   q.r = q.g = q.b = (q.r + q.g + q.b) / 3; // Convert to grayscale

    return q;
}

int main (int argc, char **argv)
{
    // Process argc and argv to get options and filenames
    //
    // If the input file specification is empty, read from stdin
    //
    // If the output file specification is empty, write to stdout
    bool verbose = false;
    const std::string input_filename = (argc < 2) ? "" : argv[1];
    const std::string output_filename = (argc < 3) ? "" : argv[2];
    return spoc::transformer::transformer_entry (verbose,
        input_filename,
        output_filename,
        init,
        preprocess,
        process);
}
