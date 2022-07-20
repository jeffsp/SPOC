#pragma once
#include "point.h"
#include <algorithm>

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

    point_record ()
        : x (0.0) , y (0.0) , z (0.0)
        , c (0) , p (0) , i (0)
        , r (0) , g (0) , b (0)
        , extra (0)
    {
    }
    explicit point_record (const double x, const double y, const double z)
        : x (x), y (y), z (z)
        , c (0) , p (0) , i (0)
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
    explicit point_record (const double x, const double y, const double z,
        const uint32_t c, const uint32_t p, const uint16_t i,
        const uint16_t r, const uint16_t g, const uint16_t b)
        : x (x), y (y), z (z)
        , c (c) , p (p) , i (i)
        , r (r) , g (g) , b (b)
        , extra (0)
    {
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

// Helper operator
inline bool operator== (const point_record &a, const point_record &b)
{
    if (a.x != b.x) return false;
    if (a.y != b.y) return false;
    if (a.z != b.z) return false;
    if (a.c != b.c) return false;
    if (a.p != b.p) return false;
    if (a.i != b.i) return false;
    if (a.r != b.r) return false;
    if (a.g != b.g) return false;
    if (a.b != b.b) return false;
    if (a.extra != b.extra) return false;
    return true;
}

// Helper operator
inline bool operator!= (const point_record &a, const point_record &b)
{
    return !(a == b);
}

// Helper typedefs
using point_records = std::vector<point_record>;

// Helper function
bool check_records (const point_records &prs)
{
    // Degenerate case
    if (prs.empty ())
        return true;

    // Make sure they all have the same number of extra fields
    const size_t extra_fields = prs[0].extra.size ();

    if (std::any_of (prs.cbegin(), prs.cend(),
        [&](const point_record &p)
        { return p.extra.size () != extra_fields; }))
        return false;

    return true;
}


} // namespace io

} // namespace spoc
