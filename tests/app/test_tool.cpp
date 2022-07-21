#include "tool.h"
#include "test_utils.h"
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace spoc::io;
using namespace spoc::tool_app;

void test_recenter ()
{
    for (auto rgb : {true, false})
    {
        // Generate spoc files
        auto f = generate_random_spoc_file (1000, 8, false, rgb);
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

void test_subtract_min ()
{
    // Generate spoc files
    auto f = generate_random_spoc_file (100, 8, false, true);
    auto g = subtract_min (f);
    auto h = subtract_min (f, true);
}

void test_get_field ()
{
    for (auto rgb : {true, false})
    {
        // Generate spoc files
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
        // Generate spoc files
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

int main (int argc, char **argv)
{
    try
    {
        test_recenter ();
        test_subtract_min ();
        test_get_field ();
        test_set_field ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
