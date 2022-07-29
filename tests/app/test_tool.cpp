#include "tool.h"
#include "spoc/spoc.h"
#include "spoc/test_utils.h"
#include <iostream>
#include <stdexcept>
#include <unordered_set>

using namespace std;
using namespace spoc::file;
using namespace spoc::io;
using namespace spoc::point_record;
using namespace spoc::subsampling;
using namespace spoc::tool_app;

void test_recenter ()
{
    for (auto rgb : {true, false})
    {
        // Generate spoc file
        const bool compressed = false;
        auto f = generate_random_spoc_file (1000, 8, compressed, rgb);
        auto g = recenter (f);
        auto h = recenter (f, true);

        const auto fx = get_x (f.get_point_records ());
        const auto fy = get_y (f.get_point_records ());
        const auto fz = get_z (f.get_point_records ());
        const auto gx = get_x (g.get_point_records ());
        const auto gy = get_y (g.get_point_records ());
        const auto gz = get_z (g.get_point_records ());
        const auto hx = get_x (h.get_point_records ());
        const auto hy = get_y (h.get_point_records ());
        const auto hz = get_z (h.get_point_records ());
        const auto n = f.get_header ().total_points;
        const auto cfx = accumulate (begin (fx), end (fx), 0.0) / n;
        const auto cfy = accumulate (begin (fy), end (fy), 0.0) / n;
        const auto cfz = accumulate (begin (fz), end (fz), 0.0) / n;
        const auto cgx = accumulate (begin (gx), end (gx), 0.0) / n;
        const auto cgy = accumulate (begin (gy), end (gy), 0.0) / n;
        const auto cgz = accumulate (begin (gz), end (gz), 0.0) / n;
        const auto chx = accumulate (begin (hx), end (hx), 0.0) / n;
        const auto chy = accumulate (begin (hy), end (hy), 0.0) / n;
        const auto chz = accumulate (begin (hz), end (hz), 0.0) / n;
        VERIFY (cfx != cgx);
        VERIFY (cfy != cgy);
        VERIFY (cfz == cgz);
        VERIFY (cfx != chx);
        VERIFY (cfy != chy);
        VERIFY (cfz != chz);
    }
}

void test_resize_extra ()
{
    // Generate spoc file
    auto f = generate_random_spoc_file (100, 8, false, true);
    auto g = resize_extra (f, 4);
    VERIFY (g.get_point_records ().front ().extra.size () == 4);
    VERIFY (g.get_point_records ().back ().extra.size () == 4);
    g = resize_extra (f, 0);
    VERIFY (g.get_point_records ().front ().extra.size () == 0);
    VERIFY (g.get_point_records ().back ().extra.size () == 0);
    g = resize_extra (f, 10);
    VERIFY (g.get_point_records ().front ().extra.size () == 10);
    VERIFY (g.get_point_records ().back ().extra.size () == 10);
}

void test_subtract_min ()
{
    // Generate spoc file
    auto f = generate_random_spoc_file (100, 8, false, true);
    auto g = subtract_min (f);
    auto h = subtract_min (f, true);
}

void test_get_field ()
{
    for (auto rgb : {true, false})
    {
        // Generate spoc file
        auto f = generate_random_spoc_file (100, 8, false, rgb);

        for (auto c : { "x", "y", "z",
                "c", "p", "i", "r", "g", "b",
                "e0", "e1", "e2", "e3", "e4", "e5", "e6", "e7" })
        {
            stringstream ss;
            get_field (f, ss, c);
        }
    }
}

void test_set_field ()
{
    for (auto rgb : {true, false})
    {
        // Generate spoc file
        const size_t n = 100;
        auto f = generate_random_spoc_file (n, 8, false, rgb);

        for (auto c : { "x", "y", "z",
                "c", "p", "i", "r", "g", "b",
                "e0", "e1", "e2", "e3", "e4", "e5", "e6", "e7" })
        {
            stringstream field_ifs;
            // Write field values
            for (size_t i = 0; i < n; ++i)
                field_ifs << i << endl;
            set_field (f, field_ifs, c);
        }
    }

    // Throw an error if there are not enough values
    const size_t n = 100;
    auto f = generate_random_spoc_file (n, 8, false);
    stringstream field_ifs;
    // Write field values with one missing
    for (size_t i = 0; i < n - 1; ++i)
        field_ifs << i << endl;
    VERIFY_THROWS (set_field (f, field_ifs, "x");)
}

void test_upsample_classifications ()
{
    const size_t n = 800;
    const size_t extra_fields = 3;
    auto f = generate_random_spoc_file (n, extra_fields);

    // Assign all high res classifications to 0
    for (auto &p : f)
        p.c = 0;

    // Subsample. The X, Y, and Z's are in the range +/-1.0
    const double resolution = 1.0;
    auto ind = get_subsample_indexes (f.get_point_records (), resolution, 123);

    spoc_file l = f.clone_empty ();
    const auto &prs = f.get_point_records ();
    const auto indexes = get_subsample_indexes (prs, resolution, 123);
    for (auto i : indexes)
        l.add (prs[i]);

    // Assign all low res classifications to 9
    for (auto &p : l)
        p.c = 9;

    // Each octant be filled with about 100 points
    VERIFY (l.get_point_records ().size () == 8);

    // Upsample, so all classes should be set to 9
    const bool verbose = true;
    stringstream s;
    const auto h = upsample_classifications (l, f, resolution, verbose, s);
    for (auto &p : h)
        VERIFY (p.c == 9);

    // Make a copy
    auto l2 (l);

    // Move the low resolution points
    for (auto &p : l2) { p.x += 1.0; p.y += 1.0; p.z += 1.0; }

    // Now only a single octant of the two point clouds overlap
    const auto h2 = upsample_classifications (l2, f, resolution, verbose, s);
    size_t assigned = 0;
    for (auto &p : h2)
        if (p.c == 9)
            ++assigned;

    // About 100 points should have been assigned
    VERIFY (assigned < 150);
    VERIFY (assigned > 50);
}

int main (int argc, char **argv)
{
    try
    {
        test_recenter ();
        test_resize_extra ();
        test_subtract_min ();
        test_get_field ();
        test_set_field ();
        test_upsample_classifications ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
