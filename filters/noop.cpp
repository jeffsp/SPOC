#include "filter.h"


// Save the points here
std::vector<spoc::point_record> points;

// Initialize the filter
void init (const size_t npoints)
{
    points.resize (npoints);
}

// Preprocess the points
void preprocess (const size_t index, const spoc::point_record &p)
{
    // Save the point
    points.at (index) = p;
}

// Get the number of points in the filtered output
size_t get_npoints ()
{
    return points.size ();
}

// Get the filtered point
spoc::point_record get_point (const size_t index)
{
    return points.at (index);
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
    return spoc::filter::filter_entry (verbose,
        input_filename,
        output_filename,
        init,
        preprocess,
        get_npoints,
        get_point);
}
