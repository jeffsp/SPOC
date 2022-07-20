#include "transform.h"
#include "test_utils.h"
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace spoc::io;
using namespace spoc::point;
using namespace spoc::transform_app;

void test_transform_detail ()
{
    // Generate spoc files
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

        // Test in-place functions
        add_x (qx, offset);
        add_y (qy, offset);
        add_z (qz, offset);

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

        // Test streaming functions
        stringstream is, os;
        const string wkt ("Test WKT");
        write_spoc_file_uncompressed (is, spoc_file (wkt, p));
        write_spoc_file_uncompressed (is, spoc_file (wkt, p));
        write_spoc_file_uncompressed (is, spoc_file (wkt, p));

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

void test_transform_quantize ()
{
    // Generate spoc files
    auto f = generate_random_spoc_file (10, 3, false);
    stringstream is, os;
    write_spoc_file_uncompressed (is, f);
    quantize (is, os, 0.001);

    write_spoc_file_uncompressed (is, f);
    quantize (is, os, 0.01);

    write_spoc_file_uncompressed (is, f);
    quantize (is, os, 100);

    clog << fixed;
    auto q = generate_points (10);
    quantize (q, 0.001);
    quantize (q, 0.1);
    quantize (q, 1.0);
}

void test_transform_rotate1 ()
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

        // Test in-place functions
        rotate_x (qx, degrees);
        rotate_y (qy, degrees);
        rotate_z (qz, degrees);

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

        // Test streaming functions
        stringstream is, os;
        const string wkt ("Test WKT");
        write_spoc_file_uncompressed (is, spoc_file (wkt, p));
        write_spoc_file_uncompressed (is, spoc_file (wkt, p));
        write_spoc_file_uncompressed (is, spoc_file (wkt, p));

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

void test_transform_rotate2 ()
{
    default_random_engine g;
    uniform_real_distribution<double> d (0.0, 1.0);

    for (auto i : { 0.0, 1.0, 2.0, 3.0, 4.0})
    {
        // Generate random coords
        const double x = i * d (g);
        const double y = i * d (g);
        const double z = i * d (g);

        // Point cloud with a single point that lies on X axis
        const vector<point<double>> p { { x, y, z } };

        // Copy it
        auto q (p);

        // These are all no-ops, since it's rotating around its own axis
        rotate_x (q, 0);
        rotate_x (q, 90);
        rotate_x (q, -180);
        VERIFY (p[0].x == q[0].x);

        // Copy it
        q = p;

        // These are all no-ops, since it's rotating around its own axis
        rotate_y (q, 0);
        rotate_y (q, 90);
        rotate_y (q, -180);
        VERIFY (p[0].y == q[0].y);

        // Copy it
        q = p;

        // These are all no-ops, since it's rotating around its own axis
        rotate_z (q, 0);
        rotate_z (q, 90);
        rotate_z (q, -180);
        VERIFY (p[0].z == q[0].z);

        // Check all axis rotations

        // Copy it
        q = p;

        // X
        rotate_x (q, 90);
        VERIFY (p[0].x == q[0].x);
        VERIFY (about_equal (p[0].y, q[0].z));
        VERIFY (about_equal (p[0].z, -q[0].y));

        rotate_x (q, -90);
        VERIFY (p[0].x == q[0].x);
        VERIFY (about_equal (p[0].y, q[0].y));
        VERIFY (about_equal (p[0].z, q[0].z));

        rotate_x (q, 180);
        VERIFY (p[0].x == q[0].x);
        VERIFY (about_equal (p[0].y, -q[0].y));
        VERIFY (about_equal (p[0].z, -q[0].z));

        // Copy it
        q = p;

        // Y
        rotate_y (q, 90);
        VERIFY (p[0].y == q[0].y);
        VERIFY (about_equal (p[0].x, q[0].z));
        VERIFY (about_equal (p[0].z, -q[0].x));

        rotate_y (q, -90);
        VERIFY (p[0].y == q[0].y);
        VERIFY (about_equal (p[0].x, q[0].x));
        VERIFY (about_equal (p[0].z, q[0].z));

        rotate_y (q, 180);
        VERIFY (p[0].y == q[0].y);
        VERIFY (about_equal (p[0].x, -q[0].x));
        VERIFY (about_equal (p[0].z, -q[0].z));

        // Copy it
        q = p;

        // Z
        rotate_z (q, 90);
        VERIFY (p[0].z == q[0].z);
        VERIFY (about_equal (p[0].y, -q[0].x));
        VERIFY (about_equal (p[0].x, q[0].y));

        rotate_z (q, -90);
        VERIFY (p[0].z == q[0].z);
        VERIFY (about_equal (p[0].y, q[0].y));
        VERIFY (about_equal (p[0].x, q[0].x));

        rotate_z (q, 180);
        VERIFY (p[0].z == q[0].z);
        VERIFY (about_equal (p[0].y, -q[0].y));
        VERIFY (about_equal (p[0].x, -q[0].x));
    }
}

void test_transform_replace ()
{
    for (auto rgb : {true, false})
    {
        // Generate spoc files
        const size_t total_points = 100;
        const size_t extra_fields = 8;
        auto f = generate_random_spoc_file (total_points, extra_fields, false, rgb);
        for (size_t i = 0; i < total_points; ++i)
            for (size_t j = 0; j < extra_fields; ++j)
                f[i].extra[j] = f[i].extra[j] % 5;

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

        // Test in-place functions
        scale_x (qx, s);
        scale_y (qy, s);
        scale_z (qz, s);

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

        // Test streaming functions
        stringstream is, os;
        const string wkt ("Test WKT");
        write_spoc_file_uncompressed (is, spoc_file (wkt, p));
        write_spoc_file_uncompressed (is, spoc_file (wkt, p));
        write_spoc_file_uncompressed (is, spoc_file (wkt, p));

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
        // Generate spoc files
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

int main (int argc, char **argv)
{
    try
    {
        test_transform_detail ();
        test_transform_add ();
        test_transform_quantize ();
        test_transform_rotate1 ();
        test_transform_rotate2 ();
        test_transform_replace ();
        test_transform_scale ();
        test_transform_set ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
