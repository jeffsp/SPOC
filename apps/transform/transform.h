#pragma once

#include "spoc/app_utils.h"
#include "spoc/spoc.h"
#include <cassert>
#include <iostream>
#include <limits>
#include <random>
#include <vector>
#include <unordered_map>

namespace spoc
{

namespace transform_app
{

// Add offset
template<typename T>
void add_x (T &p, const double v)
{
#pragma omp parallel for
    for (size_t i = 0; i < p.size (); ++i)
        p[i].x = p[i].x + v;
}

// Add offset
template<typename T>
void add_y (T &p, const double v)
{
#pragma omp parallel for
    for (size_t i = 0; i < p.size (); ++i)
        p[i].y = p[i].y + v;
}

// Add offset
template<typename T>
void add_z (T &p, const double v)
{
#pragma omp parallel for
    for (size_t i = 0; i < p.size (); ++i)
        p[i].z = p[i].z + v;
}

// Add to X, Y, or Z
template<typename OP>
void add (std::istream &is,
    std::ostream &os,
    const spoc::header::header &h,
    OP op)
{
    // Check preconditions
    REQUIRE (is.good ());
    REQUIRE (os.good ());
    REQUIRE (h.is_valid ());

    // Write the header
    write_header (os, h);

    // Process the points
    for (size_t i = 0; i < h.total_points; ++i)
    {
        // Read a point
        auto p = spoc::point_record::read_point_record (is, h.extra_fields);

        // Scale the point
        op (p);

        // Write it back out
        write_point_record (os, p);
    }
}

namespace detail
{

const spoc::header::header read_header_uncompressed (std::istream &is)
{
    // Check preconditions
    REQUIRE (is.good ());

    // Read the header
    const spoc::header::header h = spoc::header::read_header (is);

    // Check compression flag
    if (h.compressed)
        throw std::runtime_error ("Expected an uncompressed file");

    // Check post-conditions
    ENSURE (h.is_valid ());

    return h;
}

}

void add_x (std::istream &is, std::ostream &os, const double v)
{
    // Read the header and make sure it's uncompressed
    const auto h = detail::read_header_uncompressed (is);
    auto op = [&v] (auto &p) { p.x += v; };
    add (is, os, h, op);
}

void add_y (std::istream &is, std::ostream &os, const double v)
{
    // Read the header and make sure it's uncompressed
    const auto h = detail::read_header_uncompressed (is);
    auto op = [&v] (auto &p) { p.y += v; };
    add (is, os, h, op);
}

void add_z (std::istream &is, std::ostream &os, const double v)
{
    // Read the header and make sure it's uncompressed
    const auto h = detail::read_header_uncompressed (is);
    auto op = [&v] (auto &p) { p.z += v; };
    add (is, os, h, op);
}

void copy_field (std::istream &is,
    std::ostream &os,
    const std::string &field_name1,
    const std::string &field_name2)
{
    using namespace spoc::app_utils;

    // Check preconditions
    REQUIRE (is.good ());
    REQUIRE (os.good ());
    REQUIRE (check_field_name (field_name1));
    REQUIRE (check_field_name (field_name2));

    // Check to make sure x, y, z, is not being used
    switch (field_name1[0])
    {
        case 'x': throw std::runtime_error ("Cannot run the replace command on floating point fields (X, Y, Z)");
        case 'y': throw std::runtime_error ("Cannot run the replace command on floating point fields (X, Y, Z)");
        case 'z': throw std::runtime_error ("Cannot run the replace command on floating point fields (X, Y, Z)");
    }
    switch (field_name2[0])
    {
        case 'x': throw std::runtime_error ("Cannot run the replace command on floating point fields (X, Y, Z)");
        case 'y': throw std::runtime_error ("Cannot run the replace command on floating point fields (X, Y, Z)");
        case 'z': throw std::runtime_error ("Cannot run the replace command on floating point fields (X, Y, Z)");
    }

    // Read the header and make sure it's uncompressed
    const auto h = detail::read_header_uncompressed (is);

    // Get the extra indexes
    const size_t j1 = is_extra_field (field_name1)
        ? get_extra_index (field_name1)
        : h.extra_fields + 1;
    const size_t j2 = is_extra_field (field_name2)
        ? get_extra_index (field_name2)
        : h.extra_fields + 1;

    // Write the header
    write_header (os, h);

    // Process the points
    for (size_t i = 0; i < h.total_points; ++i)
    {
        // Read a point
        auto p = spoc::point_record::read_point_record (is, h.extra_fields);

        // Get the value of the source field
        size_t src = 0;
        switch (field_name1[0])
        {
            case 'c': src = p.c; break;
            case 'p': src = p.p; break;
            case 'i': src = p.i; break;
            case 'r': src = p.r; break;
            case 'g': src = p.g; break;
            case 'b': src = p.b; break;
            case 'e': // extra
            {
                assert (is_extra_field (field_name1));
                if (j1 >= p.extra.size ())
                    throw std::runtime_error ("Invalid extra field specification");
                src = p.extra[j1];
            }
            break;
        }

        // Set it in the dest field
        switch (field_name2[0])
        {
            case 'c': p.c = src; break;
            case 'p': p.p = src; break;
            case 'i': p.i = src; break;
            case 'r': p.r = src; break;
            case 'g': p.g = src; break;
            case 'b': p.b = src; break;
            case 'e': // extra
            {
                assert (is_extra_field (field_name2));
                if (j2 >= p.extra.size ())
                    throw std::runtime_error ("Invalid extra field specification");
                p.extra[j2] = src;
            }
            break;
        }

        // Write it back out
        write_point_record (os, p);
    }
}

void gaussian_noise (std::istream &is, std::ostream &os,
    const size_t random_seed,
    const double std_x, const double std_y, const double std_z)
{
    // Create rng
    std::default_random_engine rng (random_seed);

    // Create the distributions
    std::normal_distribution<double> dx (0.0, std_x);
    std::normal_distribution<double> dy (0.0, std_y);
    std::normal_distribution<double> dz (0.0, std_z);

    // Read the header and make sure it's uncompressed
    const auto h = detail::read_header_uncompressed (is);

    // Define the operation being performed
    auto op = [&] (auto &p)
    {
        if (std_x != 0.0) p.x += dx (rng);
        if (std_y != 0.0) p.y += dy (rng);
        if (std_z != 0.0) p.z += dz (rng);
    };

    // Do it
    add (is, os, h, op);
}

// Set X, Y, Z to nearest precision by truncating
template<typename T>
void quantize (T &p, const double precision)
{
    // Check preconditions
    REQUIRE (precision != 0.0);

    // Quantize x, y, z
#pragma omp parallel for
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
    const double precision)
{
    // Check preconditions
    REQUIRE (is.good ());
    REQUIRE (os.good ());
    REQUIRE (precision != 0.0);

    // Read the header and make sure it's uncompressed
    const auto h = detail::read_header_uncompressed (is);

    // Write the header
    write_header (os, h);

    // Process the points
    for (size_t i = 0; i < h.total_points; ++i)
    {
        // Read a point
        auto p = spoc::point_record::read_point_record (is, h.extra_fields);

        // Quantize the point
        p.x = static_cast<int> (p.x / precision) * precision;
        p.y = static_cast<int> (p.y / precision) * precision;
        p.z = static_cast<int> (p.z / precision) * precision;

        // Write it back out
        write_point_record (os, p);
    }
}

// Replace values in one field with values in another
void replace (std::istream &is,
    std::ostream &os,
    const std::string &field_name,
    const double v1,
    const double v2)
{
    using namespace spoc::app_utils;

    // Check preconditions
    REQUIRE (is.good ());
    REQUIRE (os.good ());
    REQUIRE (check_field_name (field_name));

    // Check to make sure x, y, z, is not being used
    switch (field_name[0])
    {
        case 'x': throw std::runtime_error ("Cannot run the replace command on floating point fields (X, Y, Z)");
        case 'y': throw std::runtime_error ("Cannot run the replace command on floating point fields (X, Y, Z)");
        case 'z': throw std::runtime_error ("Cannot run the replace command on floating point fields (X, Y, Z)");
    }

    // Read the header and make sure it's uncompressed
    const auto h = detail::read_header_uncompressed (is);

    // Get the extra index
    const size_t j = is_extra_field (field_name)
        ? get_extra_index (field_name)
        : h.extra_fields + 1;

    // Write the header
    write_header (os, h);

    // Process the points
    for (size_t i = 0; i < h.total_points; ++i)
    {
        // Read a point
        auto p = spoc::point_record::read_point_record (is, h.extra_fields);

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
                assert (is_extra_field (field_name));
                if (j >= p.extra.size ())
                    throw std::runtime_error ("Invalid extra field specification");
                if (p.extra[j] == v1)
                    p.extra[j] = v2;
            }
            break;
        }

        // Write it back out
        write_point_record (os, p);
    }
}

// Rotate about the X, Y, or Z axis
template<typename T,typename XOP,typename YOP,typename ZOP>
void rotate (T &p, XOP xop, YOP yop, ZOP zop)
{
    // Process the points
#pragma omp parallel for
    for (size_t i = 0; i < p.size (); ++i)
    {
        const double x = p[i].x;
        const double y = p[i].y;
        const double z = p[i].z;

        // Rotate the point
        p[i].x = xop (x, y, z);
        p[i].y = yop (x, y, z);
        p[i].z = zop (x, y, z);
    }
}

template<typename T>
void rotate_x (T &p, const double degrees)
{
    // Convert degrees to radians
    const double r = degrees * M_PI / 180.0;
    auto xop = [&r] (const double x, const double y, const double z) -> double { return x; };
    auto yop = [&r] (const double x, const double y, const double z) -> double { return y * cos (r) - z * sin (r); };
    auto zop = [&r] (const double x, const double y, const double z) -> double { return y * sin (r) + z * cos (r); };
    rotate (p, xop, yop, zop);
}

template<typename T>
void rotate_y (T &p, const double degrees)
{
    // Convert degrees to radians
    const double r = degrees * M_PI / 180.0;
    auto xop = [&r] (const double x, const double y, const double z) -> double { return x * cos (r) - z * sin (r); };
    auto yop = [&r] (const double x, const double y, const double z) -> double { return y; };
    auto zop = [&r] (const double x, const double y, const double z) -> double { return x * sin (r) + z * cos (r); };
    rotate (p, xop, yop, zop);
}

template<typename T>
void rotate_z (T &p, const double degrees)
{
    // Convert degrees to radians
    const double r = degrees * M_PI / 180.0;
    auto xop = [&r] (const double x, const double y, const double z) -> double { return x * cos (r) - y * sin (r); };
    auto yop = [&r] (const double x, const double y, const double z) -> double { return x * sin (r) + y * cos (r); };
    auto zop = [&r] (const double x, const double y, const double z) -> double { return z; };
    rotate (p, xop, yop, zop);
}

// Rotate about the X, Y, or Z axis
template<typename XOP,typename YOP,typename ZOP>
void rotate (std::istream &is,
    std::ostream &os,
    const spoc::header::header &h,
    XOP xop,
    YOP yop,
    ZOP zop)
{
    // Check preconditions
    REQUIRE (is.good ());
    REQUIRE (os.good ());
    REQUIRE (h.is_valid ());

    // Write the header
    write_header (os, h);

    // Process the points
    for (size_t i = 0; i < h.total_points; ++i)
    {
        // Read a point
        auto p = spoc::point_record::read_point_record (is, h.extra_fields);
        const double x = p.x;
        const double y = p.y;
        const double z = p.z;

        // Rotate the point
        p.x = xop (x, y, z);
        p.y = yop (x, y, z);
        p.z = zop (x, y, z);

        // Write it back out
        write_point_record (os, p);
    }
}

void rotate_x (std::istream &is, std::ostream &os, const double degrees)
{
    // Read the header and make sure it's uncompressed
    const auto h = detail::read_header_uncompressed (is);

    // Convert degrees to radians
    const double r = degrees * M_PI / 180.0;
    auto xop = [&r] (const double x, const double y, const double z) -> double { return x; };
    auto yop = [&r] (const double x, const double y, const double z) -> double { return y * cos (r) - z * sin (r); };
    auto zop = [&r] (const double x, const double y, const double z) -> double { return y * sin (r) + z * cos (r); };
    rotate (is, os, h, xop, yop, zop);
}

void rotate_y (std::istream &is, std::ostream &os, const double degrees)
{
    // Read the header and make sure it's uncompressed
    const auto h = detail::read_header_uncompressed (is);

    // Convert degrees to radians
    const double r = degrees * M_PI / 180.0;
    auto xop = [&r] (const double x, const double y, const double z) -> double { return x * cos (r) - z * sin (r); };
    auto yop = [&r] (const double x, const double y, const double z) -> double { return y; };
    auto zop = [&r] (const double x, const double y, const double z) -> double { return x * sin (r) + z * cos (r); };
    rotate (is, os, h, xop, yop, zop);
}

void rotate_z (std::istream &is, std::ostream &os, const double degrees)
{
    // Read the header and make sure it's uncompressed
    const auto h = detail::read_header_uncompressed (is);

    // Convert degrees to radians
    const double r = degrees * M_PI / 180.0;
    auto xop = [&r] (const double x, const double y, const double z) -> double { return x * cos (r) - y * sin (r); };
    auto yop = [&r] (const double x, const double y, const double z) -> double { return x * sin (r) + y * cos (r); };
    auto zop = [&r] (const double x, const double y, const double z) -> double { return z; };
    rotate (is, os, h, xop, yop, zop);
}

// Scale
template<typename T>
void scale_x (T &p, const double v)
{
#pragma omp parallel for
    for (size_t i = 0; i < p.size (); ++i)
        p[i].x = p[i].x * v;
}

// Scale
template<typename T>
void scale_y (T &p, const double v)
{
#pragma omp parallel for
    for (size_t i = 0; i < p.size (); ++i)
        p[i].y = p[i].y * v;
}

// Scale
template<typename T>
void scale_z (T &p, const double v)
{
#pragma omp parallel for
    for (size_t i = 0; i < p.size (); ++i)
        p[i].z = p[i].z * v;
}

// Scale X, Y, or Z
template<typename OP>
void scale (std::istream &is,
    std::ostream &os,
    const spoc::header::header &h,
    OP op)
{
    // Check preconditions
    REQUIRE (is.good ());
    REQUIRE (os.good ());
    REQUIRE (h.is_valid ());

    // Write the header
    write_header (os, h);

    // Process the points
    for (size_t i = 0; i < h.total_points; ++i)
    {
        // Read a point
        auto p = spoc::point_record::read_point_record (is, h.extra_fields);

        // Scale the point
        op (p);

        // Write it back out
        write_point_record (os, p);
    }
}

void scale_x (std::istream &is, std::ostream &os, const double v)
{
    // Read the header and make sure it's uncompressed
    const auto h = detail::read_header_uncompressed (is);
    auto op = [&v] (auto &p) { p.x *= v; };
    scale (is, os, h, op);
}

void scale_y (std::istream &is, std::ostream &os, const double v)
{
    // Read the header and make sure it's uncompressed
    const auto h = detail::read_header_uncompressed (is);
    auto op = [&v] (auto &p) { p.y *= v; };
    scale (is, os, h, op);
}

void scale_z (std::istream &is, std::ostream &os, const double v)
{
    // Read the header and make sure it's uncompressed
    const auto h = detail::read_header_uncompressed (is);
    auto op = [&v] (auto &p) { p.z *= v; };
    scale (is, os, h, op);
}

// Set field values
void set (std::istream &is,
    std::ostream &os,
    const std::string &field_name,
    const double v)
{
    using namespace spoc::app_utils;

    // Check preconditions
    REQUIRE (is.good ());
    REQUIRE (os.good ());
    REQUIRE (check_field_name (field_name));

    // Read the header and make sure it's uncompressed
    const auto h = detail::read_header_uncompressed (is);

    // Get the extra index
    const size_t j = is_extra_field (field_name)
        ? get_extra_index (field_name)
        : h.extra_fields + 1;

    // Write the header
    write_header (os, h);

    // Process the points
    for (size_t i = 0; i < h.total_points; ++i)
    {
        // Read a point
        auto p = spoc::point_record::read_point_record (is, h.extra_fields);

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
                assert (is_extra_field (field_name));
                if (j >= p.extra.size ())
                    throw std::runtime_error ("Invalid extra field specification");
                p.extra[j] = v;
            }
            break;
        }

        // Write it back out
        write_point_record (os, p);
    }
}

void uniform_noise (std::istream &is, std::ostream &os,
    const size_t random_seed,
    const double mag_x, const double mag_y, const double mag_z)
{
    // Create rng
    std::default_random_engine rng (random_seed);

    // Create the distributions
    std::uniform_real_distribution<double> dx (-mag_x, mag_x);
    std::uniform_real_distribution<double> dy (-mag_y, mag_y);
    std::uniform_real_distribution<double> dz (-mag_z, mag_z);

    // Read the header and make sure it's uncompressed
    const auto h = detail::read_header_uncompressed (is);

    // Define the operation being performed
    auto op = [&] (auto &p)
    {
        if (mag_x != 0.0) p.x += dx (rng);
        if (mag_y != 0.0) p.y += dy (rng);
        if (mag_z != 0.0) p.z += dz (rng);
    };

    // Do it
    add (is, os, h, op);
}

} // namespace transform_app

} // namespace spoc
