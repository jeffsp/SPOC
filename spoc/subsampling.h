#pragma once

#include "spoc/point.h"
#include "spoc/voxel.h"
#include <algorithm>
#include <iostream>
#include <unordered_set>
#include <vector>

namespace spoc
{

namespace subsampling
{

template<typename T>
inline std::vector<size_t> get_subsample_indexes (const T &p, const double res, const size_t random_seed)
{
    // Get indexes into the point cloud
    std::vector<size_t> indexes (p.size ());

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
    const auto v = get_voxel_indexes (p, res);

    // Save the set of all unique voxel indexes
    voxel_index_set s;

    // The return value indexes
    std::vector<size_t> r;

    r.reserve (p.size ());

    // For each record
    for (size_t i = 0; i < p.size (); ++i)
    {
        // Get the index
        const auto j = indexes[i];

        // Check logic
        assert (j < p.size ());
        assert (j < v.size ());

        // Have we encountered this one before?
        if (s.find (v[j]) != s.end ())
            continue; // Yes, skip it

        // No, save the index
        r.push_back (j);

        // Remember this voxel index
        s.insert (v[j]);
    }

    return r;
}

template<typename T>
inline std::vector<size_t> get_unique_xyz_indexes (const T &prs, const size_t random_seed)
{
    // Save each unique xyz location
    std::unordered_set<spoc::point::point<double>,spoc::point::point_hash<double>> xyzs;

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

    // Return value
    std::vector<size_t> r;

    // For each record
    for (size_t i = 0; i < prs.size (); ++i)
    {
        // Get the index
        const auto j = indexes[i];

        // Check logic
        assert (j < prs.size ());

        // Get the point
        const auto p = prs[j];

        // Get the xyz location
        spoc::point::point<double> l {p.x, p.y, p.z};

        // Have we encountered it before?
        if (xyzs.find (l) != xyzs.end ())
            continue; // Yes, skip it

        r.push_back (j); // No, save the index

        // Remember its location
        xyzs.insert (l);
    }

    return r;
}

} // namespace subsampling

} // namespace spoc
