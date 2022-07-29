#include "spoc/affine.h"
#include "spoc/test_utils.h"
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace spoc::affine;

void test_add ()
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
    }
}

void test_rotate ()
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

void test_scale ()
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
    }
}

int main (int argc, char **argv)
{
    try
    {
        test_add ();
        test_rotate ();
        test_rotate2 ();
        test_scale ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
