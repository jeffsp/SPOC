#pragma once

#include "voxel.h"
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <vector>

namespace spoc
{

namespace subsampling
{

template<typename T>
T decimate (const T &pc, const double resolution)
{
    // Get a voxel index for each point in the point cloud
    auto voxel_indexes = voxel::get_voxel_indexes (pc, resolution);

    // Randomize the order in which you access the point cloud in order to make certain
    // that there is no bias in the way we select a point from a voxel.
    std::vector<size_t> random_indexes (pc.size ());
    std::iota (random_indexes.begin (), random_indexes.end (), 0);
    std::random_shuffle (random_indexes.begin (), random_indexes.end ());

    // Map each voxel index to a point cloud index
    //
    // At each assignment, if an index was already at this location, you will
    // overwrite it. Since we are accessing the points in a random order, we
    // are effectively randomly selecting a single point cloud point from the
    // voxel.
    std::unordered_map<voxel::voxel_index,size_t,voxel::voxel_index_hash> vmap;
    for (size_t i = 0; i < pc.size (); ++i)
    {
        // Get the linear index into the point cloud
        const size_t linear_index = random_indexes[i];
        assert (linear_index < voxel_indexes.size ());
        vmap[voxel_indexes[linear_index]] = linear_index;
    }

    // Convert map to a vector of voxelized points
    T vpc (vmap.size ());
    size_t i = 0;
    for (auto v : vmap)
        vpc[i++] = pc[v.second];

    return vpc;
}

template<typename T>
T undecimate (const T &original_pc, const T &decimated_pc, const double resolution, const bool all_fields)
{
    // Get a voxel index for each point in the original point cloud
    auto original_indexes = spoc::voxel::get_voxel_indexes (original_pc, resolution);

    // Get a voxel index for each point in the decimated point cloud
    auto decimated_indexes = spoc::voxel::get_voxel_indexes (decimated_pc, original_pc, resolution);

    // Get a map from a voxel in the decimated pc to its index
    std::unordered_map<spoc::voxel::voxel_index,size_t,spoc::voxel::voxel_index_hash> vmap;
    for (size_t i = 0; i < decimated_pc.size (); ++i)
        vmap[decimated_indexes[i]] = i;

    const std::string error_string (
        "Could not match an original voxel to a decimated voxel. "
        "Did the decimated point cloud come from the original?"
        );

    if (!all_fields)
    {
        // The final point cloud will have all the same values as the
        // original except the classifications
        T pc (original_pc);

        // Stuff values into the output pc
        for (size_t i = 0; i < pc.size (); ++i)
        {
            // Check to make sure that each point in the original maps to a
            // voxel in the decimated pc
            if (vmap.find (original_indexes[i]) == vmap.end ())
                throw std::runtime_error (error_string);

            // Get the linear index into the decimated points
            const size_t decimated_index = vmap[original_indexes[i]];

            // Check logic
            assert (decimated_index < decimated_pc.size ());

            // Set the point's classification in the undecimated pc
            pc[i].data.classification = decimated_pc[decimated_index].data.classification;
        }

        return pc;
    }
    else
    {
        // The final point cloud will be the same size as the original
        T pc (original_pc.size ());

        // Stuff values into the output pc
        for (size_t i = 0; i < pc.size (); ++i)
        {
            // Check to make sure that each point in the original maps to a
            // voxel in the decimated pc
            if (vmap.find (original_indexes[i]) == vmap.end ())
                throw std::runtime_error (error_string);

            // Get the linear index into the decimated points
            const size_t decimated_index = vmap[original_indexes[i]];

            // Check logic
            assert (decimated_index < decimated_pc.size ());

            // Set the point's properties in the undecimated pc
            pc[i] = decimated_pc[decimated_index];

            // ... but retain xyz values from the original
            pc[i].x = original_pc[i].x;
            pc[i].y = original_pc[i].y;
            pc[i].z = original_pc[i].z;
        }

        return pc;
    }
}

} // namespace subsampling

} // namespace spoc
