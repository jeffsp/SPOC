#pragma once

#include "spoc.h"
#include <cassert>
#include <iostream>

namespace spoc
{

namespace util
{

// Recenter a point cloud about its mean
template<typename T>
void recenter (T &p, const bool z_flag = false)
{
    // Get X, Y, and Z
    const auto x = get_x (p);
    const auto y = get_y (p);
    const auto z = get_z (p);

    // Get average X, Y, and (optionally) Z
    const double cx = std::accumulate (begin (x), end (x), 0.0) / p.size ();
    const double cy = std::accumulate (begin (y), end (y), 0.0) / p.size ();
    const double cz = z_flag ? std::accumulate (begin (z), end (z), 0.0) / p.size () : 0.0;

    // Subtract off mean x, y, z
#pragma omp parallel for
    for (size_t i = 0; i < p.size (); ++i)
    {
        p[i].x -= cx;
        p[i].y -= cy;
        p[i].z -= cz;
    }
}

// Recenter a point cloud about its mean
void recenter (std::istream &is,
    std::ostream &os,
    const spoc::header &h,
    const bool z_flag = false)
{
    // Get number of points
    const auto n = h.total_points;

    // Read the data
    point_records p (n);
    for (size_t i = 0; i < p.size (); ++i)
        p[i] = read_point_record (is, h.extra_fields);

    recenter (p, z_flag);

    // Write it back out
    write_header (os, h);
    for (size_t i = 0; i < n; ++i)
        write_point_record (os, p[i]);
}

// Get min x/y/z values from all values
template<typename T>
spoc::point<double> get_min (const T &p)
{
    // Get X, Y, and Z
    const auto x = get_x (p);
    const auto y = get_y (p);
    const auto z = get_z (p);

    // Get min X, Y, and (optionally) Z
    const double minx = *std::min_element (begin (x), end (x));
    const double miny = *std::min_element (begin (y), end (y));
    const double minz = *std::min_element (begin (z), end (z));

    return spoc::point<double> { minx, miny, minz };
}

// Subtract min x/y/z values from all values
template<typename T>
void subtract_min (T &p, const bool z_flag = true)
{
    // Get minimum value
    const auto minp = get_min (p);

    // Subtract off min x, y, z
#pragma omp parallel for
    for (size_t i = 0; i < p.size (); ++i)
    {
        p[i].x -= minp.x;
        p[i].y -= minp.y;
        if (z_flag)
            p[i].z -= minp.z;
    }
}

// Subtract min x/y/z values from all values
void subtract_min (std::istream &is,
    std::ostream &os,
    const spoc::header &h,
    const bool z_flag = false)
{
    // Get number of points
    const auto n = h.total_points;

    // Read the data
    point_records p (n);
    for (size_t i = 0; i < p.size (); ++i)
        p[i] = read_point_record (is, h.extra_fields);

    subtract_min (p, z_flag);

    // Write it back out
    write_header (os, h);
    for (size_t i = 0; i < n; ++i)
        write_point_record (os, p[i]);
}

} // namespace util

} // namespace spoc
