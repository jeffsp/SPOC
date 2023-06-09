#pragma once

#include "spoc/point.h"
#include "spoc/spoc.h"
#include <cassert>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <random>
#include <sstream>
#include <tuple>
#include <vector>

namespace spoc
{

namespace detail
{

inline void VerificationFailed (const char *e, const char *file, const unsigned line)
{
    std::stringstream s;
    s << "Verification failed in " << file << ", line " << line << ": " << e;
    throw std::runtime_error (s.str ());
}

void test_verification (const bool flag,
    const char *e,
    const char *file,
    const unsigned line)
{
    if (!flag)
        detail::VerificationFailed (e, file, line);
}

} // namespace detail

} // namespace spoc

#define VERIFY(e) spoc::detail::test_verification (e, #e, __FILE__, __LINE__);

#define VERIFY_THROWS(e) {\
        bool failed = false;\
        try\
        {\
            e\
        }\
        catch (...)\
        {\
            failed = true;\
        }\
        VERIFY (failed);\
    }

namespace spoc
{

namespace test_utils
{

inline std::vector<spoc::point::point<double>> generate_points (
    const size_t N,
    const int min_exponent = std::numeric_limits<double>::min_exponent / 2,
    const int max_exponent = std::numeric_limits<double>::max_exponent / 2)
{
    std::vector<spoc::point::point<double>> points (N);

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
        p = spoc::point::point<double> {x, y, z};
    }

    return points;
}

template<typename T>
T get_noisy_point_cloud (const size_t n_points, const double x_extent, const double y_extent, const double z_extent, const int seed = 123)
{
    // Fill it with noise
    std::default_random_engine g;
    std::uniform_real_distribution<double> u;

    // Set the seed
    g.seed (seed);

    T pc (n_points);

    // Generate the cloud
    for (auto &i : pc)
    {
        // Random coords, uniform X,Y,Z
        i.x = u (g) * x_extent;
        i.y = u (g) * y_extent;
        i.z = u (g) * z_extent;
    }

    return pc;
}


template<typename T>
T get_noisy_gaussian_point_cloud (const size_t n_points, const double radius, const double z_stddev, const int seed = 123)
{
    // Fill it with noise
    std::default_random_engine g;
    std::uniform_real_distribution<double> u;
    std::normal_distribution<double> d;

    // Set the seed
    g.seed (seed);

    T pc (n_points);

    // Generate the cloud
    for (auto &i : pc)
    {
        // Use Box-Muller to distribute uniformly over a disk of radius 'r' and with a normal marginal distribution
        // with stddev 'z_stddev' along the z-axis.
        //
        // (Hint: Generating random polar coords does not distribute uniformly.)
        constexpr double pi2 = 2.0 * M_PI;
        const double x = u (g);
        const double y = u (g);
        i.x = radius * sqrt (-2.0 * log (x)) * cos (pi2 * y);
        i.y = radius * sqrt (-2.0 * log (x)) * sin (pi2 * y);
        i.z = z_stddev * d (g);
    }

    return pc;
}

inline std::vector<spoc::point_record::point_record> generate_random_point_records (
    const size_t total_points,
    const size_t extra_fields = 0,
    const bool rgb = true,
    const size_t seed = 123)
{
    std::default_random_engine g (seed);
    std::uniform_int_distribution<int> di (0, 1 << 15);
    std::uniform_real_distribution<double> dr (-1.0, 1.0);

    std::vector<spoc::point_record::point_record> p (total_points, spoc::point_record::point_record (extra_fields));

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

inline spoc::file::spoc_file generate_random_spoc_file (
    const size_t total_points,
    const size_t extra_fields = 0,
    const bool compressed = false,
    const bool rgb = true,
    const size_t seed = 123)
{
    const auto prs = generate_random_point_records (total_points, extra_fields, rgb, seed);
    spoc::file::spoc_file f ("WKT", compressed, prs);
    return f;
}

/// @brief Ensure that two values are equal up to 'precision' decimal places
inline bool about_equal (double a, double b, unsigned precision = 3)
{
    int c = static_cast<int> (std::round (a * std::pow (10.0, precision)));
    int d = static_cast<int> (std::round (b * std::pow (10.0, precision)));
    return c == d;
}

/// Generate a unique filename without using std::tmpnam()
inline std::string generate_tmp_filename (const size_t total_chars = 16)
{
    using namespace std;

    // In test code, this is one of the few times you want to produce a
    // non-deterministic random number. In most cases, you will instead
    // want your pseudo-random numbers to be deterministic. For example,
    // you will use the default random generator and seed it with value.
    // The two most important reasons for this are:
    //
    //     1. If your test fails, you might not be able to reproduce the
    //        failure if the test changes on every run.
    //     2. It is common practice when developing multi-threaded code
    //        to check for race conditions by running the same test
    //        twice and making sure the output is the same on both runs.
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine e (seed);
    uniform_int_distribution<int> d (0, 25);

    string fn;

    for (size_t i = 0; i < total_chars; ++i)
        fn += ('a' + d (e));

    using namespace std::filesystem;
    const path p = temp_directory_path () / fn;

    return p.string ();
}

} // namespace test_utils

} // namespace spoc
