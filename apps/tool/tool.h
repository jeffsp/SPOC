#pragma once

#include "spoc/app_utils.h"
#include "spoc/spoc.h"
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

// Resize the extra fields
template<typename T>
inline T resize_extra (const T &f, const int sz)
{
    // Check preconditions
    REQUIRE (f.is_valid ());
    REQUIRE (sz >= 0);

    // Copy file
    T g (f);

    // Change size
    g.resize_extra (sz);

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
inline T subtract_min (const T &f, const bool z_flag = false)
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

// Upsample classifications in 'l' to 'f'
template<typename T>
inline T upsample_classifications (
    const T &l,
    const T &f,
    const double resolution,
    const bool verbose,
    std::ostream &log)
{
    // Check preconditions
    REQUIRE (l.is_valid ());
    REQUIRE (f.is_valid ());
    REQUIRE (resolution > 0.0);

    using namespace spoc::voxel;

    // Copy f, the high res spoc file
    T h (f);

    // Compute h's voxel indexes, relative to the extent in 'h'
    const auto vh = get_voxel_indexes (f.get_point_records (), resolution);

    // Compute l's voxel indexes, relative to the extent in 'h' (not l)
    const auto eh = spoc::extent::get_extent (h.get_point_records ());
    const auto vl = get_voxel_indexes (l.get_point_records (), eh, resolution);

    // Get a map of voxel indexes to point record indexes in 'h'
    const auto vmh = get_voxel_index_map (vh);

    // Count the number of high resolution point records that get
    // assigned to a low resolution classification
    size_t assigned_points = 0;

    // For each voxel index in the low resolution spoc file...
    for (size_t i = 0; i < vl.size (); ++i)
    {
        // Get the voxel index
        const auto &v = vl[i];

        // Does it correspond to a voxel in the high resolution spoc
        // file?
        if (vmh.find (v) == vmh.end ())
            continue; // No, skip to next voxel

        // Get indexes of the point records that are in this high
        // resolution voxel
        const auto &indexes = vmh.at (v);

        // Check our logic, if it matched a voxel, it can't be empty
        assert (!indexes.empty ());

        // Get a reference to the low resolution point record
        const auto &p = l[i];

        // For each point record index in this high resolution voxel...
        for (const auto j : indexes)
        {
            // Set the high resolution point record's classification to
            // the low resolution classification
            assert (j < h.get_point_records ().size ());
            h[j].c = p.c;

            // Count the assignment
            ++assigned_points;
        }
    }

    if (verbose)
    {
        log << assigned_points
            << " of "
            << h.get_point_records ().size ()
            << " upsampled point record classifications"
            << " were assigned a value from the subsampled point records."
            << std::endl;
        if (assigned_points != h.get_point_records ().size ())
            log << "Was the low resolution spoc file subsampled from the"
                << " high resolution spoc file and at the same resolution?";
    }

    // Return the modified copy
    return h;
}

} // namespace tool_app

} // namespace spoc
