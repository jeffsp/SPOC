#pragma once

#include <cassert>
#include <iostream>
#include <limits>
#include <vector>
#include <unordered_map>

namespace spoc
{

namespace tile
{

template<typename T>
double get_tile_size (const T &x, const T &y, const size_t tiles)
{
    // Check args
    if (x.empty ())
        throw std::runtime_error ("No X values");
    if (y.empty ())
        throw std::runtime_error ("No Y values");

    // Get minimum and maximum X and Y values
    const double minx = *std::min_element (begin(x), end(x));
    const double maxx = *std::max_element (begin(x), end(x));
    const double miny = *std::min_element (begin(y), end(y));
    const double maxy = *std::max_element (begin(y), end(y));

    // Get difference in X and Y extents
    //
    // Add a small amount to handle rounding
    const double dx = maxx - minx + std::numeric_limits<float>::epsilon ();
    const double dy = maxy - miny + std::numeric_limits<float>::epsilon ();

    // Determine dimension of one side of a tile
    const double tile_size = ((dx > dy) ? dx / tiles : dy / tiles);

    return tile_size;
}

template<typename T>
std::vector<size_t> get_tile_indexes (const T &x, const T &y, const double tile_size)
{
    assert (!x.empty ());
    assert (x.size () == y.size ());

    // Get minimum X and Y values
    const double minx = *std::min_element (begin(x), end(x));
    const double miny = *std::min_element (begin(y), end(y));

    // Get maximum X value
    const double maxx = *std::max_element (begin(x), end(x));

    // Get difference in X extent
    //
    // Add a small amount to handle rounding
    const double dx = maxx - minx + std::numeric_limits<float>::epsilon ();

    // Compute the stride of the number of tiles
    const size_t stride = dx / tile_size + 1;

    // Allocate return value
    std::vector<size_t> indexes (x.size ());

    // For each x, y pair
    for (size_t i = 0; i < x.size (); ++i)
    {
        // Get the x and y offsets
        const double xo = x[i] - minx;
        const double yo = y[i] - miny;

        // Get the x and y indexes
        const size_t xi = xo / tile_size;
        const size_t yi = yo / tile_size;

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

} // namespace tile

} // namespace spoc
