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

using PR = spoc::point_record::point_record;
using OP = std::function<PR(PR)>;

OP get_add_x_op (double v)
{
    OP op = [=] (PR p) { p.x += v; return p; };
    return op;
}

OP get_add_y_op (double v)
{
    OP op = [=] (PR p) { p.y += v; return p; };
    return op;
}

OP get_add_z_op (double v)
{
    OP op = [=] (PR p) { p.z += v; return p; };
    return op;
}

OP get_copy_field_op (std::string field_name1, std::string field_name2, const size_t extra_fields)
{
    using namespace spoc::app_utils;

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

    // Get the extra indexes
    const size_t j1 = is_extra_field (field_name1)
        ? get_extra_index (field_name1)
        : extra_fields + 1;
    const size_t j2 = is_extra_field (field_name2)
        ? get_extra_index (field_name2)
        : extra_fields + 1;

    // Get the operation
    OP op = [=] (PR p)
    {
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

        return p;
    };

    return op;
}

OP get_gaussian_noise_op (const size_t random_seed,
    const double std_x,
    const double std_y,
    const double std_z)
{
    // Create rng
    std::default_random_engine rng (random_seed);

    // Create the distributions
    std::normal_distribution<double> dx (0.0, std_x);
    std::normal_distribution<double> dy (0.0, std_y);
    std::normal_distribution<double> dz (0.0, std_z);

    // Get the operation
    OP op = [=] (PR p) mutable
    {
        if (std_x != 0.0)
            p.x += dx (rng);
        if (std_y != 0.0)
            p.y += dy (rng);
        if (std_z != 0.0)
            p.z += dz (rng);
        return p;
    };

    return op;
}

// Set X, Y, Z to nearest precision by truncating
OP get_quantize_op (const double precision)
{
    // Get the operation
    OP op = [=] (PR p)
    {
        // Quantize the point
        p.x = static_cast<int> (p.x / precision) * precision;
        p.y = static_cast<int> (p.y / precision) * precision;
        p.z = static_cast<int> (p.z / precision) * precision;
        return p;
    };

    return op;
}

/*
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
*/

template<typename T>
void apply (std::istream &is,
    std::ostream &os,
    const T &commands,
    const size_t random_seed)
{
    // Check preconditions
    REQUIRE (is.good ());
    REQUIRE (os.good ());

    // Read the header and make sure it's uncompressed
    const auto h = detail::read_header_uncompressed (is);
    REQUIRE (h.is_valid ());

    // Write the same header to the output stream
    write_header (os, h);

    using PR = spoc::point_record::point_record;
    std::vector<std::function<PR(PR)>> ops;

    using namespace spoc::app_utils;

    // Check the commands
    for (auto c : commands)
    {
        if (c.name == "add-x")
        {
            std::string s = c.params;
            const auto v = consume_double (s);
            ops.push_back (get_add_x_op (v));
        }
        else if (c.name == "add-y")
        {
            std::string s = c.params;
            const auto v = consume_double (s);
            ops.push_back (get_add_y_op (v));
        }
        else if (c.name == "add-z")
        {
            std::string s = c.params;
            const auto v = consume_double (s);
            ops.push_back (get_add_z_op (v));
        }
        else if (c.name == "copy-field")
        {
            std::string s = c.params;
            const auto f1 = consume_field_name (s);
            const auto f2 = consume_field_name (s);
            ops.push_back (get_copy_field_op (f1, f2, h.extra_fields));
        }
        else if (c.name == "gaussian-noise")
        {
            // Get variance for X, Y, Z
            std::string s = c.params;
            const auto v = consume_double (s);
            ops.push_back (get_gaussian_noise_op (random_seed, v, v, v));
        }
        else if (c.name == "gaussian-noise-x")
        {
            // Get variance for X
            std::string s = c.params;
            const auto v = consume_double (s);
            ops.push_back (get_gaussian_noise_op (random_seed, v, 0.0, 0.0));
        }
        else if (c.name == "gaussian-noise-y")
        {
            // Get variance for Y
            std::string s = c.params;
            const auto v = consume_double (s);
            ops.push_back (get_gaussian_noise_op (random_seed, 0.0, v, 0.0));
        }
        else if (c.name == "gaussian-noise-z")
        {
            // Get variance for Z
            std::string s = c.params;
            const auto v = consume_double (s);
            ops.push_back (get_gaussian_noise_op (random_seed, 0.0, 0.0, v));
        }
        else if (c.name == "quantize-xyz")
        {
            // Get precision
            std::string s = c.params;
            const auto v = consume_double (s);
            ops.push_back (get_quantize_op (v));
        }
        else
        {
            throw std::runtime_error (std::string ("An unknown command was encountered: ") + c.name);
        }
        /*
        else if (c.name == "replace")
        {
            string s = c.params;
            const auto l = consume_field_name (s);
            const auto v1 = consume_int (s);
            const auto v2 = consume_int (s);
            replace (is (), os (), l, v1, v2);
        }
        else if (c.name == "rotate-x")
        {
            string s = c.params;
            const auto v = consume_double (s);
            rotate_x (is (), os (), v);
        }
        else if (c.name == "rotate-y")
        {
            string s = c.params;
            const auto v = consume_double (s);
            rotate_y (is (), os (), v);
        }
        else if (c.name == "rotate-z")
        {
            string s = c.params;
            const auto v = consume_double (s);
            rotate_z (is (), os (), v);
        }
        else if (c.name == "scale-x")
        {
            string s = c.params;
            const auto v = consume_double (s);
            scale_x (is (), os (), v);
        }
        else if (c.name == "scale-y")
        {
            string s = c.params;
            const auto v = consume_double (s);
            scale_y (is (), os (), v);
        }
        else if (c.name == "scale-z")
        {
            string s = c.params;
            const auto v = consume_double (s);
            scale_z (is (), os (), v);
        }
        else if (c.name == "set")
        {
            string s = c.params;
            const auto l = consume_field_name (s);
            const auto v = consume_double (s);
            spoc::transform_app::set (is (), os (), l, v);
        }
        else if (c.name == "uniform-noise")
        {
            string s = c.params;
            const auto v = consume_double (s);
            uniform_noise (is (), os (), args.random_seed, v, v, v);
        }
        else if (c.name == "uniform-noise-x")
        {
            string s = c.params;
            const auto v = consume_double (s);
            uniform_noise (is (), os (), args.random_seed, v, 0.0, 0.0);
        }
        else if (c.name == "uniform-noise-y")
        {
            string s = c.params;
            const auto v = consume_double (s);
            uniform_noise (is (), os (), args.random_seed, 0.0, v, 0.0);
        }
        else if (c.name == "uniform-noise-z")
        {
            string s = c.params;
            const auto v = consume_double (s);
            uniform_noise (is (), os (), args.random_seed, 0.0, 0.0, v);
        }
        else
            throw runtime_error (std::string ("An unknown command was encountered: ") + c.name);
        if (c.name == "copy-field")
        {
            string s = c.params;
            const auto f1 = consume_field_name (s);
            const auto f2 = consume_field_name (s);

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
            ops.push_back (get_cop
        }
        else if (c.name == "gaussian-noise")
        {
        }
        */
    }

    // Process the points
    for (size_t i = 0; i < h.total_points; ++i)
    {
        // Read a point
        auto p = spoc::point_record::read_point_record (is, h.extra_fields);

        // Apply each operation, one by one
        for (auto &op : ops)
            p = op (p);

        /*
            else if (c.name == "quantize-xyz")
            {
                string s = c.params;
                const auto v = consume_double (s);
                quantize (is (), os (), v);
            }
            else if (c.name == "replace")
            {
                string s = c.params;
                const auto l = consume_field_name (s);
                const auto v1 = consume_int (s);
                const auto v2 = consume_int (s);
                replace (is (), os (), l, v1, v2);
            }
            else if (c.name == "rotate-x")
            {
                string s = c.params;
                const auto v = consume_double (s);
                rotate_x (is (), os (), v);
            }
            else if (c.name == "rotate-y")
            {
                string s = c.params;
                const auto v = consume_double (s);
                rotate_y (is (), os (), v);
            }
            else if (c.name == "rotate-z")
            {
                string s = c.params;
                const auto v = consume_double (s);
                rotate_z (is (), os (), v);
            }
            else if (c.name == "scale-x")
            {
                string s = c.params;
                const auto v = consume_double (s);
                scale_x (is (), os (), v);
            }
            else if (c.name == "scale-y")
            {
                string s = c.params;
                const auto v = consume_double (s);
                scale_y (is (), os (), v);
            }
            else if (c.name == "scale-z")
            {
                string s = c.params;
                const auto v = consume_double (s);
                scale_z (is (), os (), v);
            }
            else if (c.name == "set")
            {
                string s = c.params;
                const auto l = consume_field_name (s);
                const auto v = consume_double (s);
                spoc::transform_app::set (is (), os (), l, v);
            }
            else if (c.name == "uniform-noise")
            {
                string s = c.params;
                const auto v = consume_double (s);
                uniform_noise (is (), os (), args.random_seed, v, v, v);
            }
            else if (c.name == "uniform-noise-x")
            {
                string s = c.params;
                const auto v = consume_double (s);
                uniform_noise (is (), os (), args.random_seed, v, 0.0, 0.0);
            }
            else if (c.name == "uniform-noise-y")
            {
                string s = c.params;
                const auto v = consume_double (s);
                uniform_noise (is (), os (), args.random_seed, 0.0, v, 0.0);
            }
            else if (c.name == "uniform-noise-z")
            {
                string s = c.params;
                const auto v = consume_double (s);
                uniform_noise (is (), os (), args.random_seed, 0.0, 0.0, v);
            }
            else
                throw runtime_error (std::string ("An unknown command was encountered: ") + c.name);
        }
        */

        // Write it back out
        write_point_record (os, p);
    }

}


} // namespace transform_app

} // namespace spoc
