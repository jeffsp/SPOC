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
        // Generate some records
        const auto p = get_random_point_records (100, rgb);

        // Generate a spoc_file
        stringstream s;
        write_spoc_file (s, string ("Test WKT"), p);
        auto f = spoc::read_spoc_file (s);

        for (auto c : { 'x', 'y', 'z',
                'c', 'p', 'i', 'r', 'g', 'b',
                '0', '1', '2', '3', '4', '5', '6', '7' })
            const auto g = run_set_command (f, c, 123);
        for (auto c : { 'q', 'w'})
        {
            bool failed = false;
            try { const auto g = run_set_command (f, c, 123); }
            catch (...) { failed = true; }
            verify (failed);
        }
    }
}

void test_transform_replace ()
{
    for (auto rgb : {true, false})
    {
        // Generate some records
        const auto p = get_random_point_records (100, rgb);

        // Generate a spoc_file
        stringstream s;
        write_spoc_file (s, string ("Test WKT"), p);
        auto f = spoc::read_spoc_file (s);

        vector<pair<unsigned,unsigned>> r;
        for (auto r1 : { 1, 2, 3, 4, 5 })
            for (auto r2 : { 1, 2, 3, 4, 5 })
                r.push_back (make_pair (r1, r2));
        for (auto c : { 'c', 'p', 'i', 'r', 'g', 'b',
                '0', '1', '2', '3', '4', '5', '6', '7' })
            const auto g = run_replace_command (f, c, r);
        for (auto c : { 'x', 'y', 'z', 'q', 'w'})
        {
            bool failed = false;
            try { const auto g = run_replace_command (f, c, r); }
            catch (...) { failed = true; }
            verify (failed);
        }
    }
}

void test_transform_recenter ()
{
    for (auto rgb : {true, false})
    {
        // Generate some records
        const auto p = get_random_point_records (100, rgb);

        // Generate a spoc_file
        stringstream s;
        write_spoc_file (s, string ("Test WKT"), p);
        auto f = spoc::read_spoc_file (s);
        auto g = recenter (f);
        auto h = recenter (g, true);
        const auto fx = f.get_x ();
        const auto fy = f.get_y ();
        const auto fz = f.get_z ();
        const auto gx = g.get_x ();
        const auto gy = g.get_y ();
        const auto gz = g.get_z ();
        const auto hx = h.get_x ();
        const auto hy = h.get_y ();
        const auto hz = h.get_z ();
        const auto n = f.get_npoints ();
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

void test_transform_pipes ()
{
    for (auto rgb : {true, false})
    {
        // Generate some records
        const auto p = get_random_point_records (100, rgb);

        // Generate a spoc_file
        stringstream s;
        write_spoc_file (s, string ("Test WKT"), p);
        auto f = spoc::read_spoc_file (s);
        stringstream ss;
        auto g = run_pipe_command (f, ss, ss);
    }
}

int main (int argc, char **argv)
{
    using namespace std;
    try
    {
        test_transform_set ();
        test_transform_replace ();
        test_transform_recenter ();
        test_transform_pipes ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
