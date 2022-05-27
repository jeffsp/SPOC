#pragma once

#include "spoc.h"
#include <cassert>
#include <iostream>

namespace spoc
{

namespace merge
{

void append (
    const spoc_file &f1,
    spoc_file &f2,
    const unsigned id,
    const bool quiet)
{
    // Check the SRS
    if (!quiet && (f1.h.wkt != f2.h.wkt))
        std::clog << "WARNING: The spatial reference systems differ" << std::endl;

    // Make room for new points
    f2.p.resize (f1.h.total_points + f2.h.total_points);

    // Append 'f1' to 'f2'
    for (size_t i = 0; i < f1.p.size (); ++i)
    {
        // Get the point from 'f1'
        auto p = f1.p[i];

        // Set its ID
        p.p = id;

        // Add it to 'b'
        f2.p[f2.h.total_points + i] = p;
    }

    // Update size
    f2.h.total_points = f2.p.size ();
}

} // namespace merge

} // namespace spoc
