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

OP get_copy_field_op (const std::string &field_name1,
    const std::string &field_name2,
    const size_t extra_fields)
{
    using namespace spoc::app_utils;

    // Check to make sure x, y, z, is not being used
    switch (field_name1[0])
    {
        case 'x':
        case 'y':
        case 'z':
            throw std::runtime_error ("Cannot run the copy command on floating point fields (X, Y, Z)");
    }
    switch (field_name2[0])
    {
        case 'x':
        case 'y':
        case 'z':
            throw std::runtime_error ("Cannot run the copy command on floating point fields (X, Y, Z)");
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

// Replace values in a field
OP get_replace_op (const std::string &field_name,
    const unsigned v1,
    const unsigned v2,
    const size_t extra_fields)
{
    using namespace spoc::app_utils;

    // Check to make sure x, y, z, is not being used
    switch (field_name[0])
    {
        case 'x': throw std::runtime_error ("Cannot run the replace command on floating point fields (X, Y, Z)");
        case 'y': throw std::runtime_error ("Cannot run the replace command on floating point fields (X, Y, Z)");
        case 'z': throw std::runtime_error ("Cannot run the replace command on floating point fields (X, Y, Z)");
    }

    // Get the extra index
    const size_t j = is_extra_field (field_name)
        ? get_extra_index (field_name)
        : extra_fields + 1;

    // Get the operation
    OP op = [=] (PR p)
    {
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

        return p;
    };

    return op;
}

// Replace all values that are not equal to a value with another value
OP get_replace_not_op (const std::string &field_name,
    std::vector<unsigned> v,
    const size_t extra_fields)
{
    using namespace spoc::app_utils;

    // Check to make sure x, y, z, is not being used
    switch (field_name[0])
    {
        case 'x': throw std::runtime_error ("Cannot run the replace command on floating point fields (X, Y, Z)");
        case 'y': throw std::runtime_error ("Cannot run the replace command on floating point fields (X, Y, Z)");
        case 'z': throw std::runtime_error ("Cannot run the replace command on floating point fields (X, Y, Z)");
    }

    // Get the last value from the vector for the value to replace with
    if (v.size () < 2)
        throw std::runtime_error ("Not enough values were specified in the replace-not command");

    // Get the last value
    const int r = v.back ();

    // Remove it
    v.pop_back ();

    // Get the extra index
    const size_t j = is_extra_field (field_name)
        ? get_extra_index (field_name)
        : extra_fields + 1;

    // Get the operation
    OP op = [=] (PR p)
    {
        // Assume that it will get replaced
        bool replace = true;

        // For each value in the list
        for (auto i : v)
        {
            // If the field's value is equal to one of those in the
            // list, it does not get replaced
            switch (field_name[0])
            {
                case 'c': if (p.c == i) replace = false; break;
                case 'p': if (p.p == i) replace = false; break;
                case 'i': if (p.i == i) replace = false; break;
                case 'r': if (p.r == i) replace = false; break;
                case 'g': if (p.g == i) replace = false; break;
                case 'b': if (p.b == i) replace = false; break;
                case 'e': // extra
                {
                    assert (is_extra_field (field_name));
                    if (j >= p.extra.size ())
                        throw std::runtime_error ("Invalid extra field specification");
                    if (p.extra[j] == i)
                        replace = false;
                }
                break;
            }

            // Short circuit if you can
            if (replace == false)
                break;
        }

        // Replace if needed
        if (replace)
        {
            switch (field_name[0])
            {
                case 'c': p.c = r; break;
                case 'p': p.c = r; break;
                case 'i': p.c = r; break;
                case 'r': p.c = r; break;
                case 'g': p.c = r; break;
                case 'b': p.c = r; break;
                case 'e': // extra
                {
                    assert (is_extra_field (field_name));
                    p.extra[j] = r;
                }
                break;
            }
        }

        return p;
    };

    return op;
}

OP get_rotate_x_op (const double degrees)
{
    // Convert degrees to radians
    const double r = degrees * M_PI / 180.0;

    // Get the operation
    OP op = [=] (PR p)
    {
        // Rotate the point about X axis
        const auto y = p.y * cos (r) - p.z * sin (r);
        const auto z = p.y * sin (r) + p.z * cos (r);
        p.y = y;
        p.z = z;
        return p;
    };

    return op;
}

OP get_rotate_y_op (const double degrees)
{
    // Convert degrees to radians
    const double r = degrees * M_PI / 180.0;

    // Get the operation
    OP op = [=] (PR p)
    {
        // Rotate the point about Y axis
        const auto x =  p.x * cos (r) + p.z * sin (r);
        const auto z = -p.x * sin (r) + p.z * cos (r);
        p.x = x;
        p.z = z;
        return p;
    };

    return op;
}

OP get_rotate_z_op (const double degrees)
{
    // Convert degrees to radians
    const double r = degrees * M_PI / 180.0;

    // Get the operation
    OP op = [=] (PR p)
    {
        // Rotate the point about Z axis
        const auto x = p.x * cos (r) - p.y * sin (r);
        const auto y = p.x * sin (r) + p.y * cos (r);
        p.x = x;
        p.y = y;
        return p;
    };

    return op;
}

OP get_scale_x_op (double v)
{
    OP op = [=] (PR p) { p.x *= v; return p; };
    return op;
}

OP get_scale_y_op (double v)
{
    OP op = [=] (PR p) { p.y *= v; return p; };
    return op;
}

OP get_scale_z_op (double v)
{
    OP op = [=] (PR p) { p.z *= v; return p; };
    return op;
}

OP get_set_op (const std::string &field_name, const double v, const size_t extra_fields)
{
    using namespace spoc::app_utils;

    // Get the extra index
    const size_t j = is_extra_field (field_name)
        ? get_extra_index (field_name)
        : extra_fields + 1;

    // Get the operation
    OP op = [=] (PR p)
    {
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

        return p;
    };

    return op;
}

OP get_uniform_noise_op (const size_t random_seed,
    const double mag_x,
    const double mag_y,
    const double mag_z)
{
    // Create rng
    std::default_random_engine rng (random_seed);

    // Create the distributions
    std::uniform_real_distribution<double> dx (-mag_x, mag_x);
    std::uniform_real_distribution<double> dy (-mag_y, mag_y);
    std::uniform_real_distribution<double> dz (-mag_z, mag_z);

    // Get the operation
    OP op = [=] (PR p) mutable
    {
        if (mag_x != 0.0)
            p.x += dx (rng);
        if (mag_y != 0.0)
            p.y += dy (rng);
        if (mag_z != 0.0)
            p.z += dz (rng);
        return p;
    };

    return op;
}

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
            // Get offset
            std::string s = c.params;
            const auto v = consume_double (s);
            ops.push_back (get_add_x_op (v));
        }
        else if (c.name == "add-y")
        {
            // Get offset
            std::string s = c.params;
            const auto v = consume_double (s);
            ops.push_back (get_add_y_op (v));
        }
        else if (c.name == "add-z")
        {
            // Get offset
            std::string s = c.params;
            const auto v = consume_double (s);
            ops.push_back (get_add_z_op (v));
        }
        else if (c.name == "copy-field")
        {
            // Get fields
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
        else if (c.name == "replace")
        {
            // Get fields and values
            std::string s = c.params;
            const auto l = consume_field_name (s);
            const auto v1 = consume_int (s);
            const auto v2 = consume_int (s);
            ops.push_back (get_replace_op (l, v1, v2, h.extra_fields));
        }
        else if (c.name == "replace-not")
        {
            // Get fields and values
            std::string s = c.params;
            const auto l = consume_field_name (s);
            auto v = consume_ints (s);
            ops.push_back (get_replace_not_op (l, v, h.extra_fields));
        }
        else if (c.name == "rotate-x")
        {
            // Get degrees
            std::string s = c.params;
            const auto v = consume_double (s);
            ops.push_back (get_rotate_x_op (v));
        }
        else if (c.name == "rotate-y")
        {
            // Get degrees
            std::string s = c.params;
            const auto v = consume_double (s);
            ops.push_back (get_rotate_y_op (v));
        }
        else if (c.name == "rotate-z")
        {
            // Get degrees
            std::string s = c.params;
            const auto v = consume_double (s);
            ops.push_back (get_rotate_z_op (v));
        }
        else if (c.name == "scale-x")
        {
            // Get magnitude
            std::string s = c.params;
            const auto v = consume_double (s);
            ops.push_back (get_scale_x_op (v));
        }
        else if (c.name == "scale-y")
        {
            // Get magnitude
            std::string s = c.params;
            const auto v = consume_double (s);
            ops.push_back (get_scale_y_op (v));
        }
        else if (c.name == "scale-z")
        {
            // Get magnitude
            std::string s = c.params;
            const auto v = consume_double (s);
            ops.push_back (get_scale_z_op (v));
        }
        else if (c.name == "set")
        {
            // Get label and value
            std::string s = c.params;
            const auto l = consume_field_name (s);
            const auto v = consume_double (s);
            ops.push_back (get_set_op (l, v, h.extra_fields));
        }
        else if (c.name == "uniform-noise")
        {
            // Get magnitude
            std::string s = c.params;
            const auto v = consume_double (s);
            ops.push_back (get_uniform_noise_op (random_seed, v, v, v));
        }
        else if (c.name == "uniform-noise-x")
        {
            // Get magnitude
            std::string s = c.params;
            const auto v = consume_double (s);
            ops.push_back (get_uniform_noise_op (random_seed, v, 0.0, 0.0));
        }
        else if (c.name == "uniform-noise-y")
        {
            // Get magnitude
            std::string s = c.params;
            const auto v = consume_double (s);
            ops.push_back (get_uniform_noise_op (random_seed, 0.0, v, 0.0));
        }
        else if (c.name == "uniform-noise-z")
        {
            // Get magnitude
            std::string s = c.params;
            const auto v = consume_double (s);
            ops.push_back (get_uniform_noise_op (random_seed, 0.0, 0.0, v));
        }
        else
            throw std::runtime_error (std::string ("An unknown command was encountered: ") + c.name);
    }

    // Process the points
    for (size_t i = 0; i < h.total_points; ++i)
    {
        // Read a point
        auto p = spoc::point_record::read_point_record (is, h.extra_fields);

        // Apply each operation, one by one
        //
        // The reference to 'op' is required because some operations
        // hold state that is changed, like for example a random
        // number generator. Cppcheck is an error for suggesting
        // otherwise.
        for (const auto &op : ops)
            p = op (p);

        // Write it back out
        write_point_record (os, p);
    }

}

} // namespace transform_app

} // namespace spoc
