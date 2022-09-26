#pragma once

#include <cassert>
#include <limits>
#include <utility>
#include <vector>
#include "spoc/point.h"

namespace spoc
{

namespace extent
{

struct extent
{
    spoc::point::point<double> minp;
    spoc::point::point<double> maxp;
};

double get_volume (const extent &e)
{
    return (e.maxp.x - e.minp.x)
        * (e.maxp.y - e.minp.y)
        * (e.maxp.z - e.minp.z);
}

double get_area (const extent &e)
{
    return (e.maxp.x - e.minp.x)
        * (e.maxp.y - e.minp.y);
}

template<typename T>
extent get_extent (const T &points)
{
    spoc::point::point<double> minp { std::numeric_limits<double>::max (),
        std::numeric_limits<double>::max (),
        std::numeric_limits<double>::max ()};
    spoc::point::point<double> maxp { std::numeric_limits<double>::lowest (),
        std::numeric_limits<double>::lowest (),
        std::numeric_limits<double>::lowest ()};
    for (const auto &p : points)
    {
        minp.x = std::min (p.x, minp.x);
        minp.y = std::min (p.y, minp.y);
        minp.z = std::min (p.z, minp.z);
        maxp.x = std::max (p.x, maxp.x);
        maxp.y = std::max (p.y, maxp.y);
        maxp.z = std::max (p.z, maxp.z);
    }
    return extent {minp, maxp};
}

// All dimensions <=
inline bool all_less_equal (const spoc::point::point<double> &a, const spoc::point::point<double> &b)
{
    if (a.x > b.x) return false;
    if (a.y > b.y) return false;
    if (a.z > b.z) return false;
    return true;
}

inline bool operator== (const extent &a, const extent &b)
{
    return (a.minp == b.minp) && (a.maxp == b.maxp);
}

using extent_pair = std::pair<std::vector<uint8_t>,std::vector<uint8_t>>;

inline extent_pair encode_extent (const spoc::extent::extent &e)
{
    extent_pair bytes;
    bytes.first = encode_point (e.minp);
    bytes.second = encode_point (e.maxp);
    return bytes;
}

inline spoc::extent::extent decode_extent (const extent_pair &bytes)
{
    spoc::extent::extent e;
    e.minp = spoc::point::decode_point (bytes.first);
    e.maxp = spoc::point::decode_point (bytes.second);
    return e;
}

constexpr double max_value = (1ul << 62);

inline spoc::point::point<uint64_t> rescale (const spoc::point::point<double> &p,
    const spoc::point::point<double> &s,
    const spoc::point::point<double> &minp)
{
    spoc::point::point<uint64_t> q;
    q.x = (p.x - minp.x) * max_value / s.x;
    q.y = (p.y - minp.y) * max_value / s.y;
    q.z = (p.z - minp.z) * max_value / s.z;
    return q;
}

inline spoc::point::point<double> restore (const spoc::point::point<uint64_t> &p,
    const spoc::point::point<double> &s,
    const spoc::point::point<double> &minp)
{
    spoc::point::point<double> q;
    q.x = p.x * s.x / max_value + minp.x;
    q.y = p.y * s.y / max_value + minp.y;
    q.z = p.z * s.z / max_value + minp.z;
    return q;
}

inline std::vector<spoc::point::point<uint64_t>> rescale (
    const std::vector<spoc::point::point<double>> &p,
    const extent &e)
{
    assert (all_less_equal (e.minp, e.maxp));
    const spoc::point::point<double> s = e.maxp - e.minp;
    std::vector<spoc::point::point<uint64_t>> q (p.size ());
    for (size_t i = 0; i < p.size (); ++i)
        q[i] = rescale (p[i], s, e.minp);
    return q;
}

inline std::vector<spoc::point::point<double>> restore (
    const std::vector<spoc::point::point<uint64_t>> &p,
    const spoc::extent::extent &e)
{
    assert (all_less_equal (e.minp, e.maxp));
    const spoc::point::point<double> s = e.maxp - e.minp;
    std::vector<spoc::point::point<double>> q (p.size ());
    for (size_t i = 0; i < p.size (); ++i)
        q[i] = restore (p[i], s, e.minp);
    return q;
}

} // namespace extent

} // namespace spoc
