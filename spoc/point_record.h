#pragma once
#include "spoc/point.h"
#include "spoc/utils.h"
#include <algorithm>
#include <functional>

namespace spoc
{

namespace point_record
{

// Point record format
struct point_record
{
    double x;
    double y;
    double z;
    uint32_t c; // classification
    uint32_t p; // point ID
    uint16_t i; // intensity/NIR
    uint16_t r; // red
    uint16_t g; // green
    uint16_t b; // blue
    std::vector<uint64_t> extra;

    // CTORs
    point_record ()
        : x (0.0) , y (0.0) , z (0.0)
        , c (0) , p (0) , i (0)
        , r (0) , g (0) , b (0)
        , extra (0)
    {
    }
    point_record (const double x, const double y, const double z)
        : x (x), y (y), z (z)
        , c (0) , p (0) , i (0)
        , r (0) , g (0) , b (0)
        , extra (0)
    {
    }
    point_record (const double x, const double y, const double z,
        const uint32_t c)
        : x (x), y (y), z (z)
        , c (c) , p (0) , i (0)
        , r (0) , g (0) , b (0)
        , extra (0)
    {
    }
    explicit point_record (const size_t extra_fields)
        : x (0.0) , y (0.0) , z (0.0)
        , c (0) , p (0) , i (0)
        , r (0) , g (0) , b (0)
        , extra (extra_fields)
    {
    }
    point_record (const double x, const double y, const double z,
        const uint32_t c, const uint32_t p, const uint16_t i,
        const uint16_t r, const uint16_t g, const uint16_t b)
        : x (x), y (y), z (z)
        , c (c) , p (p) , i (i)
        , r (r) , g (g) , b (b)
        , extra (0)
    {
    }
    point_record (const double x, const double y, const double z,
        const uint32_t c, const uint32_t p, const uint16_t i,
        const uint16_t r, const uint16_t g, const uint16_t b,
        const std::vector<uint64_t> &extra)
        : x (x), y (y), z (z)
        , c (c) , p (p) , i (i)
        , r (r) , g (g) , b (b)
        , extra (extra)
    {
    }

    // Operators
    bool operator== (const point_record &other) const
    {
        return other.x == x
            && other.y == y
            && other.z == z
            && other.c == c
            && other.p == p
            && other.i == i
            && other.r == r
            && other.g == g
            && other.b == b
            && other.extra == extra
            ;
    }
    bool operator!= (const point_record &other) const
    {
        return !(*this == other);
    }
};

// Compute a hash value from a point record
struct point_record_hash
{
    std::size_t operator() (const spoc::point_record::point_record &p) const
    {
        // Combine X, Y, Z
        size_t h = 0;
        utils::hash_combine (h, p.x, p.y, p.z);
        // Combine C and P
        utils::hash_combine (h, p.c, p.p);
        // Combine I, R, G, B
        utils::hash_combine (h, p.i, p.r, p.g, p.b);
        // Combine extra
        for (auto i : p.extra)
            utils::hash_combine (h, i);

        return h;
    }
};

// Helper I/O function
inline std::ostream &operator<< (std::ostream &s, const point_record &p)
{
    s << '\t' << p.x << '\t' << p.y << '\t' << p.z;
    s << '\t' << p.c << '\t' << p.p << '\t' << p.i;
    s << '\t' << p.r << '\t' << p.g << '\t' << p.b;
    for (auto i : p.extra)
        s << '\t' << i;
    return s;
}

// Record I/O
inline void write_point_record (std::ostream &s, const point_record &p)
{
    s.write (reinterpret_cast<const char*>(&p.x), sizeof(double)
        + sizeof(double)
        + sizeof(double)
        + sizeof(uint32_t)
        + sizeof(uint32_t)
        + sizeof(uint16_t)
        + sizeof(uint16_t)
        + sizeof(uint16_t)
        + sizeof(uint16_t));
    s.write (reinterpret_cast<const char*>(&p.extra[0]), p.extra.size () * sizeof(uint64_t));
    s.flush ();
}

// Record I/O
inline point_record read_point_record (std::istream &s, const size_t extra_fields)
{
    point_record p;
    s.read (reinterpret_cast<char*>(&p.x), sizeof(double)
        + sizeof(double)
        + sizeof(double)
        + sizeof(uint32_t)
        + sizeof(uint32_t)
        + sizeof(uint16_t)
        + sizeof(uint16_t)
        + sizeof(uint16_t)
        + sizeof(uint16_t));
    p.extra.resize (extra_fields);
    s.read (reinterpret_cast<char*>(&p.extra[0]), p.extra.size () * sizeof(uint64_t));
    return p;
}

// Helper typedef
using point_records = std::vector<point_record>;

// Helper functions
std::vector<double> get_x (const point_records &p)
{
    std::vector<double> x (p.size ());
    for (size_t n = 0; n < p.size (); ++n)
        x[n] = p[n].x;
    return x;
}

std::vector<double> get_y (const point_records &p)
{
    std::vector<double> x (p.size ());
    for (size_t n = 0; n < p.size (); ++n)
        x[n] = p[n].y;
    return x;
}

std::vector<double> get_z (const point_records &p)
{
    std::vector<double> x (p.size ());
    for (size_t n = 0; n < p.size (); ++n)
        x[n] = p[n].z;
    return x;
}

std::vector<uint32_t> get_c (const point_records &p)
{
    std::vector<uint32_t> x (p.size ());
    for (size_t n = 0; n < p.size (); ++n)
        x[n] = p[n].c;
    return x;
}

std::vector<uint32_t> get_p (const point_records &p)
{
    std::vector<uint32_t> x (p.size ());
    for (size_t n = 0; n < p.size (); ++n)
        x[n] = p[n].p;
    return x;
}

std::vector<uint16_t> get_i (const point_records &p)
{
    std::vector<uint16_t> x (p.size ());
    for (size_t n = 0; n < p.size (); ++n)
        x[n] = p[n].i;
    return x;
}

std::vector<uint16_t> get_r (const point_records &p)
{
    std::vector<uint16_t> x (p.size ());
    for (size_t n = 0; n < p.size (); ++n)
        x[n] = p[n].r;
    return x;
}

std::vector<uint16_t> get_g (const point_records &p)
{
    std::vector<uint16_t> x (p.size ());
    for (size_t n = 0; n < p.size (); ++n)
        x[n] = p[n].g;
    return x;
}

std::vector<uint16_t> get_b (const point_records &p)
{
    std::vector<uint16_t> x (p.size ());
    for (size_t n = 0; n < p.size (); ++n)
        x[n] = p[n].b;
    return x;
}

size_t get_extra_fields_size (const point_records &p)
{
    if (p.empty ())
        return 0;
    return p[0].extra.size ();
}

std::vector<uint64_t> get_extra (const size_t k, const point_records &p)
{
    std::vector<uint64_t> x (p.size ());
    for (size_t n = 0; n < p.size (); ++n)
    {
        assert (k < p[n].extra.size ());
        x[n] = p[n].extra[k];
    }
    return x;
}

inline bool has_all_zero_c (const point_records &prs)
{
    return !std::any_of (std::begin (prs), std::end (prs),
        [] (const point_record &pr) { return pr.c != 0; });
}

inline bool has_all_zero_p (const point_records &prs)
{
    return !std::any_of (std::begin (prs), std::end (prs),
        [] (const point_record &pr) { return pr.p != 0; });
}

inline bool has_all_zero_i (const point_records &prs)
{
    return !std::any_of (std::begin (prs), std::end (prs),
        [] (const point_record &pr) { return pr.i != 0; });
}

inline bool has_all_zero_r (const point_records &prs)
{
    return !std::any_of (std::begin (prs), std::end (prs),
        [] (const point_record &pr) { return pr.r != 0; });
}

inline bool has_all_zero_g (const point_records &prs)
{
    return !std::any_of (std::begin (prs), std::end (prs),
        [] (const point_record &pr) { return pr.g != 0; });
}

inline bool has_all_zero_b (const point_records &prs)
{
    return !std::any_of (std::begin (prs), std::end (prs),
        [] (const point_record &pr) { return pr.b != 0; });
}

inline bool has_all_zero_rgb (const point_records &prs)
{
    return (has_all_zero_r (prs) && has_all_zero_g (prs) && has_all_zero_b (prs));
}

inline bool has_all_zero_extra (const size_t n, const point_records &prs)
{
    return !std::any_of (std::begin (prs), std::end (prs),
        [&] (const point_record &pr) { return pr.extra[n] != 0; });
}

} // namespace point_record

} // namespace spoc
