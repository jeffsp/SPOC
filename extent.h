#pragma once

#include <cassert>
#include <limits>
#include <utility>
#include <vector>
#include "point.h"

namespace spoc
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

using extent_pair = std::pair<std::vector<uint8_t>,std::vector<uint8_t>>;

inline extent_pair encode_extent (const spoc::extent<double> &e)
{
    extent_pair bytes;
    bytes.first = encode_point (e.minp);
    bytes.second = encode_point (e.maxp);
    return bytes;
}

inline spoc::extent<double> decode_extent (const extent_pair &bytes)
{
    spoc::extent<double> e;
    e.minp = decode_point (bytes.first);
    e.maxp = decode_point (bytes.second);
    return e;
}

constexpr double max_value = (1ul << 62);

inline point<uint64_t> rescale (const point<double> &p,
    const point<double> &s,
    const point<double> &minp)
{
    point<uint64_t> q;
    q.x = (p.x - minp.x) * max_value / s.x;
    q.y = (p.y - minp.y) * max_value / s.y;
    q.z = (p.z - minp.z) * max_value / s.z;
    return q;
}

inline point<double> restore (const point<uint64_t> &p,
    const point<double> &s,
    const point<double> &minp)
{
    point<double> q;
    q.x = p.x * s.x / max_value + minp.x;
    q.y = p.y * s.y / max_value + minp.y;
    q.z = p.z * s.z / max_value + minp.z;
    return q;
}

inline std::vector<point<uint64_t>> rescale (
    const std::vector<point<double>> p,
    const extent<double> &e)
{
    assert (all_less_equal (e.minp, e.maxp));
    const point<double> s = e.maxp - e.minp;
    std::vector<point<uint64_t>> q (p.size ());
    for (size_t i = 0; i < p.size (); ++i)
        q[i] = rescale (p[i], s, e.minp);
    return q;
}

inline std::vector<point<double>> restore (
    const std::vector<point<uint64_t>> p,
    const spoc::extent<double> &e)
{
    assert (all_less_equal (e.minp, e.maxp));
    const point<double> s = e.maxp - e.minp;
    std::vector<point<double>> q (p.size ());
    for (size_t i = 0; i < p.size (); ++i)
        q[i] = restore (p[i], s, e.minp);
    return q;
}

}
