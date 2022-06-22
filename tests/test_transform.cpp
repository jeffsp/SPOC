#include "transform.h"
#include "test_utils.h"
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace spoc;
using namespace spoc::transform;

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

        // Test in-place functions
        rotate_x (qx, degrees);
        rotate_y (qy, degrees);
        rotate_z (qz, degrees);

        // Verify that the correct axis was used
        verify (p[3].x == qx[3].x);
        verify (p[3].y != qx[3].y);
        verify (p[3].z != qx[3].z);
        verify (p[3].x != qy[3].x);
        verify (p[3].y == qy[3].y);
        verify (p[3].z != qy[3].z);
        verify (p[3].x != qz[3].x);
        verify (p[3].y != qz[3].y);
        verify (p[3].z == qz[3].z);

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

        // Verify that the correct axis was used
        verify (p[3].x == qx[3].x);
        verify (p[3].y != qx[3].y);
        verify (p[3].z != qx[3].z);
        verify (p[3].x != qy[3].x);
        verify (p[3].y == qy[3].y);
        verify (p[3].z != qy[3].z);
        verify (p[3].x != qz[3].x);
        verify (p[3].y != qz[3].y);
        verify (p[3].z == qz[3].z);
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
                    verify (failed);
                }
            }
        }
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
            verify (failed);
        }
    }
}

int main (int argc, char **argv)
{
    using namespace std;
    try
    {
        test_transform_quantize ();
        test_transform_rotate ();
        test_transform_replace ();
        test_transform_set ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
