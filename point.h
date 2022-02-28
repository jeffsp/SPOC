#pragma once

#include <cassert>
#include <iostream>
#include <vector>

namespace spc
{

// A point in 3D space
template<typename T>
struct point
{
    T x = 0;
    T y = 0;
    T z = 0;
};

// Point operators

inline bool operator== (const point<double> &a, const point<double> &b)
{
    if (a.x != b.x) return false;
    if (a.y != b.y) return false;
    if (a.z != b.z) return false;
    return true;
}

inline bool operator!= (const point<double> &a, const point<double> &b)
{
    return !(a == b);
}

inline point<double> operator-= (point<double> &a, const point<double> &b)
{
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;
    return a;
}

inline point<double> operator- (const point<double> &a, const point<double> &b)
{
    auto c (a);
    return c -= b;
}

inline point<double> operator+= (point<double> &a, const point<double> &b)
{
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
    return a;
}

inline point<double> operator+ (const point<double> &a, const point<double> &b)
{
    auto c (a);
    return c += b;
}

inline std::ostream &operator<< (std::ostream &s, const point<double> &p)
{
    s << ' ' << p.x;
    s << ' ' << p.y;
    s << ' ' << p.z;
    return s;
}

inline bool operator< (const point<double> &a, const point<double> &b)
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

inline std::vector<uint8_t> encode_points (const std::vector<point<double>> &points)
{
    // Output bytes
    std::vector<uint8_t> bytes;
    // Encode the points (NOT PORTABLE)
    const uint8_t *pd = reinterpret_cast<const uint8_t *> (&points[0]);
    bytes.insert (bytes.end (), pd, pd + points.size () * sizeof(point<double>));
    return bytes;
}

inline std::vector<point<double>> decode_points (const std::vector<uint8_t> &bytes)
{
    // Check logic
    assert (bytes.size () % sizeof(point<double>) == 0);

    // Decoded points
    std::vector<point<double>> points;
    // Decode the points (NOT PORTABLE)
    const point<double> *pc = reinterpret_cast<const point<double> *> (&bytes[0]);
    const size_t total_points = bytes.size () / sizeof(point<double>);
    points.insert (points.end (), pc, pc + total_points);
    return points;
}

}
