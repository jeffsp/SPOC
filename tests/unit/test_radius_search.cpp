#include "spoc/radius_search.h"
#include "spoc/test_utils.h"
#include "spoc/voxel.h"
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace spoc::radius_search;
using namespace spoc::test_utils;
using namespace spoc::voxel;

// Dummy
struct point {
    double x;
    double y;
    double z;
};

/// @brief helper function, Get the variance of the values in a container
/// @tparam T Container type
/// @param x Container
/// @return Variance
template <typename T>
double variance (const T &x)
{
    size_t total = 0;
    double sum = 0.0;
    double sum2 = 0.0;
    for (size_t i = 0; i < x.size (); ++i)
    {
        ++total;
        sum += x[i];
        sum2 += x[i] * x[i];
    }

    // Degenerate case
    if (total == 0)
        return 0.0;

    // Variance = E[x^2] - E[x]^2
    const double mean = sum / total;
    const double var = sum2 / total - mean * mean;

    // E[x^2] >= E[x]^2
    assert (var >= 0.0);

    return var;
}


using PC = vector<point>;

const PC point_cube = {
    { 0,  0, 0},
    { 0,  1, 0},
    { 0, -1, 0},
    { 1,  0, 0},
    { 1,  1, 0},
    { 1, -1, 0},
    {-1,  0, 0},
    {-1,  1, 0},
    {-1, -1, 0},

    { 0,  0, 1},
    { 0,  1, 1},
    { 0, -1, 1},
    { 1,  0, 1},
    { 1,  1, 1},
    { 1, -1, 1},
    {-1,  0, 1},
    {-1,  1, 1},
    {-1, -1, 1},

    { 0,  0, -1},
    { 0,  1, -1},
    { 0, -1, -1},
    { 1,  0, -1},
    { 1,  1, -1},
    { 1, -1, -1},
    {-1,  0, -1},
    {-1,  1, -1},
    {-1, -1, -1},
    };

// empty test
void test1 ()
{
    PC points;
    auto neighbors = radius_search_get_neighbors(points, 1.0);
    VERIFY (neighbors.empty());
}

// test single point
void test2 ()
{
    PC points;
    points.push_back({0, 0, 0});
    auto neighbors = radius_search_get_neighbors(points, 1.0);
    VERIFY (neighbors.size() == points.size());
    VERIFY (neighbors.at(0).size() == 1);
}

// test radius=1.1
void test3 ()
{
    auto neighbors = radius_search_get_neighbors(point_cube, 1.1);
    VERIFY (neighbors.size() == point_cube.size());
    VERIFY (neighbors.at(0).size() == 7);
}

// test radius=2.0
void test4 ()
{
    auto neighbors = radius_search_get_neighbors(point_cube, 2.1);
    VERIFY (neighbors.size() == point_cube.size());
    VERIFY (neighbors.at(0).size() == 27);
}

// test radius=<everything>
void test5 ()
{
    size_t count = 0;
    auto op = [&](size_t i, const vector<size_t> &indexes)
    {
#pragma omp atomic
            count += indexes.size ();
    };
    radius_search_all(point_cube, 100.0, op);
    // Every possible pair should have been counted
    VERIFY (count == point_cube.size () * point_cube.size ());
}

// test for race conditions
void test6 ()
{
    // Generate a point cloud
    const double stddev = 10.0;
    const double radius = 5.0;
    PC pc = get_noisy_gaussian_point_cloud<PC> (M_PI * radius * radius * 100, radius, stddev);

    // You should get the same exact answer.
    //
    // If you don't, a race condition probably exists
    auto n1 = radius_search_get_neighbors(pc, 1.0);
    auto n2 = radius_search_get_neighbors(pc, 1.0);
    VERIFY (n1 == n2);

    // Check to make sure each the neighbor count is non-zero
    for (size_t i = 0; i < n1.size (); ++i)
        VERIFY (n1[i].size () > 0);
}

// test for race conditions
void test7 ()
{
    // Generate a point cloud
    const double stddev = 10.0;
    const double radius = 5.0;
    PC pc = get_noisy_gaussian_point_cloud<PC> (M_PI * radius * radius * 100, radius, stddev);

    auto n1 = radius_search_get_neighbors(pc, 0.5);
    auto n2 = radius_search_get_neighbors(pc, 1.0);
    auto n3 = radius_search_get_neighbors(pc, 1.5);

    // Check to make sure each the neighbor count is some reasonable number
    vector<double> sizes1 (n1.size ());
    vector<double> sizes2 (n2.size ());
    vector<double> sizes3 (n3.size ());
    for (size_t i = 0; i < n1.size (); ++i)
    {
        sizes1[i] = n1[i].size ();
        sizes2[i] = n2[i].size ();
        sizes3[i] = n3[i].size ();
    }

    const double sz1 = sqrt (variance (sizes1));
    const double sz2 = sqrt (variance (sizes2));
    const double sz3 = sqrt (variance (sizes3));

    VERIFY (sz1 * 2 < sz2);
    VERIFY (sz2 * 2 < sz3);
}

// Test different interfaces
void test8 ()
{
    // Generate a point cloud
    const double stddev = 10.0;
    const double radius = 5.0;
    PC pc = get_noisy_gaussian_point_cloud<PC> (M_PI * radius * radius * 100, radius, stddev);

    size_t callbacks = 0;
    size_t connections = 0;
    auto op = [&](size_t i, const vector<size_t> &indexes)
    {
#pragma omp atomic
            ++callbacks;
#pragma omp atomic
            connections += indexes.size ();
    };

    callbacks = 0;
    connections = 0;
    radius_search_all (pc, 1.0, op);
    VERIFY (callbacks == pc.size ());

    callbacks = 0;
    connections = 0;
    vector<size_t> ind {0, 1, 10, 5, 3, 1, 5};
    radius_search_indexes (pc, ind, 2.0, op);
    VERIFY (callbacks == ind.size ());

    // Make sure max_neighbors works
    callbacks = 0;
    connections = 0;
    size_t max_neighbors = 5;
    radius_search_all_neighbors (pc, ind, 4.0, op, max_neighbors);
    VERIFY (callbacks == ind.size ());
    VERIFY (connections == ind.size () * max_neighbors);
    callbacks = 0;
    connections = 0;
    radius_search_all_neighbors (pc, ind, 4.0, op, max_neighbors);
    VERIFY (callbacks == ind.size ());
    VERIFY (connections == ind.size () * max_neighbors);

    // Check degenerate case
    callbacks = 0;
    connections = 0;
    max_neighbors = 0;
    radius_search_all_neighbors (pc, ind, 4.0, op, max_neighbors);
    VERIFY (callbacks == ind.size ());
    VERIFY (connections == ind.size () * max_neighbors);
}

// Test very dense point cloud
void test9 ()
{
    // Generate a point cloud with about 10,000/27=370 points per m^3
    PC pc = get_noisy_point_cloud<PC> (10000, 3, 3, 3);

    size_t callbacks = 0;
    size_t connections = 0;
    auto op = [&](size_t i, const vector<size_t> &indexes)
    {
#pragma omp atomic
            ++callbacks;
#pragma omp atomic
            connections += indexes.size ();
    };

    vector<size_t> ind (10);
    iota (ind.begin (), ind.end (), 0);
    double radius = 1.0;
    size_t max_neighbors = 5;
    radius_search_all_neighbors (pc, ind, radius, op, max_neighbors);
}

// Test optimized radius search
void test10 ()
{
    PC pc;

    // Put a single point at 0,0,0
    pc.push_back({0, 0, 0});

    // Put a single point at 1,1,1
    pc.push_back({1, 1, 1});

    // Put 100 points at 1,1,3
    for (size_t i = 0; i < 100; ++i)
        pc.push_back({1, 1, 3});

    size_t callbacks = 0;
    size_t neighbors = 0;
    auto op = [&](size_t i, const vector<size_t> &indexes)
    {
#pragma omp atomic
            ++callbacks;
#pragma omp atomic
            neighbors += indexes.size ();
    };

    // Search at the point 1,1,1 with a 1.0m radius, max=10000 neighbors
    //
    // Only one point is within radius -- the point at 1,1,1
    callbacks = 0;
    neighbors = 0;
    radius_search_all_neighbors (pc, vector<size_t> (1, 1), 1.0, op, 10000);
    VERIFY (callbacks == 1);
    VERIFY (neighbors == 1);

    // Search at the point 1,1,1 with a <2m radius, max=10000 neighbors
    //
    // 2 points are now within the radius -- 0,0,0 is sqrt(3)=1.73m from 1,1,1
    callbacks = 0;
    neighbors = 0;
    radius_search_all_neighbors (pc, vector<size_t> (1, 1), 1.9, op, 10000);
    VERIFY (callbacks == 1);
    VERIFY (neighbors == 2);

    // Search at the point 1,1,1 with a >2m radius, max=10000 neighbors
    //
    // All points are within the radius, the last 100 are 2m from 1,1,1...
    // ... however, they are in a different voxel than the one at 1,1,1
    callbacks = 0;
    neighbors = 0;
    radius_search_all_neighbors (pc, vector<size_t> (1, 1), 2.1, op, 10000);
    VERIFY (callbacks == 1);
    VERIFY (neighbors == 102);

    // The voxel index distribution
    unordered_map<size_t,size_t> dist;

    auto op2 = [&](size_t i, const vector<size_t> &indexes)
    {
#pragma omp atomic
            ++callbacks;
#pragma omp atomic
            neighbors += indexes.size ();
#pragma omp critical
            {
                // Index 0 or 1 -> 0, higher indexes -> 1
                for (auto j : indexes)
                    ++dist[j < 2 ? 0 : 1];
            }
    };

    // Search at the point 1,1,1 with a >2m radius, max=10 neighbors
    //
    // The 10 it selects should mostly be in the voxel that contains 100.
    //
    // The chances that it chooses the ones at 0,0,0 or 1,1,1 is much lower.
    const double radius = 2.1;
    auto point_voxel_indexes = get_voxel_indexes (pc, radius);
    auto neighbor_voxel_indexes = get_voxel_indexes (pc, radius);
    auto neighbor_vim = get_voxel_index_map (neighbor_voxel_indexes);

    // The voxel index map should only have two voxels with entries: One
    // contains two points at 0,0,0 and 1,1,1 and the other contains 100
    // points at 1,1,3
    VERIFY ((neighbor_vim[{0,0,0}].size () == 2 && neighbor_vim[{0,0,1}].size () == 100)
            || (neighbor_vim[{0,0,1}].size () == 100 && neighbor_vim[{0,0,0}].size () == 2));

    // Get 10 neighbors.
    //
    // About 10% should come from the voxel at 0,0,0, and about 90% should
    // come from the voxel at 0,0,1.
    //
    // There is a chance that we will get 0 points from 0,0,0, so do it
    // several times. Also
    for (size_t i = 0; i < 10; ++i)
    {
        // The RNG is seeded by the number of points in the voxels, so we will
        // add point each time to ensure that we get a different random order
        // every time.
        //
        // Note that the order in which the points are selected from each
        // voxel will always be the same: reverse order that they are stored
        // in the vector of voxel point indexes. This is by design. The order
        // in which point indexes are stored in the point index vectors is
        // random because they are hashed from the voxel index. But in this
        // test, the voxel indexes don't change, so the pseudo-random order
        // never changes.
        pc.push_back({1, 1, 3});
        radius_search_all_neighbors (pc, vector<size_t> (1, 1), radius, op2, 10);
    }

    // Should be about a 1:10 ratio of points in the distribution
    VERIFY (dist[0] < 30);
    VERIFY (dist[1] > 70);
}

int main ()
{
    try
    {
        test1 ();
        test2 ();
        test3 ();
        test4 ();
        test5 ();
        test6 ();
        test7 ();
        test8 ();
        test9 ();
        test10 ();

        return 0;
    }
    catch (const exception &e)
    {
        cerr << "exception: " << e.what () << endl;
    }
    return -1;
}
