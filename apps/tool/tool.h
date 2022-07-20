#pragma once

#include "app_utils.h"
#include "spoc.h"
#include <cassert>
#include <iostream>

namespace spoc
{

namespace tool_app
{

void get_field (std::istream &is,
    std::ostream &os,
    const spoc::io::header &h,
    const std::string &field_name)
{
    // Check preconditions
    REQUIRE (is.good ());
    REQUIRE (os.good ());
    REQUIRE (h.is_valid ());
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
        : h.extra_fields + 1;

    // Write out the fields
    for (size_t i = 0; i < h.total_points; ++i)
    {
        // Read a point
        auto p = spoc::point_record::read_point_record (is, h.extra_fields);

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
void recenter (T &p, const bool z_flag = false)
{
    using namespace spoc::io;

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
    const spoc::io::header &h,
    const bool z_flag = false)
{
    // Check preconditions
    REQUIRE (is.good ());
    REQUIRE (os.good ());
    REQUIRE (h.is_valid ());

    // Get number of points
    const auto n = h.total_points;

    // Read the data
    spoc::point_record::point_records p (n);
    for (size_t i = 0; i < p.size (); ++i)
        p[i] = spoc::point_record::read_point_record (is, h.extra_fields);

    recenter (p, z_flag);

    // Write it back out
    write_header (os, h);
    for (size_t i = 0; i < n; ++i)
        write_point_record (os, p[i]);
}

void set_field (std::istream &is,
    std::ostream &os,
    std::istream &field_ifs,
    const spoc::io::header &h,
    const std::string &field_name)
{
    // Check preconditions
    REQUIRE (is.good ());
    REQUIRE (os.good ());
    REQUIRE (h.is_valid ());
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

    // Allocate the memory
    if (float_flag)
        f_double.resize (h.total_points);
    else
        f_size_t.resize (h.total_points);

    // Read the values
    for (size_t i = 0; i < h.total_points; ++i)
    {
        if (float_flag)
            field_ifs >> f_double[i];
        else
            field_ifs >> f_size_t[i];
    }

    // Get the extra index
    const size_t j = app_utils::is_extra_field (field_name)
        ? app_utils::get_extra_index (field_name)
        : h.extra_fields + 1;

    // Write the header back out
    write_header (os, h);

    // Process the points
    for (size_t i = 0; i < h.total_points; ++i)
    {
        // Read a point
        auto p = spoc::point_record::read_point_record (is, h.extra_fields);

        // Process the point
        switch (field_name[0])
        {
            case 'x': p.x = f_double[i]; break;
            case 'y': p.y = f_double[i]; break;
            case 'z': p.z = f_double[i]; break;
            case 'c': p.c = f_size_t[i]; break;
            case 'p': p.p = f_size_t[i]; break;
            case 'i': p.i = f_size_t[i]; break;
            case 'r': p.r = f_size_t[i]; break;
            case 'g': p.g = f_size_t[i]; break;
            case 'b': p.b = f_size_t[i]; break;
            case 'e': // extra
            {
                assert (app_utils::is_extra_field (field_name));
                assert (j < p.extra.size ());
                p.extra[j] = f_size_t[i];
            }
            break;
        }

        // Write it back out
        write_point_record (os, p);
    }
}

// Get min x/y/z values from all values
template<typename T>
spoc::point::point<double> get_min (const T &p)
{
    using namespace spoc::io;

    // Get X, Y, and Z
    const auto x = get_x (p);
    const auto y = get_y (p);
    const auto z = get_z (p);

    // Get min X, Y, and (optionally) Z
    const double minx = *std::min_element (begin (x), end (x));
    const double miny = *std::min_element (begin (y), end (y));
    const double minz = *std::min_element (begin (z), end (z));

    return spoc::point::point<double> { minx, miny, minz };
}

// Subtract x/y/z value from all values
template<typename T>
void subtract (T &p, const spoc::point::point<double> &minp, const bool z_flag = true)
{
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
    const spoc::io::header &h,
    const bool z_flag = false)
{
    // Check preconditions
    REQUIRE (is.good ());
    REQUIRE (os.good ());
    REQUIRE (h.is_valid ());

    // Get number of points
    const auto n = h.total_points;

    // Read the data
    spoc::point_record::point_records p (n);
    for (size_t i = 0; i < p.size (); ++i)
        p[i] = spoc::point_record::read_point_record (is, h.extra_fields);

    // Get minimum value
    const auto minp = get_min (p);

    subtract (p, minp, z_flag);

    // Write it back out
    write_header (os, h);
    for (size_t i = 0; i < n; ++i)
        write_point_record (os, p[i]);
}

} // namespace tool_app

} // namespace spoc
