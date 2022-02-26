#pragma once

#include <cassert>
#include <cmath>
#include <sstream>
#include <vector>
#include "point.h"

inline void Verify (const char *e, const char *file, const unsigned line)
{
    std::stringstream s;
    s << "verification failed in " << file << ", line " << line << ": " << e;
    throw std::runtime_error (s.str ());
}

#define verify(e) (void)((e) || (Verify (#e, __FILE__, __LINE__), 0))

namespace spc
{

inline bool about_equal (const double &a,
    const double &b,
    const unsigned precision)
{
    const auto d = a - b;
    if (std::round (d * std::pow (10, precision)) != 0)
        return false;
    return true;
}

inline bool about_equal (const point<double> &a,
    const point<double> &b,
    const unsigned precision)
{
    if (!about_equal (a.x, b.x, precision))
        return false;
    if (!about_equal (a.y, b.y, precision))
        return false;
    if (!about_equal (a.z, b.z, precision))
        return false;
    return true;
}

inline bool about_equal (const std::vector<point<double>> &a,
    const std::vector<point<double>> &b,
    unsigned precision = 12)
{
    assert (a.size () == b.size ());
    for (size_t i = 0; i < a.size (); ++i)
        if (!about_equal (a[i], b[i], precision))
            return false;
    return true;
}

}
