#pragma once

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
    std::vector<size_t> indexes;
    return indexes;
}

using tile_map = std::unordered_map<size_t, std::vector<size_t>>;

template<typename T>
tile_map get_tile_map (const T &indexes)
{
    tile_map m;
    return m;
}

} // namespace tile

} // namespace spoc
