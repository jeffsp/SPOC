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

    // Append 'b' to 'a'
    // TODO
}

} // namespace merge

} // namespace spoc
