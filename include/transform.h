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

spoc_file set (const spoc_file &f,
    const char field_name,
    const double v)
{
    // Get number of points
    const size_t n = f.get_header ().total_points;

    // Return value
    spoc_file g (f);

    // Set the values
    switch (field_name)
    {
        default:
        {
            std::string s ("Unknown field name '");
            s += field_name;
            s += "'";
            throw std::runtime_error (s);
        }
        case 'x': for (size_t i = 0; i < n; ++i) g[i].x = v; break;
        case 'y': for (size_t i = 0; i < n; ++i) g[i].y = v; break;
        case 'z': for (size_t i = 0; i < n; ++i) g[i].z = v; break;
        case 'c': for (size_t i = 0; i < n; ++i) g[i].c = v; break;
        case 'p': for (size_t i = 0; i < n; ++i) g[i].p = v; break;
        case 'i': for (size_t i = 0; i < n; ++i) g[i].i = v; break;
        case 'r': for (size_t i = 0; i < n; ++i) g[i].r = v; break;
        case 'g': for (size_t i = 0; i < n; ++i) g[i].g = v; break;
        case 'b': for (size_t i = 0; i < n; ++i) g[i].b = v; break;
        case '0': case '1': case '2': case '3':
        case '4': case '5': case '6': case '7':
        case '8': case '9':
        {
              const size_t j = field_name - '0';
              for (size_t i = 0; i < n; ++i)
              {
                  if (j >= g[i].extra.size ())
                      throw std::runtime_error ("Specified extra index is too large");
                  g[i].extra[j] = v;
              }
              break;
        }
    }

    return g;
}

spoc_file replace (const spoc_file &f,
    const char field_name,
    const double v1,
    const double v2)
{
    // Get number of points
    const size_t n = f.get_header ().total_points;

    // Return value
    spoc_file g (f);

    switch (field_name)
    {
        default:
        {
            std::string s ("Unknown field name '");
            s += field_name;
            s += "'";
            throw std::runtime_error (s);
        }
        case 'x': throw std::runtime_error ("Cannot run the replace command on floating point fields (X, Y, Z)");
        case 'y': throw std::runtime_error ("Cannot run the replace command on floating point fields (X, Y, Z)");
        case 'z': throw std::runtime_error ("Cannot run the replace command on floating point fields (X, Y, Z)");
        case 'c': for (size_t i = 0; i < n; ++i) if (g[i].c == v1) g[i].c = v2; break;
        case 'p': for (size_t i = 0; i < n; ++i) if (g[i].p == v1) g[i].p = v2; break;
        case 'i': for (size_t i = 0; i < n; ++i) if (g[i].i == v1) g[i].i = v2; break;
        case 'r': for (size_t i = 0; i < n; ++i) if (g[i].r == v1) g[i].r = v2; break;
        case 'g': for (size_t i = 0; i < n; ++i) if (g[i].g == v1) g[i].g = v2; break;
        case 'b': for (size_t i = 0; i < n; ++i) if (g[i].b == v1) g[i].b = v2; break;
        case '0': case '1': case '2': case '3':
        case '4': case '5': case '6': case '7':
        case '8': case '9':
        {
              const size_t j = field_name - '0';
              for (size_t i = 0; i < n; ++i)
              {
                  if (j >= g[i].extra.size ())
                      throw std::runtime_error ("Specified extra index is too large");
                  if (g[i].extra[j] == v1)
                      g[i].extra[j] = v2;
              }
              break;
        }
    }

    return g;
}

// Recenter a point cloud about its mean
spoc_file recenter (const spoc_file &f, const bool z_flag = false)
{
    // Make a copy
    spoc::spoc_file g (f);
    const auto x = get_x (g.get_point_records ());
    const auto y = get_y (g.get_point_records ());
    const auto z = get_z (g.get_point_records ());
    const auto n = f.get_header ().total_points;

    // Get average X, Y, and (optionally) Z
    const double cx = std::accumulate (begin (x), end (x), 0.0) / n;
    const double cy = std::accumulate (begin (y), end (y), 0.0) / n;
    const double cz = z_flag ? std::accumulate (begin (z), end (z), 0.0) / n : 0.0;

    // Subtract off mean x, y, z
    for (size_t i = 0; i < n; ++i)
    {
        auto p = g[i];
        p.x -= cx;
        p.y -= cy;
        p.z -= cz;
        g[i] = p;
    }
    return g;
}

// Subtract min x/y/z values from all values
spoc_file subtract_min (const spoc_file &f, const bool z_flag = false)
{
    // Make a copy
    spoc::spoc_file g (f);
    const auto x = get_x (g.get_point_records ());
    const auto y = get_y (g.get_point_records ());
    const auto z = get_z (g.get_point_records ());
    const auto n = g.get_header ().total_points;

    // Get min X, Y, and (optionally) Z
    const double minx = *std::min_element (begin (x), end (x));
    const double miny = *std::min_element (begin (y), end (y));
    const double minz = z_flag ? *std::min_element (begin (z), end (z)) : 0.0;

    // Subtract off min x, y, z
    for (size_t i = 0; i < n; ++i)
    {
        auto p = g[i];
        p.x -= minx;
        p.y -= miny;
        p.z -= minz;
        g[i] = p;
    }
    return g;
}

spoc_file quantize (const spoc_file &f, const double precision)
{
    // Get number of points
    const size_t n = f.get_header ().total_points;

    // Return value
    spoc_file g (f);

    for (size_t i = 0; i < n; ++i)
    {
        g[i].x = static_cast<int> (g[i].x / precision) * precision;
        g[i].y = static_cast<int> (g[i].y / precision) * precision;
        g[i].z = static_cast<int> (g[i].z / precision) * precision;
    }
    return g;
}

} // namespace transform

} // namespace spoc
