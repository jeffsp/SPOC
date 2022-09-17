#include "transform.h"
#include "spoc/spoc.h"
#include "spoc/test_utils.h"
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace spoc::file;
using namespace spoc::io;
using namespace spoc::point;
using namespace spoc::point_record;
using namespace spoc::test_utils;
using namespace spoc::transform_app;

void test_transform_detail ()
{
    // Generate spoc file
    auto f = generate_random_spoc_file (10, 3, true);
    stringstream is, os;
    write_spoc_file_compressed (is, f);
    VERIFY_THROWS (const auto h = detail::read_header_uncompressed (is); )
}

void test_transform_add ()
{
    // Generate random reference
    const auto p = generate_random_point_records (100);

    default_random_engine g;
    uniform_real_distribution<double> d (-100, 100);

    for (auto i : { 0, 1, 2, 3, 4})
    {
        (void)i; // disable not used warning
        const double offset = d (g);

        // Copy it
        auto qx (p);
        auto qy (p);
        auto qz (p);

        // Test streaming functions
        stringstream is, os;
        const string wkt ("Test WKT");
        write_spoc_file_uncompressed (is, spoc_file (wkt, false, p));
        write_spoc_file_uncompressed (is, spoc_file (wkt, false, p));
        write_spoc_file_uncompressed (is, spoc_file (wkt, false, p));

        add_x (is, os, offset);
        auto r = read_spoc_file_uncompressed (os);
        qx = r.get_point_records ();

        add_y (is, os, offset);
        r = read_spoc_file_uncompressed (os);
        qy = r.get_point_records ();

        add_z (is, os, offset);
        r = read_spoc_file_uncompressed (os);
        qz = r.get_point_records ();

        // VERIFY that the correct axis was used
        VERIFY (about_equal (p[3].x + offset, qx[3].x));
        VERIFY (p[3].y == qx[3].y);
        VERIFY (p[3].z == qx[3].z);
        VERIFY (p[3].x == qy[3].x);
        VERIFY (about_equal (p[3].y + offset, qy[3].y));
        VERIFY (p[3].z == qy[3].z);
        VERIFY (p[3].x == qz[3].x);
        VERIFY (p[3].y == qz[3].y);
        VERIFY (about_equal (p[3].z + offset, qz[3].z));
    }
}

void test_transform_copy_field ()
{
    // Generate spoc file
    const size_t total_points = 100;
    const size_t extra_fields = 8;
    const bool compressed = false;
    const bool rgb = true;
    auto f = generate_random_spoc_file (total_points, extra_fields, compressed, rgb);

    // Copy c to p
    {
        stringstream is, os;
        write_spoc_file_uncompressed (is, f);
        copy_field (is, os, "c", "p");
        const auto g = read_spoc_file_uncompressed (os);
        const auto c = get_c (f.get_point_records ());
        const auto p = get_p (g.get_point_records ());
        VERIFY (c == p);
    }

    // Copy e7 to e0
    {
        stringstream is, os;
        write_spoc_file_uncompressed (is, f);
        copy_field (is, os, "e7", "e0");
        const auto g = read_spoc_file_uncompressed (os);
        const auto e7 = get_extra (7, f.get_point_records ());
        const auto e0 = get_extra (0, g.get_point_records ());
        VERIFY (e7 == e0);
    }

    // Copy i to e2
    {
        stringstream is, os;
        write_spoc_file_uncompressed (is, f);
        copy_field (is, os, "i", "e2");
        const auto g = read_spoc_file_uncompressed (os);
        const auto i = get_i (f.get_point_records ());
        const auto e2 = get_extra (2, g.get_point_records ());
        // Convert to uint64_t
        const vector<uint64_t> i64 (begin (i), end (i));
        VERIFY (i64 == e2);
    }

    for (auto fn1 : { "c", "p", "i", "r", "g", "b",
            "e0", "e1", "e2", "e3", "e4", "e5", "e6", "e7" })
    {
        for (auto fn2 : { "c", "p", "i", "r", "g", "b",
                "e0", "e1", "e2", "e3", "e4", "e5", "e6", "e7" })
        {
            stringstream is, os;
            write_spoc_file_uncompressed (is, f);
            copy_field (is, os, fn1, fn2);
            const auto g = read_spoc_file_uncompressed (os);
        }
        for (auto fn2 : { "x", "y", "z", "q", "w", "e8"})
        {
            {
            bool failed = false;
            try {
                stringstream is, os;
                write_spoc_file_uncompressed (is, f);
                copy_field (is, os, fn1, fn2);
                const auto g = read_spoc_file_uncompressed (os);
            }
            catch (...) { failed = true; }
            // The preconditions are only checked in debug mode,
            // so use assert() instead of verify()
            assert (failed);
            (void) failed; // Turn off 'set but not used' error
            }

            // Reverse order of field names
            {
            bool failed = false;
            try {
                stringstream is, os;
                write_spoc_file_uncompressed (is, f);
                copy_field (is, os, fn2, fn1);
                const auto g = read_spoc_file_uncompressed (os);
            }
            catch (...) { failed = true; }
            // The preconditions are only checked in debug mode,
            // so use assert() instead of verify()
            assert (failed);
            (void) failed; // Turn off 'set but not used' error
            }
        }
    }
}

void test_transform_gaussian_noise ()
{
    // Generate spoc file
    auto f = generate_random_spoc_file (100, 3, false);
    // Copy point records
    auto prs = f.get_point_records ();
    // Set X, Y, Z values to 0.0
    for (auto &p : prs)
        p.x = p.y = p.z = 0.0;
    // Set new points
    f.set_point_records (prs);
    // Write to a stream
    stringstream is, os;
    write_spoc_file_uncompressed (is, f);
    // Add the noise
    gaussian_noise (is, os, 123, 1.0, 2.0, 3.0);
    // Get the result
    auto g = read_spoc_file_uncompressed (os);
    const auto x = get_x (g.get_point_records ());
    const auto y = get_y (g.get_point_records ());
    const auto z = get_z (g.get_point_records ());
    const auto min_x = *min_element (begin (x), end (x));
    const auto max_x = *max_element (begin (x), end (x));
    const auto min_y = *min_element (begin (y), end (y));
    const auto max_y = *max_element (begin (y), end (y));
    const auto min_z = *min_element (begin (z), end (z));
    const auto max_z = *max_element (begin (z), end (z));
    VERIFY (min_x < -1 && max_x > 1);
    VERIFY (min_y < -2 && max_y > 2);
    VERIFY (min_z < -3 && max_y > 3);
    VERIFY (min_x > -100 && max_x < 100);
    VERIFY (min_y > -200 && max_y < 200);
    VERIFY (min_z > -300 && max_z < 300);
}

void test_transform_quantize ()
{
    // Generate spoc file
    auto f = generate_random_spoc_file (10, 3, false);
    stringstream is, os;
    write_spoc_file_uncompressed (is, f);
    quantize (is, os, 0.001);

    write_spoc_file_uncompressed (is, f);
    quantize (is, os, 0.01);

    write_spoc_file_uncompressed (is, f);
    quantize (is, os, 100);
}

void test_transform_rotate ()
{
    // Generate random reference
    const auto p = generate_random_point_records (100);

    default_random_engine g;
    uniform_real_distribution<double> d (-M_PI, M_PI);

    for (auto i : { 0, 1, 2, 3, 4})
    {
        (void)i; // disable not used warning
        const double degrees = d (g);

        // Copy it
        auto qx (p);
        auto qy (p);
        auto qz (p);

        // Test streaming functions
        stringstream is, os;
        const string wkt ("Test WKT");
        write_spoc_file_uncompressed (is, spoc_file (wkt, false, p));
        write_spoc_file_uncompressed (is, spoc_file (wkt, false, p));
        write_spoc_file_uncompressed (is, spoc_file (wkt, false, p));

        rotate_x (is, os, degrees);
        auto r = read_spoc_file_uncompressed (os);
        qx = r.get_point_records ();

        rotate_y (is, os, degrees);
        r = read_spoc_file_uncompressed (os);
        qy = r.get_point_records ();

        rotate_z (is, os, degrees);
        r = read_spoc_file_uncompressed (os);
        qz = r.get_point_records ();

        // VERIFY that the correct axis was used
        VERIFY (p[3].x == qx[3].x);
        VERIFY (p[3].y != qx[3].y);
        VERIFY (p[3].z != qx[3].z);
        VERIFY (p[3].x != qy[3].x);
        VERIFY (p[3].y == qy[3].y);
        VERIFY (p[3].z != qy[3].z);
        VERIFY (p[3].x != qz[3].x);
        VERIFY (p[3].y != qz[3].y);
        VERIFY (p[3].z == qz[3].z);
    }
}

void test_transform_replace ()
{
    for (auto rgb : {true, false})
    {
        // Generate spoc file
        const size_t total_points = 100;
        const size_t extra_fields = 8;
        auto f = generate_random_spoc_file (total_points, extra_fields, false, rgb);
        // Copy point records
        auto prs = f.get_point_records ();
        for (size_t i = 0; i < total_points; ++i)
            for (size_t j = 0; j < extra_fields; ++j)
                prs[i].extra[j] = prs[i].extra[j] % 5;
        // Set new points
        f.set_point_records (prs);

        for (auto v1 : { 1.0, 2.0, 3.0, 4.0, 5.0 })
        {
            for (auto v2 : { 1.0, 2.0, 3.0, 4.0, 5.0 })
            {
                for (auto fn : { "c", "p", "i", "r", "g", "b",
                        "e0", "e1", "e2", "e3", "e4", "e5", "e6", "e7" })
                {
                    stringstream is, os;
                    write_spoc_file_uncompressed (is, f);
                    replace (is, os, fn, v1, v2);
                    const auto g = read_spoc_file_uncompressed (os);
                }
                for (auto fn : { "x", "y", "z", "q", "w", "e8"})
                {
                    bool failed = false;
                    try {
                        stringstream is, os;
                        write_spoc_file_uncompressed (is, f);
                        replace (is, os, fn, v1, v2);
                        const auto g = read_spoc_file_uncompressed (os);
                    }
                    catch (...) { failed = true; }
                    // The preconditions are only checked in debug mode,
                    // so use assert() instead of verify()
                    assert (failed);
                    (void) failed; // Turn off 'set but not used' error
                }
            }
        }
    }
}

void test_transform_scale ()
{
    // Generate random reference
    const auto p = generate_random_point_records (100);

    default_random_engine g;
    uniform_real_distribution<double> d (-100, 100);

    for (auto i : { 0, 1, 2, 3, 4})
    {
        (void)i; // disable not used warning
        const double s = d (g);

        // Copy it
        auto qx (p);
        auto qy (p);
        auto qz (p);

        // Test streaming functions
        stringstream is, os;
        const string wkt ("Test WKT");
        write_spoc_file_uncompressed (is, spoc_file (wkt, false, p));
        write_spoc_file_uncompressed (is, spoc_file (wkt, false, p));
        write_spoc_file_uncompressed (is, spoc_file (wkt, false, p));

        scale_x (is, os, s);
        auto r = read_spoc_file_uncompressed (os);
        qx = r.get_point_records ();

        scale_y (is, os, s);
        r = read_spoc_file_uncompressed (os);
        qy = r.get_point_records ();

        scale_z (is, os, s);
        r = read_spoc_file_uncompressed (os);
        qz = r.get_point_records ();

        // VERIFY that the correct axis was used
        VERIFY (about_equal (p[3].x * s, qx[3].x));
        VERIFY (p[3].y == qx[3].y);
        VERIFY (p[3].z == qx[3].z);
        VERIFY (p[3].x == qy[3].x);
        VERIFY (about_equal (p[3].y * s, qy[3].y));
        VERIFY (p[3].z == qy[3].z);
        VERIFY (p[3].x == qz[3].x);
        VERIFY (p[3].y == qz[3].y);
        VERIFY (about_equal (p[3].z * s, qz[3].z));
    }
}

void test_transform_set ()
{
    for (auto rgb : {true, false})
    {
        // Generate spoc file
        auto f = generate_random_spoc_file (100, 8, false, rgb);

        for (auto c : { "x", "y", "z",
                "c", "p", "i", "r", "g", "b",
                "e0", "e1", "e2", "e3", "e4", "e5", "e6", "e7" })
        {
            stringstream is, os;
            write_spoc_file_uncompressed (is, f);
            set (is, os, c, 123);
            const auto g = read_spoc_file_uncompressed (os);
        }
        for (auto c : { "q", "w", "e9"})
        {
            bool failed = false;
            try {
                stringstream is, os;
                write_spoc_file_uncompressed (is, f);
                set (is, os, c, 123);
                const auto g = read_spoc_file_uncompressed (os);
            }
            catch (...) { failed = true; }
            // The preconditions are only checked in debug mode, so use
            // assert() instead of verify()
            assert (failed);
            (void) failed; // Turn off 'set but not used' error
        }
    }
}

void test_transform_uniform_noise ()
{
    // Generate spoc file
    auto f = generate_random_spoc_file (100, 3, false);
    // Copy point records
    auto prs = f.get_point_records ();
    // Set X, Y, Z values to 0.0
    for (auto &p : prs)
        p.x = p.y = p.z = 0.0;
    // Set new points
    f.set_point_records (prs);
    // Write to a stream
    stringstream is, os;
    write_spoc_file_uncompressed (is, f);
    // Add the noise
    uniform_noise (is, os, 123, 1.0, 2.0, 3.0);
    // Get the result
    auto g = read_spoc_file_uncompressed (os);
    const auto x = get_x (g.get_point_records ());
    const auto y = get_y (g.get_point_records ());
    const auto z = get_z (g.get_point_records ());
    const auto min_x = *min_element (begin (x), end (x));
    const auto max_x = *max_element (begin (x), end (x));
    const auto min_y = *min_element (begin (y), end (y));
    const auto max_y = *max_element (begin (y), end (y));
    const auto min_z = *min_element (begin (z), end (z));
    const auto max_z = *max_element (begin (z), end (z));
    VERIFY (min_x > -1 && max_x < 1);
    VERIFY (min_y > -2 && max_y < 2);
    VERIFY (min_z > -3 && max_z < 3);
}

int main (int argc, char **argv)
{
    try
    {
        test_transform_detail ();
        test_transform_add ();
        test_transform_copy_field ();
        test_transform_gaussian_noise ();
        test_transform_quantize ();
        test_transform_rotate ();
        test_transform_replace ();
        test_transform_scale ();
        test_transform_set ();
        test_transform_uniform_noise ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
