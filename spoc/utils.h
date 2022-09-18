#pragma once
#include "contracts.h"
#include <functional>

namespace spoc
{

namespace utils
{

template <typename T>
inline size_t hash_value (size_t &seed, const T &v)
{
    return (seed << 6) + (seed >> 2) + 0x9e3779b9 + std::hash<T>{}(v);
}

template <typename T, typename... Args>
inline void hash_combine (size_t &seed, const T &v, const Args&... args)
{
    // These constants are from boost::hash_combine
    seed ^= spoc::utils::hash_value (seed, v);
    // Unary right fold
    //
    // For example
    //
    //     size_t s = 0;
    //     hash_combine(s,a,b,c)
    //
    // becomes
    //
    //     hash_combine(s,a), hash_combine(s,b,c)
    (hash_combine(seed, args), ...);
}

// Set X, Y, Z to nearest precision by truncating
template<typename T>
void quantize (T &p, const double precision)
{
    // Check preconditions
    REQUIRE (precision != 0.0);

    // Quantize x, y, z
#pragma omp parallel for
    for (size_t i = 0; i < p.size (); ++i)
    {
        // Quantize the point
        p[i].x = static_cast<int> (p[i].x / precision) * precision;
        p[i].y = static_cast<int> (p[i].y / precision) * precision;
        p[i].z = static_cast<int> (p[i].z / precision) * precision;
    }
}

template<typename T>
typename T::value_type get_center (const T &points)
{
    typename T::value_type c;
    for (const auto &p : points)
    {
        c.x += p.x;
        c.y += p.y;
        c.z += p.z;
    }
    c.x /= points.size ();
    c.y /= points.size ();
    c.z /= points.size ();
    return c;
}

template<typename T,typename U>
T crop (const T &points, const U &extent)
{
    T q;
    q.reserve (points.size ());
    for (const auto &p : points)
    {
        // If it does not lie within extent, ignore it
        if (
            p.x < extent.minp.x ||
            p.y < extent.minp.y ||
            p.z < extent.minp.z ||
            p.x > extent.maxp.x ||
            p.y > extent.maxp.y ||
            p.z > extent.maxp.z
           )
            continue;
        // Save it
        q.push_back (p);
    }
    return q;
}

} // namespace utils

} // namespace spoc
