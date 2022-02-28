#pragma once

#include <cassert>
#include <cmath>
#include <random>
#include <sstream>
#include <vector>
#include "point.h"

inline void Verify (const char *e, const char *file, const unsigned line)
{
    std::stringstream s;
    s << "verification failed in " << file << ", line " << line << ": " << e;
    throw std::runtime_error (s.str ());
}

#define verify(e) (void)((e) || (Verify (#e, __FILE__, __LINE__), 0))

namespace spc
{

inline bool about_equal (const double &a,
    const double &b,
    const unsigned precision)
{
    const auto d = a - b;
    if (std::round (d * std::pow (10, precision)) != 0)
        return false;
    return true;
}

inline bool about_equal (const point<double> &a,
    const point<double> &b,
    const unsigned precision)
{
    if (!about_equal (a.x, b.x, precision))
        return false;
    if (!about_equal (a.y, b.y, precision))
        return false;
    if (!about_equal (a.z, b.z, precision))
        return false;
    return true;
}

inline bool about_equal (const std::vector<point<double>> &a,
    const std::vector<point<double>> &b,
    unsigned precision = 12)
{
    assert (a.size () == b.size ());
    for (size_t i = 0; i < a.size (); ++i)
        if (!about_equal (a[i], b[i], precision))
            return false;
    return true;
}

inline std::vector<spc::point<double>> generate_points (
    const size_t N,
    const int min_exponent = std::numeric_limits<double>::min_exponent / 2,
    const int max_exponent = std::numeric_limits<double>::max_exponent / 2)
{
    using namespace std;
    using namespace spc;

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

    return points;
}

}
