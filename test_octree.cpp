#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <random>
#include <string>
#include <vector>
#include "compress.h"
#include "extent.h"
#include "octree.h"
#include "test_utils.h"


namespace spc
{

/*
std::vector<uint8_t> encode_point_counts (const std::unique_ptr<octree_node> &root)
{
    std::vector<uint64_t> point_counts;
    const auto node_function = [&] (const std::unique_ptr<octree_node> &node) { };
    const auto leaf_function = [&] (const std::unique_ptr<octree_node> &node)
    {
        // Save the point location
        point_counts.push_back (node->deltas->size ());
    };
    spc::dfs (root, node_function, leaf_function);
    // Output bytes
    std::vector<uint8_t> x;
    // Encode the point counts (NOT PORTABLE)
    const uint8_t *pc = reinterpret_cast<const uint8_t *> (&point_counts[0]);
    x.insert (x.end (), pc, pc + point_counts.size () * sizeof(double));
    return x;
}

std::vector<uint8_t> encode_point_deltas (const std::unique_ptr<octree_node> &root)
{
    std::vector<point<double>> deltas;
    const auto node_function = [&] (const std::unique_ptr<octree_node> &node) { };
    const auto leaf_function = [&] (const std::unique_ptr<octree_node> &node)
    {
        // Save the deltas
        deltas.insert (deltas.end (),
            node->deltas->begin (),
            node->deltas->end ());
    };
    spc::dfs (root, node_function, leaf_function);
    // Output bytes
    std::vector<uint8_t> x;
    // Save the point deltas (NOT PORTABLE)
    const uint8_t *pd = reinterpret_cast<const uint8_t *> (&deltas[0]);
    x.insert (x.end (), pd, pd + deltas.size () * 3 * sizeof(double));
    return x;
}

std::vector<uint8_t> encode (std::vector<point<double>> &points,
        const size_t max_depth = 10)
{
    octree o (max_depth, points);
    const auto x1 = encode_octree (o.get_root ());
    const auto x2 = encode_point_counts (o.get_root ());
    const auto x3 = encode_point_deltas (o.get_root ());

    // Output
    std::vector<uint8_t> x;
    x.insert (x.end (), x1.begin (), x1.end ());
    x.insert (x.end (), x2.begin (), x2.end ());
    x.insert (x.end (), x3.begin (), x3.end ());
    return x;
}

std::vector<point<double>> decode (const std::vector<uint8_t> &x)
{
    std::vector<point<double>> y;
    return y;
}
*/

} // namespace spc

/*

*/

template<typename T>
void test_extent (const T &points)
{
    using namespace std;
    using namespace spc;
    const auto e = get_extent (points);
    const auto x = encode_extent (e);
    const auto y = decode_extent (x);
    verify (y == e);
}

template<typename T>
void test_octree (const T &points)
{
    using namespace std;
    using namespace spc;

    const auto e = get_extent (points);

    const size_t max_depth = 10;
    octree o (max_depth, points);

    print_octree_info (clog, o);

    const auto x = encode_octree (o.get_root ());
    octree o2 (x, e);
    const auto y = encode_octree (o2.get_root ());
    verify (x == y);
}

void test (const size_t N,
    const int min_exponent = std::numeric_limits<double>::min_exponent / 2,
    const int max_exponent = std::numeric_limits<double>::max_exponent / 2)
{
    using namespace std;
    using namespace spc;

    clog << "Generating " << N << " random points" << endl;

    vector<point<double>> points (N);

    // Generate random doubles at varying scales.
    //
    // We don't want a uniform distribution.
    std::default_random_engine g;
    std::uniform_real_distribution<double> d (-1.0, 1.0);
    std::uniform_int_distribution<int> exp (min_exponent, max_exponent);

    for (auto &p : points)
    {
        const double x = std::ldexp (d (g), exp (g));
        const double y = std::ldexp (d (g), exp (g));
        const double z = std::ldexp (d (g), exp (g));
        p = point<double> {x, y, z};
    }

    test_extent (points);
    test_octree (points);

    /*
    {
    const auto x = encode_point_counts (o.get_root ());
    const auto y = spc::compress (x);
    const auto z = spc::decompress (y);
    verify (x == z);
    }
    return;

    {
    const auto x = encode_point_deltas (o.get_root ());
    const auto y = spc::compress (x);
    const auto z = spc::decompress (y);
    verify (x == z);
    }

    {
        // Copy the points
        auto x (points);
        // Encode them
        auto y = encode (x);
        // Get decoded points
        auto z = decode (y);
        // Sort them because order is not preserved in encoded bytes
        sort (begin (x), end (x));
        sort (begin (z), end (z));
        // Check results
        verify (x == z);
    }
    */
}

int main (int argc, char **argv)
{
    using namespace std;
    using namespace spc;

    try
    {
        test (10, 0, 0);
        test (10, 1, 10);
        test (10, -10, 10);
        test (10'000, 0, 0);
        test (10'000, 0, 10);
        test (10'000, -10, 10);
        test (10'000, 0, 20);
        test (10'000, -20, 20);
        test (10'000);
        test (1'000'000);
        test (1'000'000, 0, 0);
        test (1'000'000, -10, 10);
        test (10'000'000);
        test (11'000'000, 0, 20);

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
