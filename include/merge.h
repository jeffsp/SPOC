#pragma once

#include "spoc.h"
#include <cassert>
#include <iostream>

namespace spoc
{

namespace merge
{

void append (const spoc_file &a, spoc_file &b, const unsigned id, const bool quiet)
{
    // Check the SRS
    if (!quiet && (a.get_wkt () != b.get_wkt ()))
        std::clog << "WARNING: The spatial reference systems differ" << std::endl;

    // Get current number of points
    const size_t npoints = b.get_npoints ();

    // Make room for new points
    b.resize (npoints + a.get_npoints ());

    // Append 'a' to 'b'
    for (size_t i = 0; i < a.get_npoints (); ++i)
    {
        // Get the point from 'a'
        auto p = a.get (i);

        // Set its ID
        p.p = id;

        // Add it to 'b'
        b.set (npoints + i, p);
    }
}

} // namespace merge

} // namespace spoc
