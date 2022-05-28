#include "transform.h"
#include "test_utils.h"
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace spoc::transform;

void test_transform_set ()
{
    for (auto rgb : {true, false})
    {
        // Generate spoc files
        auto f = generate_random_spoc_file (100, 8, rgb);

        for (auto c : { 'x', 'y', 'z',
                'c', 'p', 'i', 'r', 'g', 'b',
                '0', '1', '2', '3', '4', '5', '6', '7' })
            const auto g = set (f, c, 123);
        for (auto c : { 'q', 'w', '9'})
        {
            bool failed = false;
            try { const auto g = set (f, c, 123); }
            catch (...) { failed = true; }
            verify (failed);
        }
    }
}

void test_transform_replace ()
{
    for (auto rgb : {true, false})
    {
        // Generate spoc files
        auto f = generate_random_spoc_file (100, 8, rgb);

        for (auto v1 : { 1.0, 2.0, 3.0, 4.0, 5.0 })
        {
            for (auto v2 : { 1.0, 2.0, 3.0, 4.0, 5.0 })
            {
                for (auto c : { 'c', 'p', 'i', 'r', 'g', 'b',
                        '0', '1', '2', '3', '4', '5', '6', '7' })
                    const auto g = replace (f, c, v1, v2);
                for (auto c : { 'x', 'y', 'z', 'q', 'w', '8'})
                {
                    bool failed = false;
                    try { const auto g = replace (f, c, v1, v2); }
                    catch (...) { failed = true; }
                    verify (failed);
                }
            }
        }
    }
}

void test_transform_recenter ()
{
    for (auto rgb : {true, false})
    {
        // Generate spoc files
        auto f = generate_random_spoc_file (100, 8, rgb);

        auto g = recenter (f);
        auto h = recenter (g, true);
        const auto fx = get_x (f.get_points ());
        const auto fy = get_y (f.get_points ());
        const auto fz = get_z (f.get_points ());
        const auto gx = get_x (g.get_points ());
        const auto gy = get_y (g.get_points ());
        const auto gz = get_z (g.get_points ());
        const auto hx = get_x (h.get_points ());
        const auto hy = get_y (h.get_points ());
        const auto hz = get_z (h.get_points ());
        const auto n = f.get_header ().total_points;
        const auto cfx = std::accumulate (begin (fx), end (fx), 0.0) / n;
        const auto cfy = std::accumulate (begin (fy), end (fy), 0.0) / n;
        const auto cfz = std::accumulate (begin (fz), end (fz), 0.0) / n;
        const auto cgx = std::accumulate (begin (gx), end (gx), 0.0) / n;
        const auto cgy = std::accumulate (begin (gy), end (gy), 0.0) / n;
        const auto cgz = std::accumulate (begin (gz), end (gz), 0.0) / n;
        const auto chx = std::accumulate (begin (hx), end (hx), 0.0) / n;
        const auto chy = std::accumulate (begin (hy), end (hy), 0.0) / n;
        const auto chz = std::accumulate (begin (hz), end (hz), 0.0) / n;
        verify (cfx != cgx);
        verify (cfy != cgy);
        verify (cfz == cgz);
        verify (cfx != chx);
        verify (cfy != chy);
        verify (cfz != chz);
    }
}

void test_transform_subtract_min ()
{
    // Generate spoc files
    auto f = generate_random_spoc_file (100, 8, true);
    auto g = subtract_min (f);
    auto h = subtract_min (g, true);
}

int main (int argc, char **argv)
{
    using namespace std;
    try
    {
        test_transform_set ();
        test_transform_replace ();
        test_transform_recenter ();
        test_transform_subtract_min ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
