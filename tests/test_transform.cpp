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
        test_transform_pipes ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
