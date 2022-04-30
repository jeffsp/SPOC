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

spoc::spoc_file process (const spoc_file &f, std::ostream &ips, std::istream &ops)
{
    spoc::spoc_file g (f);

    // Write a header
    write_binary_header (ips,
        f.get_major_version (),
        f.get_minor_version (),
        f.get_npoints ());

    // For each record
    for (size_t i = 0; i < f.get_npoints (); ++i)
    {
        // Get the record
        auto p = f.get (i);

        // Write the record to the filter's input pipe
        spoc::write_binary (ips, p);

        // Read the transformed record from the filter's output pipe
        const spoc::point_record q = spoc::read_binary (ops);

        // Set it in the new spoc_file
        g.set (i, q);
    }

    return g;
}

} // namespace transform

} // namespace spoc
