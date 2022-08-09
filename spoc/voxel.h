#pragma once

#include "spoc/extent.h"
#include <unordered_map>
#include <unordered_set>
#include <numeric>

namespace spoc
{

namespace voxel
{

// A 3D index into a packed 3D volume
struct voxel_index
{
    voxel_index ()
        : i (0)
        , j (0)
        , k (0)
    {}

    voxel_index (size_t i, size_t j, size_t k)
        : i (i)
        , j (j)
        , k (k)
    {}

    size_t i;
    size_t j;
    size_t k;
    bool operator== (const voxel_index &v) const
    {
        return v.i == i && v.j == j && v.k == k;
    }
};

// Get a 3D index given a point in xyz space
template<typename T,typename U>
voxel_index get_voxel_index (const T &p, const U &minp, const double res)
{
    voxel_index v;
    v.i = (p.x - minp.x) / res;
    v.j = (p.y - minp.y) / res;
    v.k = (p.z - minp.z) / res;
    return v;
}

template<typename T>
std::vector<voxel_index> get_voxel_indexes (const T &points,
    const spoc::extent::extent &e,
    const double res)
{
    // Return value
    std::vector<voxel_index> voxel_indexes (points.size ());
#pragma omp parallel for
    for (size_t i = 0; i < voxel_indexes.size (); ++i)
        voxel_indexes[i] = get_voxel_index (points[i], e.minp, res);
    return voxel_indexes;
}

template<typename T, typename U>
std::vector<voxel_index> get_voxel_indexes (const T &points,
    const std::vector<U> &point_indexes,
    const double res)
{
    // get extent so we can get the minimum point
    const auto e = spoc::extent::get_extent (points);
    std::vector<voxel_index> voxel_indexes (point_indexes.size ());
#pragma omp parallel for
    for (size_t i = 0; i < voxel_indexes.size (); ++i)
        voxel_indexes[i] = get_voxel_index (points[point_indexes[i]], e.minp, res);
    return voxel_indexes;
}

template<typename T>
std::vector<voxel_index> get_voxel_indexes (const T &points, const double res)
{
    const auto e = spoc::extent::get_extent (points);
    return get_voxel_indexes (points, e, res);
}

// Compute a hash value from a voxel index.
struct voxel_index_hash
{
    std::size_t operator() (const voxel_index &v) const
    {
        size_t hash = (v.i << 0) ^ (v.j << 16) ^ (v.k << 32);
        return hash;
    }
};

// Typedefs for maps and sets of voxel indexes
using voxel_index_set = std::unordered_set<voxel_index, voxel_index_hash>;
using voxel_index_map = std::unordered_map<voxel_index, std::vector<size_t>, voxel_index_hash>;

template<typename T>
voxel_index_set get_voxel_index_set (const T &voxel_indexes)
{
    voxel_index_set vis;
    vis.insert (voxel_indexes.begin (), voxel_indexes.end ());
    return vis;
}

template<typename T>
voxel_index_map get_voxel_index_map (const T &voxel_indexes)
{
    voxel_index_map vim;
    for (size_t i = 0; i < voxel_indexes.size (); ++i)
    {
        voxel_index v = voxel_indexes[i];
        vim[v].push_back (i);
    }
    // Free unused memory
    for (auto &v : vim)
        v.second.shrink_to_fit ();
    return vim;
}

} // namespace voxel

} // namespace spoc
