#include "spoc/extent.h"
#include "spoc/point.h"
#include "spoc/test_utils.h"
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

using namespace std;
using namespace spoc::extent;
using namespace spoc::point;
using namespace spoc::test_utils;

template<typename T>
void test_extent (const T &points)
{
    const auto e = get_extent (points);

    const auto e_e = encode_extent (e);
    const auto e_d = decode_extent (e_e);
    VERIFY (e_d == e);

    const auto x = rescale (points, e);
    const auto y = restore (x, e);
    vector<point<double>> d (points.size ());
    vector<point<double>> z (points.size ());

    for (size_t i = 0; i < points.size (); ++i)
        d[i] = points[i] - y[i];

    for (size_t i = 0; i < points.size (); ++i)
        z[i] = y[i] + d[i];

    VERIFY (points == z);
}

void test (const size_t N,
    const int min_exponent = numeric_limits<double>::min_exponent / 2,
    const int max_exponent = numeric_limits<double>::max_exponent / 2)
{
    const auto points = generate_points (N, min_exponent, max_exponent);

    test_extent (points);
}

void test_area ()
{
    VERIFY (about_equal (get_area (spoc::extent::extent {{1.0, 2.0, 3.0}, {4.0, 6.0, 8.0}}), 3.0 * 4.0));
}

void test_volume ()
{
    VERIFY (about_equal (get_volume (spoc::extent::extent {{1.0, 2.0, 3.0}, {4.0, 6.0, 8.0}}), 3.0 * 4.0 * 5.0));
}


void test_total_extent ()
{
    spoc::extent::extent e1 = {{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}};
    spoc::extent::extent e2 = {{2.0, 3.0, 4.0}, {5.0, 6.0, 7.0}};
    spoc::extent::extent e_total = get_total_extent(e1, e2);
    VERIFY (about_equal(e_total.minp.x, e1.minp.x));
    VERIFY (about_equal(e_total.minp.y, e1.minp.y));
    VERIFY (about_equal(e_total.minp.z, e1.minp.z));
    VERIFY (about_equal(e_total.maxp.x, e2.maxp.x));
    VERIFY (about_equal(e_total.maxp.y, e2.maxp.y));
    VERIFY (about_equal(e_total.maxp.z, e2.maxp.z));
}

int main (int argc, char **argv)
{
    try
    {
        test (3, 0, 0);
        test (10, 0, 0);
        test (10, 1, 10);
        test (10, -10, 10);
        test (10'000, 0, 0);
        test (10'000, 0, 10);
        test (10'000, -10, 0);
        test (10'000, -10, 10);
        test (10'000, 0, 20);
        test (10'000, -20, 20);
        test (10'000, -40, 0);
        test (10'000, -50, 0);
        test (1'000, 0, 12);
        test_area ();
        test_volume ();
        test_total_extent ();

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
