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

    // Get X, Y, and Z
    const auto x = get_x (p);
    const auto y = get_y (p);
    const auto z = get_z (p);

    // Get average X, Y, and (optionally) Z
    const double cx = std::accumulate (begin (x), end (x), 0.0) / n;
    const double cy = std::accumulate (begin (y), end (y), 0.0) / n;
    const double cz = z_flag ? std::accumulate (begin (z), end (z), 0.0) / n : 0.0;

    // Subtract off mean x, y, z
    for (size_t i = 0; i < n; ++i)
    {
        p[i].x -= cx;
        p[i].y -= cy;
        p[i].z -= cz;
    }

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

    // Get number of points
    const size_t n = h.total_points;

    // Read the data
    point_records p (n);
    for (size_t i = 0; i < p.size (); ++i)
        p[i] = read_point_record (is, h.extra_fields);

    switch (field_name[0])
    {
        // GCOV_EXCL_START
        default:
        {
            // This a logic error. If you get here, it means that
            // check_field_name() did not do its job.
            std::string s ("Unknown field name '");
            s += field_name;
            s += "'";
            throw std::runtime_error (s);
        }
        // GCOV_EXCL_STOP
        case 'x': throw std::runtime_error ("Cannot run the replace command on floating point fields (X, Y, Z)");
        case 'y': throw std::runtime_error ("Cannot run the replace command on floating point fields (X, Y, Z)");
        case 'z': throw std::runtime_error ("Cannot run the replace command on floating point fields (X, Y, Z)");
        case 'c': for (size_t i = 0; i < n; ++i) if (p[i].c == v1) p[i].c = v2; break;
        case 'p': for (size_t i = 0; i < n; ++i) if (p[i].p == v1) p[i].p = v2; break;
        case 'i': for (size_t i = 0; i < n; ++i) if (p[i].i == v1) p[i].i = v2; break;
        case 'r': for (size_t i = 0; i < n; ++i) if (p[i].r == v1) p[i].r = v2; break;
        case 'g': for (size_t i = 0; i < n; ++i) if (p[i].g == v1) p[i].g = v2; break;
        case 'b': for (size_t i = 0; i < n; ++i) if (p[i].b == v1) p[i].b = v2; break;
        case 'e': // extra
        {
            const size_t j = get_extra_index (field_name);
            for (size_t i = 0; i < n; ++i)
            {
                if (j >= p[i].extra.size ())
                    throw std::runtime_error ("Specified extra index is too large");
                if (p[i].extra[j] == v1)
                    p[i].extra[j] = v2;
            }
            break;
        }
    }

    // Write it back out
    write_header (os, h);
    for (size_t i = 0; i < n; ++i)
        write_point_record (os, p[i]);
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

    // Get number of points
    const size_t n = h.total_points;

    // Read the data
    point_records p (n);
    for (size_t i = 0; i < p.size (); ++i)
        p[i] = read_point_record (is, h.extra_fields);

    // Set the values
    switch (field_name[0])
    {
        // GCOV_EXCL_START
        default:
        {
            // This a logic error. If you get here, it means that
            // check_field_name() did not do its job.
            std::string s ("Unknown field name '");
            s += field_name;
            s += "'";
            throw std::runtime_error (s);
        }
        // GCOV_EXCL_STOP
        case 'x': for (size_t i = 0; i < n; ++i) p[i].x = v; break;
        case 'y': for (size_t i = 0; i < n; ++i) p[i].y = v; break;
        case 'z': for (size_t i = 0; i < n; ++i) p[i].z = v; break;
        case 'c': for (size_t i = 0; i < n; ++i) p[i].c = v; break;
        case 'p': for (size_t i = 0; i < n; ++i) p[i].p = v; break;
        case 'i': for (size_t i = 0; i < n; ++i) p[i].i = v; break;
        case 'r': for (size_t i = 0; i < n; ++i) p[i].r = v; break;
        case 'g': for (size_t i = 0; i < n; ++i) p[i].g = v; break;
        case 'b': for (size_t i = 0; i < n; ++i) p[i].b = v; break;
        case 'e': // extra
        {
            const size_t j = get_extra_index (field_name);
            for (size_t i = 0; i < n; ++i)
            {
                if (j >= p[i].extra.size ())
                    throw std::runtime_error ("Specified extra index is too large");
                p[i].extra[j] = v;
            }
            break;
        }
    }

    // Write it back out
    write_header (os, h);
    for (size_t i = 0; i < n; ++i)
        write_point_record (os, p[i]);
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

    // Get X, Y, and Z
    const auto x = get_x (p);
    const auto y = get_y (p);
    const auto z = get_z (p);

    // Get min X, Y, and (optionally) Z
    const double minx = *std::min_element (begin (x), end (x));
    const double miny = *std::min_element (begin (y), end (y));
    const double minz = z_flag ? *std::min_element (begin (z), end (z)) : 0.0;

    // Subtract off min x, y, z
    for (size_t i = 0; i < n; ++i)
    {
        p[i].x -= minx;
        p[i].y -= miny;
        p[i].z -= minz;
    }

    // Write it back out
    write_header (os, h);
    for (size_t i = 0; i < n; ++i)
        write_point_record (os, p[i]);
}

} // namespace transform

} // namespace spoc
