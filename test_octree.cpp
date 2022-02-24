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
void test_octree (const T &points, const size_t max_depth)
{
    using namespace std;
    using namespace spc;

    const auto e = get_extent (points);

    octree o (max_depth, points);

    print_octree_info (clog, o);

    const auto x = encode_octree (o.get_root ());
    octree o2 (e, x);
    const auto y = encode_octree (o2.get_root ());
    verify (x == y);
}

template<typename T>
void test_point_counts (const T &points, const size_t max_depth)
{
    using namespace std;
    using namespace spc;

    octree o (max_depth, points);

    const auto pcs = get_point_counts (o.get_root ());
    const auto x = encode_point_counts (pcs);
    const auto y = decode_point_counts (x);
    verify (pcs == y);
}

template<typename T>
void test_point_deltas (const T &points, const size_t max_depth)
{
    using namespace std;
    using namespace spc;

    octree o (max_depth, points);

    const auto pds = get_point_deltas (o.get_root ());
    const auto x = encode_point_deltas (pds);
    const auto y = decode_point_deltas (x);
    verify (pds == y);
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

    for (auto max_depth : {1, 8, 16})
    {
        test_octree (points, max_depth);
        test_point_counts (points, max_depth);
        test_point_deltas (points, max_depth);
    }

    /*
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
        test (1'000'000, -10, 10);
        test (1'000'000, 0, 30);

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
