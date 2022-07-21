#pragma once

#include "app_utils.h"
#include "spoc.h"
#include <cassert>
#include <iostream>

namespace spoc
{

namespace tool_app
{

template<typename T>
inline void get_field (const T &f, std::ostream &os, const std::string &field_name)
{
    // Check preconditions
    REQUIRE (os.good ());
    REQUIRE (app_utils::check_field_name (field_name));

    // Set the output precision for doubles
    switch (field_name[0])
    {
        default:
        // do nothing for non-double fields
        break;
        case 'x':
        case 'y':
        case 'z':
            os << std::setprecision(std::numeric_limits<double>::digits10 + 1);
        break;
    }

    // Get the extra index
    const size_t j = app_utils::is_extra_field (field_name)
        ? app_utils::get_extra_index (field_name)
        : f.get_header ().extra_fields + 1;

    // Write out the fields
    for (const auto &p : f.get_point_records ())
    {
        // Process the point
        switch (field_name[0])
        {
            case 'x': os << p.x << std::endl; break;
            case 'y': os << p.y << std::endl; break;
            case 'z': os << p.z << std::endl; break;
            case 'c': os << p.c << std::endl; break;
            case 'p': os << p.p << std::endl; break;
            case 'i': os << p.i << std::endl; break;
            case 'r': os << p.r << std::endl; break;
            case 'g': os << p.g << std::endl; break;
            case 'b': os << p.b << std::endl; break;
            case 'e': // extra
            {
                assert (app_utils::is_extra_field (field_name));
                assert (j < p.extra.size ());
                os << p.extra[j] << std::endl;
            }
            break;
        }
    }
}

// Recenter a point cloud about its mean
template<typename T>
inline T recenter (const T &f, const bool z_flag = false)
{
    // Check preconditions
    REQUIRE (f.is_valid ());

    // Sum x, y, and z
    double sx = 0.0;
    double sy = 0.0;
    double sz = 0.0;
    for (const auto &p : f.get_point_records ())
    {
        sx += p.x;
        sy += p.y;
        if (z_flag)
            sz += p.z;
    }

    // Get mean x, y, and z
    const size_t n = f.get_point_records ().size ();
    double cx = n ? sx / n : 0.0;
    double cy = n ? sy / n : 0.0;
    double cz = n ? sz / n : 0.0;

    // Copy file
    T g (f);

    // Recenter its points
    for (size_t i = 0; i < n; ++i)
    {
        g[i].x -= cx;
        g[i].y -= cy;
        if (z_flag)
            g[i].z -= cz;
    }

    // Return the copy
    return g;
}

template<typename T>
inline T set_field (const T &f, std::istream &field_ifs, const std::string &field_name)
{
    // Check preconditions
    REQUIRE (f.is_valid ());
    REQUIRE (field_ifs.good ());
    REQUIRE (app_utils::check_field_name (field_name));

    // Read in the fields that you are going to set
    std::vector<double> f_double;
    std::vector<size_t> f_size_t;

    // We are reading in floating points or integers
    bool float_flag = false;

    switch (field_name[0])
    {
        case 'x':
        case 'y':
        case 'z':
        float_flag = true;
    }

    // Get the number of points
    const size_t n = f.get_point_records ().size ();

    // Allocate the memory
    if (float_flag)
        f_double.resize (n);
    else
        f_size_t.resize (n);

    // Read the values
    for (size_t i = 0; i < n; ++i)
    {
        // Read the field value
        if (float_flag)
            field_ifs >> f_double[i];
        else
            field_ifs >> f_size_t[i];

        // Make sure there are still values to read
        if (field_ifs.eof ())
            throw std::runtime_error ("Reached end-of-file before reading all field values");
    }

    // Get the extra index
    const size_t j = app_utils::is_extra_field (field_name)
        ? app_utils::get_extra_index (field_name)
        : f.get_header ().extra_fields + 1;

    // Copy the file
    T g (f);

    // Process the points
    for (size_t i = 0; i < n; ++i)
    {
        // Process the point
        switch (field_name[0])
        {
            case 'x': g[i].x = f_double[i]; break;
            case 'y': g[i].y = f_double[i]; break;
            case 'z': g[i].z = f_double[i]; break;
            case 'c': g[i].c = f_size_t[i]; break;
            case 'p': g[i].p = f_size_t[i]; break;
            case 'i': g[i].i = f_size_t[i]; break;
            case 'r': g[i].r = f_size_t[i]; break;
            case 'g': g[i].g = f_size_t[i]; break;
            case 'b': g[i].b = f_size_t[i]; break;
            case 'e': // extra
            {
                assert (app_utils::is_extra_field (field_name));
                assert (j < g[i].extra.size ());
                g[i].extra[j] = f_size_t[i];
            }
            break;
        }
    }

    // Return copy
    return g;
}

// Subtract min x/y/z values from all values
template<typename T>
inline T subtract_min (T &f, const bool z_flag = false)
{
    // Check preconditions
    REQUIRE (f.is_valid ());

    // Get minimum value
    const auto e = extent::get_extent (f.get_point_records ());

    // Copy it
    T g (f);

    // Recenter its points
    for (size_t i = 0; i < g.get_point_records ().size (); ++i)
    {
        g[i].x -= e.minp.x;
        g[i].y -= e.minp.y;
        if (z_flag)
            g[i].z -= e.minp.z;
    }

    // Return copy
    return g;
}

} // namespace tool_app

} // namespace spoc
