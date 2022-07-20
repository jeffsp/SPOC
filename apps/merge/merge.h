#pragma once

#include "spoc.h"
#include <cassert>
#include <iostream>

namespace spoc
{

namespace merge_app
{

void append (
    const spoc::io::spoc_file &f1,
    spoc::io::spoc_file &f2,
    const unsigned id,
    const bool quiet,
    std::ostream &s = std::clog)
{
    // Check the SRS
    if (!quiet && (f1.get_header ().wkt != f2.get_header ().wkt))
        s << "WARNING: The spatial reference systems differ" << std::endl;

    // Save the old size
    const size_t f2_size = f2.get_header ().total_points;

    // Make room for new points
    f2.resize (f1.get_header ().total_points + f2.get_header ().total_points);

    // Append 'f1' to 'f2'
    for (size_t i = 0; i < f1.get_header ().total_points; ++i)
    {
        // Get the point from 'f1'
        auto p = f1.get_point_records ()[i];

        // Set its ID
        p.p = id;

        // Add it to 'b'
        f2.set (f2_size + i, p);
    }
}

} // namespace merge_app

} // namespace spoc
