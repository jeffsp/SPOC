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
#include "compression.h"
#include "extent.h"
#include "test_utils.h"

using namespace std;
using namespace spoc::compression;
using namespace spoc::extent;
using namespace spoc::point;

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

    const auto z1 = compress (encode_points (x));
    const auto z2 = compress (encode_points (d));
    const auto z3 = compress (encode_points (points));
}

void test (const size_t N,
    const int min_exponent = numeric_limits<double>::min_exponent / 2,
    const int max_exponent = numeric_limits<double>::max_exponent / 2)
{
    const auto points = generate_points (N, min_exponent, max_exponent);

    test_extent (points);
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

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
