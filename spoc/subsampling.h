#pragma once

#include "spoc/voxel.h"
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <vector>

namespace spoc
{

namespace subsampling
{

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

        // Remember this voxel index
        s.insert (v[j]);
    }

    return g;
}

} // namespace subsampling

} // namespace spoc
