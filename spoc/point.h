#pragma once

#include "utils.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <random>
#include <vector>

namespace spoc
{

namespace point
{

/// A point in 3D space
/// @tparam T point type
template<typename T>
struct point
{
    T x = 0;
    T y = 0;
    T z = 0;
};

/// Compute a hash value from a point struct
/// @tparam T point type
template<typename T>
struct point_hash
{
    std::size_t operator() (const point<T> &p) const
    {
        // Combine X, Y, Z
        size_t h = 0;
        utils::hash_combine (h, p.x, p.y, p.z);
        return h;
    }
};

/// Compare two points for equality
///
/// @tparam T point type
/// @param a First point
/// @param b Second point
template<typename T>
inline bool operator== (const point<T> &a, const point<T> &b)
{
    if (a.x != b.x) return false;
    if (a.y != b.y) return false;
    if (a.z != b.z) return false;
    return true;
}

/// Compare two points for inequality
///
/// @tparam T point type
/// @param a First point
/// @param b Second point
template<typename T>
inline bool operator!= (const point<T> &a, const point<T> &b)
{
    return !(a == b);
}

/// Subtract one point from another
///
/// @tparam T point type
/// @param a First point
/// @param b Second point
template<typename T>
inline point<T> operator-= (point<T> &a, const point<T> &b)
{
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;
    return a;
}

/// Subtract one point from another
///
/// @tparam T point type
/// @param a First point
/// @param b Second point
template<typename T>
inline point<T> operator- (const point<T> &a, const point<T> &b)
{
    auto c (a);
    return c -= b;
}

/// Add one point to another
///
/// @tparam T point type
/// @param a First point
/// @param b Second point
template<typename T>
inline point<T> operator+= (point<T> &a, const point<T> &b)
{
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
    return a;
}

/// Add one point to another
///
/// @tparam T point type
/// @param a First point
/// @param b Second point
template<typename T>
inline point<T> operator+ (const point<T> &a, const point<T> &b)
{
    auto c (a);
    return c += b;
}

/// Helper function for printing a point
///
/// @tparam T point type
/// @param s Output stream
/// @param p Point to print
template<typename T>
inline std::ostream &operator<< (std::ostream &s, const point<T> &p)
{
    s << ' ' << p.x;
    s << ' ' << p.y;
    s << ' ' << p.z;
    return s;
}

/// Point less than operator
///
/// @tparam T point type
/// @param a First point
/// @param b Second point
template<typename T>
inline bool operator< (const point<T> &a, const point<T> &b)
{
    if (a.x < b.x) return true;
    if (a.x > b.x) return false;
    if (a.y < b.y) return true;
    if (a.y > b.y) return false;
    if (a.z < b.z) return true;
    if (a.z > b.z) return false;
    return false;
}

inline void append_double (const double d, std::vector<uint8_t> &bytes)
{
    const uint8_t *p = reinterpret_cast<const uint8_t *> (&d);
    bytes.insert (bytes.end (), p, p + sizeof(double));
}

inline double bytes_to_double (const uint8_t *p)
{
    // cppcheck-suppress invalidPointerCast
    const double d = *(reinterpret_cast<const double *> (p));
    return d;
}

inline std::vector<uint8_t> encode_point (const point<double> &p)
{
    std::vector<uint8_t> bytes;
    append_double (p.x, bytes);
    append_double (p.y, bytes);
    append_double (p.z, bytes);
    return bytes;
}

inline point<double> decode_point (const std::vector<uint8_t> &bytes)
{
    point<double> p;
    p.x = bytes_to_double (&bytes[0] + 0 * sizeof(double));
    p.y = bytes_to_double (&bytes[0] + 1 * sizeof(double));
    p.z = bytes_to_double (&bytes[0] + 2 * sizeof(double));
    return p;
}

template<typename T>
inline std::vector<uint8_t> encode_points (const std::vector<point<T>> &points)
{
    // Output bytes
    std::vector<uint8_t> bytes;
    // Encode the points (NOT PORTABLE)
    const uint8_t *pd = reinterpret_cast<const uint8_t *> (&points[0]);
    bytes.insert (bytes.end (), pd, pd + points.size () * sizeof(point<T>));
    return bytes;
}

template<typename T>
inline std::vector<point<T>> decode_points (const std::vector<uint8_t> &bytes)
{
    // Check logic
    assert (bytes.size () % sizeof(point<T>) == 0);

    // Decoded points
    std::vector<point<T>> points;
    // Decode the points (NOT PORTABLE)
    const point<T> *pc = reinterpret_cast<const point<T> *> (&bytes[0]);
    const size_t total_points = bytes.size () / sizeof(point<T>);
    points.insert (points.end (), pc, pc + total_points);
    return points;
}

/// double equality test with precision
///
/// @param a First value
/// @param b Second value
/// @param precision Precision
inline bool about_equal (const double &a,
    const double &b,
    const unsigned precision)
{
    const auto d = a - b;
    if (std::round (d * std::pow (10, precision)) != 0)
        return false;
    return true;
}

/// Point equality test with precision
///
/// @param a First point
/// @param b Second point
/// @param precision Precision
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

/// Vector of points equality test with precision
///
/// @param a First vector
/// @param b Second vector
/// @param precision Precision
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

} // namespace point

} // namespace spoc
