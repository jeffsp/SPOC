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
#include "test_utils.h"

template<typename T>
void test_extent (const T &points)
{
    using namespace std;
    using namespace spc;
    const auto e = get_extent (points);

    const auto e_e = encode_extent (e);
    const auto e_d = decode_extent (e_e);
    verify (e_d == e);

    const auto x = rescale (points, e);
    const auto y = restore (x, e);
    std::vector<point<double>> d (points.size ());
    std::vector<point<double>> z (points.size ());

    for (size_t i = 0; i < points.size (); ++i)
        d[i] = points[i] - y[i];

    for (size_t i = 0; i < points.size (); ++i)
        z[i] = y[i] + d[i];

    verify (points == z);

    const auto z1 = compress (encode_points (x));
    const auto z2 = compress (encode_points (d));
    const auto z3 = compress (encode_points (points));
    clog << "ratios x/d/original"
        << '\t' << z1.size () * 1.0 / points.size ()
        << '\t' << z2.size () * 1.0 / points.size ()
        << '\t' << z3.size () * 1.0 / points.size ()
        << " bytes/pt" << endl;
}

void test (const size_t N,
    const int min_exponent = std::numeric_limits<double>::min_exponent / 2,
    const int max_exponent = std::numeric_limits<double>::max_exponent / 2)
{
    using namespace std;
    using namespace spc;

    clog << "Generating " << N << " random points" << endl;

    const auto points = generate_points (N, min_exponent, max_exponent);

    test_extent (points);
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
