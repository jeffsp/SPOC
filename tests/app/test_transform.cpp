#include "spoc/spoc.h"
#include "spoc/test_utils.h"
#include "transform.h"
#include "transform_cmd.h"
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace spoc::file;
using namespace spoc::io;
using namespace spoc::point;
using namespace spoc::point_record;
using namespace spoc::test_utils;
using namespace spoc::transform_app;
using namespace spoc::transform_cmd;

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
    const size_t random_seed = 0;

    for (auto i : { 0, 1, 2, 3, 4})
    {
        (void)i; // disable not used warning
        const double offset = d (g);

        // Write it out 3 times
        stringstream is, os;
        const string wkt ("Test WKT");
        write_spoc_file_uncompressed (is, spoc_file (wkt, false, p));
        write_spoc_file_uncompressed (is, spoc_file (wkt, false, p));
        write_spoc_file_uncompressed (is, spoc_file (wkt, false, p));

        vector<command> commands (1);

        // Apply to first one on x
        commands[0] = command ("add-x", std::to_string (offset));
        apply (is, os, commands, random_seed);
        auto r = read_spoc_file_uncompressed (os);
        auto qx = r.get_point_records ();

        // Apply to second one on y
        commands[0] = command ("add-y", std::to_string (offset));
        apply (is, os, commands, random_seed);
        r = read_spoc_file_uncompressed (os);
        auto qy = r.get_point_records ();

        // Apply to third one on z
        commands[0] = command ("add-z", std::to_string (offset));
        apply (is, os, commands, random_seed);
        r = read_spoc_file_uncompressed (os);
        auto qz = r.get_point_records ();

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
    const size_t random_seed = 0;

    // Copy c to p
    {
        stringstream is, os;
        write_spoc_file_uncompressed (is, f);
        vector<command> commands (1);
        commands[0] = command ("copy-field", "c,p");
        apply (is, os, commands, random_seed);
        const auto g = read_spoc_file_uncompressed (os);
        const auto c = get_c (f.get_point_records ());
        const auto p = get_p (g.get_point_records ());
        VERIFY (c == p);
    }

    // Copy e7 to e0
    {
        stringstream is, os;
        write_spoc_file_uncompressed (is, f);
        vector<command> commands (1);
        commands[0] = command ("copy-field", "e7,e0");
        apply (is, os, commands, random_seed);
        const auto g = read_spoc_file_uncompressed (os);
        const auto e7 = get_extra (7, f.get_point_records ());
        const auto e0 = get_extra (0, g.get_point_records ());
        VERIFY (e7 == e0);
    }

    // Copy i to e2
    {
        stringstream is, os;
        write_spoc_file_uncompressed (is, f);
        vector<command> commands (1);
        commands[0] = command ("copy-field", "i,e2");
        apply (is, os, commands, random_seed);
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
            vector<command> commands (1);
            commands[0] = command ("copy-field", string(fn1) + "," + fn2);
            apply (is, os, commands, random_seed);
            const auto g = read_spoc_file_uncompressed (os);
        }
        for (auto fn2 : { "x", "y", "z", "q", "w", "e8"})
        {
            {
            bool failed = false;
            try {
                stringstream is, os;
                write_spoc_file_uncompressed (is, f);
                vector<command> commands (1);
                commands[0] = command ("copy-field", string(fn1) + "," + fn2);
                apply (is, os, commands, random_seed);
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
                vector<command> commands (1);
                commands[0] = command ("copy-field", string(fn2) + "," + fn1);
                apply (is, os, commands, random_seed);
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
    vector<command> commands (3);
    commands[0] = command ("gaussian-noise-x", "1.0");
    commands[1] = command ("gaussian-noise-y", "2.0");
    commands[2] = command ("gaussian-noise-z", "3.0");
    const size_t random_seed = 123;
    apply (is, os, commands, random_seed);
    // Get the result
    const auto g = read_spoc_file_uncompressed (os);
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
    commands[0] = command ("gaussian-noise", "1.0");
    commands[1] = command ("gaussian-noise", "2.0");
    commands[2] = command ("gaussian-noise", "3.0");
    write_spoc_file_uncompressed (is, f);
    // Add the noise
    apply (is, os, commands, random_seed);
}

void test_transform_quantize ()
{
    // Generate spoc file
    auto f = generate_random_spoc_file (10, 3, false);
    stringstream is, os;
    vector<command> commands (1);
    const size_t random_seed = 0;

    write_spoc_file_uncompressed (is, f);
    commands[0] = command ("quantize-xyz", "0.001");
    apply (is, os, commands, random_seed);
    auto g = read_spoc_file_uncompressed (os);
    auto x = get_x (g.get_point_records ());
    for (auto i : x)
        VERIFY (i != 0.0 && (static_cast<int> (i * 1000) / 1000 == 0.0));

    write_spoc_file_uncompressed (is, f);
    commands[0] = command ("quantize-xyz", "0.01");
    apply (is, os, commands, random_seed);
    g = read_spoc_file_uncompressed (os);
    auto y = get_y (g.get_point_records ());
    for (auto i : y)
        VERIFY (i != 0.0 && (static_cast<int> (i * 100) / 100 == 0.0));

    write_spoc_file_uncompressed (is, f);
    commands[0] = command ("quantize-xyz", "100");
    apply (is, os, commands, random_seed);
    g = read_spoc_file_uncompressed (os);
    auto z = get_z (g.get_point_records ());
    for (auto i : z)
        VERIFY (i == 0);
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

        for (auto v1 : { 1, 2, 3, 4, 5 })
        {
            for (auto v2 : { 1, 2, 3, 4, 5 })
            {
                for (auto fn : { "c", "p", "i", "r", "g", "b",
                        "e0", "e1", "e2", "e3", "e4", "e5", "e6", "e7" })
                {
                    stringstream is, os;
                    write_spoc_file_uncompressed (is, f);
                    const size_t random_seed = 0;
                    vector<command> commands;
                    commands.push_back (command ("replace",
                        std::string (fn) + "," +
                        std::to_string (v1) + "," +
                        std::to_string (v2)));
                    apply (is, os, commands, random_seed);
                    const auto g = read_spoc_file_uncompressed (os);
                }
                for (auto fn : { "x", "y", "z", "q", "w", "e8"})
                {
                    bool failed = false;
                    try {
                        stringstream is, os;
                        write_spoc_file_uncompressed (is, f);
                        const size_t random_seed = 0;
                        vector<command> commands;
                        commands.push_back (command ("replace",
                            std::string (fn) + "," +
                            std::to_string (v1) + "," +
                            std::to_string (v2)));
                        apply (is, os, commands, random_seed);
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

void test_transform_replace_not ()
{
    for (auto rgb : {true, false})
    {
        // Generate spoc file
        const size_t total_points = 100;
        const size_t extra_fields = 8;
        auto f = generate_random_spoc_file (total_points, extra_fields, false, rgb);
        // Copy point records
        auto prs = f.get_point_records ();
        for (size_t i = 0; i < prs.size (); ++i)
        {
            // Limit the range of some of the random values
            prs[i].c %= 10;
            prs[i].p %= 10;
            prs[i].i %= 10;
            for (size_t j = 0; j < extra_fields; ++j)
                prs[i].extra[j] = prs[i].extra[j] % 5;
        }
        // Set new points
        f.set_point_records (prs);

        // Do this several times
        for (auto total_values : {0u, 1u, 2u, 3u, 4u})
        {
            // For each field name
            for (auto fn : { "c", "p", "i", "r", "g", "b",
                    "e0", "e1", "e2", "e3", "e4", "e5", "e6", "e7" })
            {
                // Create the list of values
                vector<size_t> values;
                for (size_t i = 0; i < total_values; ++i)
                    values.push_back (rand () % 20);

                string cmd_arg = string (fn);
                for (auto v : values)
                    cmd_arg += string (",") + to_string(v);
                bool failed = false;
                try {
                    stringstream is, os;
                    write_spoc_file_uncompressed (is, f);
                    const size_t random_seed = 0;
                    vector<command> commands;
                    commands.push_back (command ("replace-not", cmd_arg));
                    apply (is, os, commands, random_seed);
                    const auto g = read_spoc_file_uncompressed (os);
                }
                catch (...) { failed = true; }
                // It should fail when there are not enough values in the
                // string. There must be at least two
                if (values.size () < 2)
                {
                    VERIFY (failed == true);
                }
                else
                {
                    VERIFY (failed == false);
                }
            }
            // For each invalid field name
            for (auto fn : { "x", "y", "z", "q", "e100"})
            {
                // Create the list of values
                vector<size_t> values;
                for (size_t i = 0; i < total_values; ++i)
                    values.push_back (rand () % 20);

                string cmd_arg = string (fn);
                for (auto v : values)
                    cmd_arg += string (",") + to_string(v);
                bool failed = false;
                try {
                    stringstream is, os;
                    write_spoc_file_uncompressed (is, f);
                    const size_t random_seed = 0;
                    vector<command> commands;
                    commands.push_back (command ("replace-not", cmd_arg));
                    apply (is, os, commands, random_seed);
                    const auto g = read_spoc_file_uncompressed (os);
                }
                catch (...) { failed = true; }
                VERIFY (failed == true);
            }
        }
    }
}

void test_transform_rotate ()
{
    // Generate random reference
    const auto p = generate_random_point_records (100);

    default_random_engine g;
    uniform_real_distribution<double> d (-M_PI, M_PI);
    const size_t random_seed = 0;
    vector<command> commands (1);

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

        commands[0] = command ("rotate-x", std::to_string (degrees));
        apply (is, os, commands, random_seed);
        auto r = read_spoc_file_uncompressed (os);
        qx = r.get_point_records ();

        commands[0] = command ("rotate-y", std::to_string (degrees));
        apply (is, os, commands, random_seed);
        r = read_spoc_file_uncompressed (os);
        qy = r.get_point_records ();

        commands[0] = command ("rotate-z", std::to_string (degrees));
        apply (is, os, commands, random_seed);
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

void test_transform_scale ()
{
    // Generate random reference
    const auto p = generate_random_point_records (100);

    default_random_engine g;
    uniform_real_distribution<double> d (-100, 100);
    const size_t random_seed = 0;
    vector<command> commands (1);

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

        commands[0] = command ("scale-x", std::to_string (s));
        apply (is, os, commands, random_seed);
        auto r = read_spoc_file_uncompressed (os);
        qx = r.get_point_records ();

        commands[0] = command ("scale-y", std::to_string (s));
        apply (is, os, commands, random_seed);
        r = read_spoc_file_uncompressed (os);
        qy = r.get_point_records ();

        commands[0] = command ("scale-z", std::to_string (s));
        apply (is, os, commands, random_seed);
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
    const size_t random_seed = 0;
    vector<command> commands (1);

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
            commands[0] = command ("set", std::string (c) + ",123");
            apply (is, os, commands, random_seed);
            const auto g = read_spoc_file_uncompressed (os);
        }
        for (auto c : { "q", "w", "e9"})
        {
            bool failed = false;
            try {
                stringstream is, os;
                write_spoc_file_uncompressed (is, f);
                commands[0] = command ("set", std::string (c) + ",123");
                apply (is, os, commands, random_seed);
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
    vector<command> commands (1);
    commands[0] = command ("uniform-noise",
                        std::string ("1.0") + "," +
                        std::string ("2.0") + "," +
                        std::string ("3.0"));
    const size_t random_seed = 123;
    apply (is, os, commands, random_seed);
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
    commands[0] = command ("uniform-noise-x", "1.0");
    write_spoc_file_uncompressed (is, f);
    apply (is, os, commands, random_seed);
    commands[0] = command ("uniform-noise-y", "1.0");
    write_spoc_file_uncompressed (is, f);
    apply (is, os, commands, random_seed);
    commands[0] = command ("uniform-noise-z", "1.0");
    write_spoc_file_uncompressed (is, f);
    apply (is, os, commands, random_seed);
}

void test_transform_multiple_ops ()
{
    // Generate spoc file
    const size_t total_points = 100;
    const size_t extra_fields = 8;
    auto f1 = generate_random_spoc_file (total_points, extra_fields);
    const auto p1 = f1.get_point_records ();

    const size_t random_seed = 0;
    vector<command> forward_commands;
    forward_commands.push_back (command ("add-x", "10.0"));
    forward_commands.push_back (command ("rotate-x", "17.5"));
    forward_commands.push_back (command ("scale-x", "100.0"));
    vector<command> reverse_commands;
    reverse_commands.push_back (command ("scale-x", "0.01"));
    reverse_commands.push_back (command ("rotate-x", "-17.5"));
    reverse_commands.push_back (command ("add-x", "-10.0"));

    // Streams for I/O
    stringstream s1, s2, s3;

    // Perform the transform
    write_spoc_file_uncompressed (s1, f1);
    apply (s1, s2, forward_commands, random_seed);

    // The points should have changed
    const auto f2 = read_spoc_file_uncompressed (s2);
    const auto p2 = f2.get_point_records ();
    VERIFY (!about_equal (p1.back ().x, p2.back ().x));
    VERIFY (!about_equal (p1.back ().y, p2.back ().y));
    VERIFY (!about_equal (p1.back ().z, p2.back ().z));

    // Undo the transform
    write_spoc_file_uncompressed (s2, f2);
    apply (s2, s3, reverse_commands, random_seed);

    // The points should match the original
    const auto f3 = read_spoc_file_uncompressed (s3);
    const auto p3 = f3.get_point_records ();
    VERIFY (about_equal (p1.back ().x, p3.back ().x));
    VERIFY (about_equal (p1.back ().y, p3.back ().y));
    VERIFY (about_equal (p1.back ().z, p3.back ().z));
}

void test_transform_bad_command ()
{
    bool failed = false;
    try {
        stringstream is, os;
        vector<command> commands (1);
        commands[0] = command ("xxx", "1.0");
        apply (is, os, commands, 123);
    }
    catch (...) { failed = true; }
    VERIFY (failed);
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
        test_transform_replace_not ();
        test_transform_scale ();
        test_transform_set ();
        test_transform_uniform_noise ();
        test_transform_multiple_ops ();
        test_transform_bad_command ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
