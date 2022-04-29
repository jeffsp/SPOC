#pragma once

#include "spoc.h"
#include <cassert>
#include <iostream>
#include <limits>
#include <vector>
#include <unordered_map>

namespace spoc
{

namespace transform
{

spoc::spoc_file process (const spoc_file &f, std::ostream &ops, std::istream &ips)
{
    spoc::spoc_file g (f);

    // For each record
    for (size_t i = 0; i < f.get_npoints (); ++i)
    {
        // Get the record
        auto p = f.get (i);

        // Write it to the pipe
        ops << p << std::endl;

        // Read back the transformed record
        spoc::point_record q;
        ips >> q;

        // Set it in the new spoc_file
        g.set (i, q);
    }

    return g;
}

} // namespace transform

} // namespace spoc
