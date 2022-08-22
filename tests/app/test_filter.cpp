#include "filter.h"
#include "spoc/spoc.h"
#include "spoc/test_utils.h"
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace spoc::file;
using namespace spoc::filter_app;
using namespace spoc::io;
using namespace spoc::point_record;

void test_keep_classes ()
{
    spoc_file f;
    f.push_back (point_record (0.0, 0.0, 0.0, 1, 0, 0, 0, 0, 0));
    f.push_back (point_record (0.0, 0.0, 0.0, 2, 0, 0, 0, 0, 0));
    f.push_back (point_record (0.0, 0.0, 0.0, 2, 0, 0, 0, 0, 0));
    f.push_back (point_record (0.0, 0.0, 0.0, 2, 0, 0, 0, 0, 0));
    f.push_back (point_record (0.0, 0.0, 0.0, 3, 0, 0, 0, 0, 0));
    auto g = keep_classes (f, unordered_set<int> { 2 });
    VERIFY (g.get_point_records ().size () == 3);
    VERIFY (g.get_point_records ().front ().c == 2);
    VERIFY (g.get_point_records ().back ().c == 2);
    g = keep_classes (f, unordered_set<int> { 1 });
    VERIFY (g.get_point_records ().size () == 1);
    VERIFY (g.get_point_records ().front ().c == 1);
    g = keep_classes (f, unordered_set<int> { 3 });
    VERIFY (g.get_point_records ().size () == 1);
    VERIFY (g.get_point_records ().front ().c == 3);
    g = keep_classes (f, unordered_set<int> { 1000 });
    VERIFY (g.get_point_records ().empty ());
    g = keep_classes (f, unordered_set<int> { 1, 2, 3 });
    VERIFY (g.get_point_records ().size () == f.get_point_records ().size ());
    VERIFY (g.get_point_records ().front ().c == 1);
    VERIFY (g.get_point_records ().back ().c == 3);
}

void test_remove_classes ()
{
    spoc_file f;
    f.push_back (point_record (0.0, 0.0, 0.0, 1, 0, 0, 0, 0, 0));
    f.push_back (point_record (0.0, 0.0, 0.0, 2, 0, 0, 0, 0, 0));
    f.push_back (point_record (0.0, 0.0, 0.0, 2, 0, 0, 0, 0, 0));
    f.push_back (point_record (0.0, 0.0, 0.0, 2, 0, 0, 0, 0, 0));
    f.push_back (point_record (0.0, 0.0, 0.0, 3, 0, 0, 0, 0, 0));
    auto g = remove_classes (f, unordered_set<int> { 2 });
    VERIFY (g.get_point_records ().size () == 2);
    VERIFY (g.get_point_records ().front ().c == 1);
    VERIFY (g.get_point_records ().back ().c == 3);
    g = remove_classes (f, unordered_set<int> { 1 });
    VERIFY (g.get_point_records ().size () == 4);
    VERIFY (g.get_point_records ().front ().c == 2);
    g = remove_classes (f, unordered_set<int> { 3 });
    VERIFY (g.get_point_records ().size () == 4);
    VERIFY (g.get_point_records ().front ().c == 1);
    VERIFY (g.get_point_records ().back ().c == 2);
    g = remove_classes (f, unordered_set<int> { 1000 });
    VERIFY (g.get_point_records ().size () == f.get_point_records ().size ());
    VERIFY (g.get_point_records ().front ().c == 1);
    VERIFY (g.get_point_records ().back ().c == 3);
    g = remove_classes (f, unordered_set<int> { 1, 2, 3 });
    VERIFY (g.get_point_records ().empty ());
}

void test_unique_xyz ()
{
    {
    spoc_file f;
    f.push_back (point_record (0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0));
    f.push_back (point_record (0.1, 0.0, 0.0, 1, 0, 0, 0, 0, 0));
    f.push_back (point_record (0.0, 0.2, 0.0, 2, 0, 0, 0, 0, 0));
    f.push_back (point_record (0.0, 0.0, 0.3, 3, 0, 0, 0, 0, 0));
    f.push_back (point_record (0.1, 0.0, 0.0, 4, 0, 0, 0, 0, 0));
    f.push_back (point_record (0.0, 0.2, 0.0, 5, 0, 0, 0, 0, 0));
    f.push_back (point_record (0.0, 0.0, 0.3, 6, 0, 0, 0, 0, 0));
    const size_t random_seed = 0;
    auto g = unique_xyz (f, random_seed);
    VERIFY (g.get_point_records ().size () == 4);
    VERIFY (g.get_point_records ().front ().c == 0);
    VERIFY (g.get_point_records ().back ().c == 3);
    }

    {
    spoc_file f;
    const size_t n = 1000;

    for (size_t i = 0; i < n; ++i)
    {
        f.push_back (point_record (i, 0.0, 0.0, i, 0, 0, 0, 0, 0));
        f.push_back (point_record (i, 0.0, 0.0, i, 0, 0, 0, 0, 0));
    }

    // 1000 points + 1000 more duplicates
    VERIFY (f.get_point_records ().size () == n*2);

    auto g = unique_xyz (f, 0); // Don't randomize
    auto h = unique_xyz (f, 123); // Do randomize

    // Half are gone
    VERIFY (g.get_point_records ().size () == n);
    VERIFY (h.get_point_records ().size () == n);

    // They should not have been selected randomly
    VERIFY (g.get_point_records ().front ().c == 0);
    VERIFY (g.get_point_records ().back ().c == n - 1);

    // They should have been selected randomly
    VERIFY (h.get_point_records ().front ().c != 0);
    VERIFY (h.get_point_records ().back ().c != n - 1);
    }
}

void test_subsample ()
{
    const size_t n = 800;
    const size_t extra_fields = 3;
    auto f = generate_random_spoc_file (n, extra_fields);

    // Subsample
    const double resolution = 1.0;
    auto l = subsample (f, resolution, 123);

    // Each octant be filled with about 100 points
    VERIFY (l.get_point_records ().size () == 8);
}

int main (int argc, char **argv)
{
    try
    {
        test_keep_classes ();
        test_remove_classes ();
        test_unique_xyz ();
        test_subsample ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
