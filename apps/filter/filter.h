#pragma once

#include "app_utils.h"
#include "spoc.h"
#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <limits>
#include <random>
#include <vector>
#include <unordered_map>

namespace spoc
{

namespace filter_app
{

template<typename T,typename U>
inline T keep_classes (const T &f, const U &c)
{
    // Get an empty clone of the spoc file
    T g = f.clone_empty ();

    // Get a reference to the records
    const auto &prs = f.get_point_records ();

    // For each record
    for (const auto &p : prs)
        // If it's in the set, keep it
        if (c.find (p.c) != c.end ())
            g.add (p);

    return g;
}

template<typename T,typename U>
inline T remove_classes (const T &f, const U &c)
{
    // Get an empty clone of the spoc file
    T g = f.clone_empty ();

    // Get a reference to the records
    const auto &prs = f.get_point_records ();

    // For each record
    for (const auto &p : prs)
        // If it's not in the set, keep it
        if (c.find (p.c) == c.end ())
            g.add (p);

    return g;
}

namespace detail
{

struct xyz
{
    double x, y, z;
    bool operator== (const xyz &other) const
    {
        return other.x == x
            && other.y == y
            && other.z == z;
    }
};

// Compute a hash value from an xyz struct
struct xyz_hash
{
    std::size_t operator() (const xyz &p) const
    {
        // Combine X, Y, Z
        size_t h = 0;
        utils::hash_combine (h, p.x, p.y, p.z);
        return h;
    }
};

using xyz_set = std::unordered_set<xyz,xyz_hash>;

} // namespace detail

template<typename T>
inline T unique_xyz (const T &f, const size_t random_seed)
{
    // Get an empty clone of the spoc file
    T g = f.clone_empty ();

    // Get a reference to the records
    const auto &prs = f.get_point_records ();

    // Save each unique xyz location
    detail::xyz_set xyzs;

    // Get indexes into the point cloud
    std::vector<size_t> indexes (prs.size ());

    // 0, 1, 2, ...
    std::iota (indexes.begin (), indexes.end (), 0);

    // Should we do this in a random order?
    if (random_seed != 0)
    {
        // Yes, create rng
        std::default_random_engine rng (random_seed);

        // Randomize order
        std::shuffle (indexes.begin (), indexes.end (), rng);
    }

    // For each record
    for (size_t i = 0; i < prs.size (); ++i)
    {
        // Check logic
        assert (indexes[i] < prs.size ());

        // Get the point
        const auto p = prs[indexes[i]];

        // Get the xyz location
        detail::xyz l {p.x, p.y, p.z};

        // Have we encountered it before?
        if (xyzs.find (l) != xyzs.end ())
            continue; // Yes, skip it

        g.add (p); // No, save the point

        // Remember its location
        xyzs.insert (l);
    }

    return g;
}

template<typename T>
inline T subsample (const T &f, const double res, const size_t random_seed)
{
    // Get an empty clone of the spoc file
    T g = f.clone_empty ();

    // Get a reference to the records
    const auto &prs = f.get_point_records ();

    // Get indexes into the point cloud
    std::vector<size_t> indexes (prs.size ());

    // 0, 1, 2, ...
    std::iota (indexes.begin (), indexes.end (), 0);

    // Should we do this in a random order?
    if (random_seed != 0)
    {
        // Yes, create rng
        std::default_random_engine rng (random_seed);

        // Randomize order
        std::shuffle (indexes.begin (), indexes.end (), rng);
    }

    using namespace spoc::voxel;

    // Get the voxel indexes for each point
    const auto v = get_voxel_indexes (f.get_point_records (), res);

    // Save the set of all unique voxel indexes
    voxel_index_set s;

    // For each record
    for (size_t i = 0; i < prs.size (); ++i)
    {
        // Get the index
        const auto j = indexes[i];

        // Check logic
        assert (j < prs.size ());
        assert (j < v.size ());

        // Have we encountered this one before?
        if (s.find (v[j]) != s.end ())
            continue; // Yes, skip it
        // No, save the point
        g.add (prs[j]);

        // Remember its voxel index
        s.insert (v[j]);
    }

    return g;
}

} // namespace filter_app

} // namespace spoc
