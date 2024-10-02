#pragma once

#include <vector>

#include "spoc/point_record.h"
#include "spoc/point.h"
#include "spoc/voxel.h"

namespace spoc
{

namespace radius_search
{

/// @brief Get the index of a random neighbor within the 3X3X3 voxels surrounding a point.
class random_neighbor_selector
{
    private:
    // The map into the vectors of neighbor indexes
    const spoc::voxel::voxel_index_map &neighbor_vim;

    // Get the voxel index for each of the 27 voxels
    const size_t total_voxels = 27;
    std::vector<spoc::voxel::voxel_index> voxel_indexes;

    // Keep track of how many points are in each voxel
    std::vector<size_t> voxel_counts;

    // Keep track of how many points are in each of the 27 voxels
    size_t total_points_left = 0;

    // Random number generator
    std::minstd_rand rng;

    public:
    random_neighbor_selector (const spoc::voxel::voxel_index &ijk, const spoc::voxel::voxel_index_map &neighbor_vim)
        : neighbor_vim (neighbor_vim)
        , voxel_indexes (total_voxels)
        , voxel_counts (total_voxels)
    {
        // Get the voxel indexes for the 27 voxels
        const int i1 = static_cast<int> (ijk.i) - 1;
        const int i2 = static_cast<int> (ijk.i) + 2;
        const int j1 = static_cast<int> (ijk.j) - 1;
        const int j2 = static_cast<int> (ijk.j) + 2;
        const int k1 = static_cast<int> (ijk.k) - 1;
        const int k2 = static_cast<int> (ijk.k) + 2;
        size_t n = 0;
        for (int i = i1; i < i2; ++i)
            for (int j = j1; j < j2; ++j)
                for (int k = k1; k < k2; ++k)
                    voxel_indexes[n++] = spoc::voxel::voxel_index (i, j, k);

        // There should have been 27
        assert (n == total_voxels);

        // Count the number of points in all voxels
        for (auto i : voxel_indexes)
        {
            // Is this voxel occupied?
            const auto entry = neighbor_vim.find (i);
            // Yes, add to the total
            if (entry != neighbor_vim.end ())
                total_points_left += entry->second.size ();
        }

        // Count the number of points in each voxel
        n = 0;
        for (auto i : voxel_indexes)
        {
            // Is this voxel occupied?
            const auto entry = neighbor_vim.find (i);
            // Yes, save its count
            if (entry != neighbor_vim.end ())
                voxel_counts[n] = entry->second.size ();
            ++n;
        }

        // There should have been 27
        assert (n == total_voxels);

        // Seed our rng with something changing and deterministic
        rng.seed (total_points_left);
    }

    // How many neighbors are left to examine?
    size_t get_total_points_left () const
    {
        return total_points_left;
    }

    // Don't call this unless there are still points left
    size_t operator () ()
    {
        assert (total_points_left != 0);

        // Use the cumulative distribution trick to get our random point
        //
        // Get a random index into the 'total_points_left' points left to examine
        const size_t random_index = rng () % total_points_left;

        // Keep track of the cumulative count and just index into the distribution
        //
        // This trick allows you to take any distribution and sample from it
        // using a uniform random number generator
        size_t chosen_voxel = 0;
        size_t cumulative_count = voxel_counts[chosen_voxel];

        // Skip voxels that have no points
        while (voxel_counts[chosen_voxel] == 0 || cumulative_count < random_index)
        {
            ++chosen_voxel;
            assert (chosen_voxel < voxel_counts.size ());
            cumulative_count += voxel_counts[chosen_voxel];
        }

        // We have now selected the voxel that we want in 'chosen_voxel'.
        //
        // You might be wondering why we didn't just randomly select one of
        // the 27 voxels. The reason you can't do that is because that would
        // not be uniform across all the points in the voxels. For example, if
        // voxel 0 had 1 point and voxel 2 had 1,000,000 points, and you
        // wanted to select 10 points, you would almost never select the point
        // from voxel 0 if you were selecting uniformly, but if you randomly
        // selected one of the 27 voxels, you would choose voxel 0 often.
        //
        // The points within each voxel index are in unordered containers,
        // i.e. they are in a random order, so we can just pull them out in
        // order.  We will pull the last one out first. That way, we can use
        // the voxel point counter to keep track of which ones have already
        // been pulled out.
        //
        // Get the vector of indexes in the chosen voxel.
        const auto entry = neighbor_vim.find (voxel_indexes[chosen_voxel]);

        // It has to be in the map, and it can't be empty.
        assert (entry != neighbor_vim.end ());
        assert (entry->second.size () != 0);

        // How many are left to pull in this voxel?
        const size_t count = voxel_counts[chosen_voxel];
        assert (count > 0);

        // Pull the index from the vector
        const size_t neighbor_index = entry->second[count - 1];

        // Decrease number of points that have been pulled from this voxel
        --voxel_counts[chosen_voxel];

        // Decrease number of points that have been pulled from all 27 voxels
        --total_points_left;

        return neighbor_index;
    }
};

/// @brief Get neighbors using fast algorithm and arbitrary distance function
template<typename T,typename U,typename V,typename W,typename X, typename Y>
std::vector<size_t> get_neighbors (
    const size_t i,
    const T &points,
    const U &point_indexes,
    const V &neighbor_indexes,
    const W &point_voxel_indexes,
    const X &neighbor_vim,
    const Y distance_function,
    const double radius,
    const size_t max_neighbors)
{
    // Return value
    std::vector<size_t> neighbors;

    // Check the easy case
    if (max_neighbors == 0)
        return neighbors;

    // Get the voxel index of point 'i'.
    const auto &ijk = point_voxel_indexes[i];

    // This object helps us select the neighbor index
    random_neighbor_selector selector (ijk, neighbor_vim);

    // While we are not done examining this point's neighbors
    while (selector.get_total_points_left () > 0)
    {
        // Have we gotten as many as we need?
        if (neighbors.size () == max_neighbors)
            break;

        // Get the index of a neighbor that is uniformly selected at random
        // from all of the points in the 3x3x3 surrounding voxels.
        const size_t j = selector ();

        // 'i' is the index into 'point_indexes'
        // 'j' is the index into 'neighbor_indexes'
        assert (i < point_indexes.size ());
        assert (j < neighbor_indexes.size ());
        assert (point_indexes[i] < points.size ());
        assert (neighbor_indexes[j] < points.size ());

        // Is it close enough?
        const double distance = distance_function(points[point_indexes[i]], points[neighbor_indexes[j]]);
        if (distance <= radius)
            neighbors.push_back (j);
    }

    // Deallocate memory
    neighbors.shrink_to_fit ();

    return neighbors;
}

/// @brief Get neighbors using fast algorithm and using 3d distance
template<typename T,typename U,typename V,typename W,typename X>
std::vector<size_t> get_neighbors_3d (
    const size_t i,
    const T &points,
    const U &point_indexes,
    const V &neighbor_indexes,
    const W &point_voxel_indexes,
    const X &neighbor_vim,
    const double radius,
    const size_t max_neighbors)
{
    using point_t = typename T::value_type;
    std::function<double(const point_t &p1,
                         const point_t &p2)> distance_function
        = [](const point_t &p1,
             const point_t &p2)
        {
            return spoc::point::distance(p1, p2);
        };
    return get_neighbors(
        i,
        points,
        point_indexes,
        neighbor_indexes,
        point_voxel_indexes,
        neighbor_vim,
        distance_function,
        radius,
        max_neighbors
    );
}

/// @brief Get neighbors using fast algorithm and using 2d distance.
/// This version of get_neighbors could be used by zeroing all z vlaues in a point
/// cloud before voxelizing. Then z values can be restored and this function
/// can be used to search over all points in a cynlinder around the center point.
template<typename T,typename U,typename V,typename W,typename X>
std::vector<size_t> get_neighbors_2d (
    const size_t i,
    const T &points,
    const U &point_indexes,
    const V &neighbor_indexes,
    const W &point_voxel_indexes,
    const X &neighbor_vim,
    const double radius,
    const size_t max_neighbors)
{
    using point_t = typename T::value_type;
    std::function<double(const point_t &p1,
                         const point_t &p2)> distance_function
        = [](const point_t &p1,
             const point_t &p2)
        {
            return spoc::point::distance_2d(p1, p2);
        };
    return get_neighbors(
        i,
        points,
        point_indexes,
        neighbor_indexes,
        point_voxel_indexes,
        neighbor_vim,
        distance_function,
        radius,
        max_neighbors
    );
}

/// @brief Get neighbors within a specified radius
/// @tparam T Point cloud type
/// @tparam U Point cloud indexes type
/// @tparam V Neighbor operation type
/// @param points Point cloud
/// @param point_indexes Indexes of points to search
/// @param neighbor_indexes Indexes of neighbors to search
/// @param radius Radius to search in meters
/// @param neighbor_op Operation to perform on point and neighbors
/// @param max_neighbors Maximum allowable number of neighbors
///
/// Points in the 'point_indexes' parameter will be used
/// as the starting points for each neighbor search.
///
/// Only indexes in 'neighbor_indexes' are valid neighbors.
///
/// To search the whole point cloud, set 'point_indexes' and 'neighbor_indexes'
/// both equal to (0, 1, 2, ..., N-1). Or, simply call the helper function
/// below.
///
/// To search only a subset of the point cloud, list the subset indexes in
/// 'point_indexes' and set 'neighbor_indexes' equal to 'point_indexes'. Or,
/// again, just call the helper function below.
///
/// To find neighbors for a subset of points, but to allow the entire point
/// cloud to be used as neighbors, set 'point_indexes' to the subset
/// indexes, and set 'neighbor_indexes' to (0, 1, 2, ..., N - 1).
///
/// NOTE: The 'op' operations may be called in parallel,
/// but each value of 'i' is guaranteed to be unique.
///
/// Make sure you guard the call with
///
///     #pragma omp critical
///
/// or
///     #pragma omp atomic
///
/// if needed.
template<typename T,typename U,typename V>
void radius_search (
    const T &points,
    const U &point_indexes,
    const U &neighbor_indexes,
    const double radius,
    V neighbor_op,
    const size_t max_neighbors = 32)
{
    // Get an i,j,k for each point index
    const auto point_voxel_indexes = spoc::voxel::get_voxel_indexes (points, point_indexes, radius);

    // Get an i,j,k for each neighbor index
    const auto neighbor_voxel_indexes = spoc::voxel::get_voxel_indexes (points, neighbor_indexes, radius);

    // Map each unique i,j,k to a vector of indexes back into 'points'
    const auto neighbor_vim = spoc::voxel::get_voxel_index_map (neighbor_voxel_indexes);

    // For each search starting point
#pragma omp parallel for
    for (size_t i = 0; i < point_voxel_indexes.size(); ++i)
    {
        // The neighbor indexes
        std::vector<size_t> neighbors;

        neighbors = get_neighbors_3d (i,
                points,
                point_indexes,
                neighbor_indexes,
                point_voxel_indexes,
                neighbor_vim,
                radius,
                max_neighbors);

        // Do the neighbor operation
        //
        // Be sure to make this operation thread-safe. See note above.
        //
        // Also note that 'i' is the index into 'point_indexes', not an index
        // into 'points'.
        neighbor_op (i, neighbors);
    }
}

/// @brief Perform 'op' on neighbors within a specified radius, only looking
/// at points specified by indexes
/// @tparam T Point cloud type
/// @tparam U Point cloud indexes type
/// @tparam V Neighbor operation type
/// @param points Point cloud
/// @param indexes Indexes into the point cloud to look at
/// @param radius Radius to search in meters
/// @param neighbor_op Operation to perform on point and neighbors
/// @param max_neighbors Maximum allowable number of neighbors
///
/// Points that are not referenced in the 'indexes' parameter are ignored.
/// That is, they will not be searched for neighbors, and they will never be
/// listed as neighbors. They are effectively removed from the point cloud.
template<typename T,typename U,typename V>
void radius_search_indexes (const T &points, const U &indexes, const double radius, V neighbor_op, const size_t max_neighbors = 32)
{
    radius_search (points, indexes, indexes, radius, neighbor_op, max_neighbors);
}

/// @brief Perform 'op' on neighbors within a specified radius. Search all available
/// points for neighbors, but only search for neighbors for the points specified by
/// 'indexes'
/// @tparam T Point cloud type
/// @tparam U Point cloud indexes type
/// @tparam V Neighbor operation type
/// @param points Point cloud
/// @param indexes Indexes for points in the point cloud to search for neighbors around
/// @param radius Radius to search in meters
/// @param neighbor_op Operation to perform on point and neighbors
/// @param max_neighbors Maximum allowable number of neighbors
///
/// Perform the 'op' at each point in 'indexes', but look at the entire
/// point cloud for neighbors, not just the points in 'indexes'.
template<typename T,typename U,typename V>
void radius_search_all_neighbors (const T &points,
    const U &indexes,
    const double radius,
    V neighbor_op,
    const size_t max_neighbors)
{
    std::vector<size_t> neighbor_indexes (points.size ());
    std::iota (neighbor_indexes.begin (), neighbor_indexes.end (), 0);
    radius_search (points, indexes, neighbor_indexes, radius, neighbor_op, max_neighbors);
}

/// @brief Perform 'op' on neighbors within a specified radius for all points in the cloud
/// @tparam T Point cloud type
/// @tparam U Operation type
/// @param points Point cloud
/// @param radius Radius to search in meters
/// @param op Operation to perform on search pair
///
/// See note above about thread safety and the 'op' parameter.
template<typename T,typename U>
void radius_search_all (const T &points, const double radius, U op,
                        const size_t max_neighbors = 32)
{
    // Radius search the entire point cloud
    std::vector<size_t> indexes (points.size ());
    std::iota (indexes.begin (), indexes.end (), 0);
    radius_search (points, indexes, indexes, radius, op, max_neighbors);
}

/// @brief Get neighbor indexes within a specified radius
/// @tparam T Point cloud type
/// @param points Point cloud
/// @param radius Radius to search in meters
/// @return list of indices per point
template<typename T>
std::vector<std::vector<size_t>> radius_search_get_neighbors(const T& points, const double radius)
{
    // Return value
    std::vector<std::vector<size_t>> neighbors (points.size());

    auto op = [&](size_t i, const std::vector<size_t> &indexes)
    {
        assert (i < neighbors.size ());
        neighbors[i] = indexes;
    };

    radius_search_all (points, radius, op);

    return neighbors;
}

} // namespace radius_search

} // namespace spoc
