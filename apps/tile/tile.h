#pragma once

#include "spoc/extent.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <vector>
#include <unordered_map>

namespace spoc
{

namespace tile_app
{

double get_tile_size (const spoc::extent::extent &e, const size_t tiles)
{
    // Get difference in X and Y extents
    //
    // Add a small amount to handle rounding
    const double dx = e.maxp.x - e.minp.x + std::numeric_limits<float>::epsilon ();
    const double dy = e.maxp.y - e.minp.y + std::numeric_limits<float>::epsilon ();

    // Determine dimension of one side of a tile
    const double tile_size = ((dx > dy) ? dx / tiles : dy / tiles);

    return tile_size;
}

template<typename T>
std::vector<size_t> get_tile_indexes (const T &p,
    const spoc::extent::extent &e,
    const double tile_size_x,
    const double tile_size_y)
{
    // Get difference in X extent
    //
    // Add a small amount to handle rounding
    const double dx = e.maxp.x - e.minp.x + std::numeric_limits<float>::epsilon ();

    // Compute the stride of the number of tiles
    const size_t stride = tile_size_x > 0.0 ? dx / tile_size_x + 1.0 : 1.0;

    // Allocate return value
    std::vector<size_t> indexes (p.size ());

    // For each x, y pair
    for (size_t i = 0; i < p.size (); ++i)
    {
        // Get the x and y offsets
        const double xo = p[i].x - e.minp.x;
        const double yo = p[i].y - e.minp.y;

        // Get the x and y indexes
        const size_t xi = tile_size_x > 0.0 ? xo / tile_size_x : 0.0;
        const size_t yi = tile_size_y > 0.0 ? yo / tile_size_y : 0.0;

        // Determine the tile index
        const size_t index = yi * stride + xi;

        // Set the index
        assert (i < indexes.size ());
        indexes[i] = index;
    }

    return indexes;
}

using tile_map = std::unordered_map<size_t, std::vector<size_t>>;

template<typename T>
tile_map get_tile_map (const T &indexes)
{
    // Return value
    tile_map m;

    // For each tile index, save the point's index in the tile index's vector
    for (size_t i = 0; i < indexes.size (); ++i)
        m[indexes[i]].push_back (i);

    return m;
}

} // namespace tile_app

} // namespace spoc
