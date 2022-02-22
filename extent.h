#pragma once

#include <limits>
#include <vector>
#include "point.h"

namespace spc
{

template<typename T>
struct extent
{
    point<T> minp;
    point<T> maxp;
};

template<typename T>
extent<T> get_extent (const std::vector<point<T>> &points)
{
    point<T> minp { std::numeric_limits<T>::max (),
        std::numeric_limits<T>::max (),
        std::numeric_limits<T>::max ()};
    point<T> maxp { std::numeric_limits<T>::lowest (),
        std::numeric_limits<T>::lowest (),
        std::numeric_limits<T>::lowest ()};
    for (const auto &p : points)
    {
        minp.x = std::min (p.x, minp.x);
        minp.y = std::min (p.y, minp.y);
        minp.z = std::min (p.z, minp.z);
        maxp.x = std::max (p.x, maxp.x);
        maxp.y = std::max (p.y, maxp.y);
        maxp.z = std::max (p.z, maxp.z);
    }
    return extent<T> {minp, maxp};
}

template<typename T>
extent<T> get_new_extent (const extent<T> &e,
    const point<double> &p,
    const point<double> &midp)
{
    auto f (e);
    if (p.x < midp.x) f.maxp.x = midp.x; else f.minp.x = midp.x;
    if (p.y < midp.y) f.maxp.y = midp.y; else f.minp.y = midp.y;
    if (p.z < midp.z) f.maxp.z = midp.z; else f.minp.z = midp.z;
    return f;
}

template<typename T>
extent<T> get_new_extent (const extent<T> &e,
    const unsigned octant_index,
    const point<double> &midp)
{
    auto f (e);
    if (octant_index & 0x01) f.maxp.x = midp.x; else f.minp.x = midp.x;
    if (octant_index & 0x02) f.maxp.y = midp.y; else f.minp.y = midp.y;
    if (octant_index & 0x04) f.maxp.z = midp.z; else f.minp.z = midp.z;
    return f;
}

// All dimensions <=
inline bool all_less_equal (const point<double> &a, const point<double> &b)
{
    if (a.x > b.x) return false;
    if (a.y > b.y) return false;
    if (a.z > b.z) return false;
    return true;
}

inline bool operator== (const extent<double> &a, const extent<double> &b)
{
    return (a.minp == b.minp) && (a.maxp == b.maxp);
}

}
