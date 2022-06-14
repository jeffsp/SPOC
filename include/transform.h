#pragma once

#include "spoc.h"
#include <cassert>
#include <iostream>
#include <limits>
#include <vector>
#include <unordered_map>

namespace spoc
{

namespace transform
{

// Set X, Y, Z to nearest precision by truncating
template<typename T>
void quantize (T &p, const double precision)
{
    // Quantize x, y, z
    for (size_t i = 0; i < p.size (); ++i)
    {
        // Quantize the point
        p[i].x = static_cast<int> (p[i].x / precision) * precision;
        p[i].y = static_cast<int> (p[i].y / precision) * precision;
        p[i].z = static_cast<int> (p[i].z / precision) * precision;
    }
}

// Set X, Y, Z to nearest precision by truncating
void quantize (std::istream &is,
    std::ostream &os,
    const spoc::header &h,
    const double precision)
{
    // Write the header
    write_header (os, h);

    // Process the points
    for (size_t i = 0; i < h.total_points; ++i)
    {
        // Read a point
        auto p = read_point_record (is, h.extra_fields);

        // Quantize the point
        p.x = static_cast<int> (p.x / precision) * precision;
        p.y = static_cast<int> (p.y / precision) * precision;
        p.z = static_cast<int> (p.z / precision) * precision;

        // Write it back out
        write_point_record (os, p);
    }
}

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

// Replace values in one field with values in another
void replace (std::istream &is,
    std::ostream &os,
    const spoc::header &h,
    const std::string &field_name,
    const double v1,
    const double v2)
{
    // Check the arguments
    if (!check_field_name (field_name))
        throw std::runtime_error (std::string ("Invalid field name: ")
            + field_name);

    // Check to make suze x, y, z, is not being used
    switch (field_name[0])
    {
        case 'x': throw std::runtime_error ("Cannot run the replace command on floating point fields (X, Y, Z)");
        case 'y': throw std::runtime_error ("Cannot run the replace command on floating point fields (X, Y, Z)");
        case 'z': throw std::runtime_error ("Cannot run the replace command on floating point fields (X, Y, Z)");
    }

    // Get the extra index
    const size_t j = get_extra_index (field_name);

    // Write the header
    write_header (os, h);

    // Process the points
    for (size_t i = 0; i < h.total_points; ++i)
    {
        // Read a point
        auto p = read_point_record (is, h.extra_fields);

        // Process the point
        switch (field_name[0])
        {
            case 'c': if (p.c == v1) p.c = v2; break;
            case 'p': if (p.p == v1) p.p = v2; break;
            case 'i': if (p.i == v1) p.i = v2; break;
            case 'r': if (p.r == v1) p.r = v2; break;
            case 'g': if (p.g == v1) p.g = v2; break;
            case 'b': if (p.b == v1) p.b = v2; break;
            case 'e': // extra
            {
                if (j >= p.extra.size ())
                    throw std::runtime_error ("Specified extra index is too large");
                if (p.extra[j] == v1)
                    p.extra[j] = v2;
            }
        }

        // Write it back out
        write_point_record (os, p);
    }
}

// Set field values
void set (std::istream &is,
    std::ostream &os,
    const spoc::header &h,
    const std::string &field_name,
    const double v)
{
    // Check the arguments
    if (!check_field_name (field_name))
        throw std::runtime_error (std::string ("Invalid field name: ")
            + field_name);

    // Get the extra index
    const size_t j = get_extra_index (field_name);

    // Write the header
    write_header (os, h);

    // Process the points
    for (size_t i = 0; i < h.total_points; ++i)
    {
        // Read a point
        auto p = read_point_record (is, h.extra_fields);

        // Process the point
        switch (field_name[0])
        {
            case 'x': p.x = v; break;
            case 'y': p.y = v; break;
            case 'z': p.z = v; break;
            case 'c': p.c = v; break;
            case 'p': p.p = v; break;
            case 'i': p.i = v; break;
            case 'r': p.r = v; break;
            case 'g': p.g = v; break;
            case 'b': p.b = v; break;
            case 'e': // extra
            {
                if (j >= p.extra.size ())
                    throw std::runtime_error ("Specified extra index is too large");
                p.extra[j] = v;
            }
        }

        // Write it back out
        write_point_record (os, p);
    }
}

// Subtract min x/y/z values from all values
template<typename T>
void subtract_min (T &p, const bool z_flag = false)
{
    // Get X, Y, and Z
    const auto x = get_x (p);
    const auto y = get_y (p);
    const auto z = get_z (p);

    // Get min X, Y, and (optionally) Z
    const double minx = *std::min_element (begin (x), end (x));
    const double miny = *std::min_element (begin (y), end (y));
    const double minz = z_flag ? *std::min_element (begin (z), end (z)) : 0.0;

    // Subtract off min x, y, z
    for (size_t i = 0; i < p.size (); ++i)
    {
        p[i].x -= minx;
        p[i].y -= miny;
        p[i].z -= minz;
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

} // namespace transform

} // namespace spoc
