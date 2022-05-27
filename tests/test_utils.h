#pragma once

#include "point.h"
#include "spoc.h"
#include <cassert>
#include <cmath>
#include <random>
#include <sstream>
#include <tuple>
#include <vector>

inline void Verify (const char *e, const char *file, const unsigned line)
{
    std::stringstream s;
    s << "verification failed in " << file << ", line " << line << ": " << e;
    throw std::runtime_error (s.str ());
}

#define verify(e) (void)((e) || (Verify (#e, __FILE__, __LINE__), 0))

inline std::vector<spoc::point<double>> generate_points (
    const size_t N,
    const int min_exponent = std::numeric_limits<double>::min_exponent / 2,
    const int max_exponent = std::numeric_limits<double>::max_exponent / 2)
{
    using namespace std;
    using namespace spoc;

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

std::vector<spoc::point_record> generate_random_point_records (
    const size_t total_points,
    const size_t extra_size = 0,
    const bool rgb = true)
{
    std::default_random_engine g;
    std::uniform_int_distribution<int> di (0, 1 << 15);
    std::uniform_real_distribution<double> dr (-1.0, 1.0);

    std::vector<spoc::point_record> p (total_points, spoc::point_record (extra_size));

    for (auto &i : p)
    {
        i.x = dr (g); i.y = dr (g); i.z = dr (g);
        i.c = di (g); i.p = di (g); i.i = di (g);
        if (rgb)
        {
            i.r = di (g);
            i.g = di (g);
            i.b = di (g);
        }
        for (auto &j : i.extra)
            j = di (g);
    }

    return p;
}

spoc::spoc_file generate_random_spoc_file (
    const size_t total_points,
    const size_t extra_size = 0,
    const bool rgb = true)
{
    spoc::spoc_file f;
    f.h.wkt = "WKT";
    f.h.extra_size = extra_size;
    f.h.total_points = total_points;
    f.p = generate_random_point_records (total_points, extra_size, true);
    return f;
}

/// @brief Ensure that two values are equal up to 'precision' decimal places
bool about_equal (double a, double b, unsigned precision = 3)
{
    int c = static_cast<int> (std::round (a * std::pow (10.0, precision)));
    int d = static_cast<int> (std::round (b * std::pow (10.0, precision)));
    return c == d;
}

