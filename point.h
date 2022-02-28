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

inline std::vector<spc::point<double>> generate_points (
    const size_t N,
    const int min_exponent = std::numeric_limits<double>::min_exponent / 2,
    const int max_exponent = std::numeric_limits<double>::max_exponent / 2)
{
    using namespace std;
    using namespace spc;

    vector<point<double>> points (N);

    // Generate random doubles at varying scales.
    //
    // We don't want a uniform distribution.
    std::default_random_engine g;
    std::uniform_real_distribution<double> d (-1.0, 1.0);
    std::uniform_int_distribution<int> exp (min_exponent, max_exponent);

    for (auto &p : points)
    {
        const double x = std::ldexp (d (g), exp (g));
        const double y = std::ldexp (d (g), exp (g));
        const double z = std::ldexp (d (g), exp (g));
        p = point<double> {x, y, z};
    }

    return points;
}

}
