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
#include "point.h"
#include "test_utils.h"

template<typename T>
void test_octants (const T &points, const size_t max_depth)
{
    using namespace std;
    using namespace spc;

    const auto e = get_extent (points);

    octree o (max_depth, points);

    print_octree_info (clog, o);

    const auto b = encode_extent (e);
    const auto x = encode_octants (o.get_root ());
    octree o2 (b, x);
    const auto y = encode_octants (o2.get_root ());
    verify (x == y);
}

/*
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
    const auto x = encode_points (pds);
    const auto y = decode_point_deltas (x);
    verify (pds == y);
}

template<typename T>
void test_octree (const T &points, const size_t max_depth)
{
    using namespace std;
    using namespace spc;

    octree o1 (max_depth, points);

    const auto x = encode (o1);

    // Decode the octree
    octree o2 (x);

    auto p1 = o1.get_points ();
    auto p2 = o2.get_points ();
    auto p3 = points;

    verify (p1.size () == p2.size ());
    verify (p1.size () == p3.size ());

    // Point order is not preserved
    std::sort (std::begin (p1), std::end (p1));
    std::sort (std::begin (p2), std::end (p2));
    std::sort (std::begin (p3), std::end (p3));
    clog << setprecision(std::numeric_limits<double>::digits10);
    const unsigned precision = 9;
    for (size_t i = 0; i < p1.size () && i < p2.size (); ++i)
    {
        if (!about_equal (p1[i], p2[i], precision))
        {
            clog << p1[i] << '\t' << p2[i] << endl;
        }
    }

    // The same points stored in two different trees will be equal
    verify (p1 == p2);

    // The original points will be slightly different than the ones
    // retrieved from the tree because of rounding error
    verify (spc::about_equal (p1, p3, precision));
}

template<typename T>
void test_compress_octree (const T &points)
{
    using namespace std;
    using namespace spc;

    vector<uint8_t> b;
    for (size_t max_depth = 1; max_depth < 16; ++max_depth)
    {
        octree o (max_depth, points);
        const auto x = encode (o);
        auto y = compress (x.extent_bytes);
        clog << "extent size " << y.size ();
        b.insert (b.end (), y.begin (), y.end ());
        y = compress (x.octant_bytes);
        clog << "\toctants size " << y.size ();
        b.insert (b.end (), y.begin (), y.end ());
        y = compress (x.point_count_bytes);
        clog << "\tpoint counts size " << y.size ();
        b.insert (b.end (), y.begin (), y.end ());
        y = compress (x.point_delta_bytes);
        clog << "\tpoint deltas size " << y.size () << endl;
        b.insert (b.end (), y.begin (), y.end ());
        clog << "max_depth " << max_depth
            << "\tbytes/point " << b.size () * 1.0 / points.size ()
            << endl;
    }
}
*/

void test (const size_t N,
    const int min_exponent = std::numeric_limits<double>::min_exponent / 2,
    const int max_exponent = std::numeric_limits<double>::max_exponent / 2)
{
    using namespace std;
    using namespace spc;

    clog << "Generating " << N << " random points" << endl;

    const auto points = generate_points (N, min_exponent, max_exponent);

    /*
    for (auto max_depth : {1, 2, 4, 8})
    {
        test_octants (points, max_depth);
        test_point_counts (points, max_depth);
        test_point_deltas (points, max_depth);
        test_octree (points, max_depth);
    }

    test_compress_octree (points);
    */
}

int main (int argc, char **argv)
{
    using namespace std;
    using namespace spc;

    try
    {
        test (3, 0, 0);
        test (10, 0, 0);
        test (10, 1, 10);
        test (10, -10, 10);
        test (10'000, 0, 0);
        test (10'000, 0, 10);
        test (10'000, -10, 10);
        test (10'000, 0, 20);
        test (10'000, -20, 20);
        test (10'000, -40, 0);
        test (100'000, 0, 0);
        test (100'000, 0, 5);
        test (100'000, -10, 0);
        test (100'000, 0, 10);
        test (100'000, 0, 15);
        test (100'000, 0, 20);
        test (100'000, -20, 20);
        test (100'000, 0, 30);
        test (100'000, 0, 40);
        test (100'000, 0, 50);
        test (100'000, -50, 0);
        test (1'000'000, 0, 12);

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
