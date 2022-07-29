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
inline std::vector<size_t> subsample (const T &p, const double res, const size_t random_seed)
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

} // namespace subsampling

} // namespace spoc
