#include "diff.h"
#include "test_utils.h"
#include <iostream>
#include <stdexcept>

void test_diff ()
{
    using namespace std;

    // Generate some records
    const auto p = get_random_point_records (100, true);
    const auto q = get_random_point_records (100, false);

    // Generate a spoc_file
    stringstream s;
    write_spoc_file (s, string ("Text WKT"), p);
    write_spoc_file (s, string ("Text WKT"), q);
    auto f1 = spoc::read_spoc_file (s);
    auto f2 = spoc::read_spoc_file (s);
    verify (spoc::diff::diff (f1, f2) != 0);

    vector<vector<int>> fields {
        {'x', 'y', 'z'},
        {'z'},
        {'c', 'p', 'i'},
        {'r', 'g', 'b'},
        {'0', '2', '4', '6'},
        {'1', '6'},
        {'0', '1', '2', '3', '4', '5', '6', '7'}};
    for (auto header_only : {true, false})
    for (auto data_only : {true, false})
    for (auto reverse : {true, false})
    for (auto f : fields)
    {
        spoc::diff::diff (f1, f2, header_only, data_only, f, reverse);
        spoc::diff::diff (f1, f1, header_only, data_only, f, reverse);
        spoc::diff::diff (f2, f2, header_only, data_only, f, reverse);
    }
}

void test_diff_header ()
{
    using namespace std;

    spoc::spoc_file f1;
    spoc::spoc_file f2;
    verify (spoc::diff::diff (f1, f2) == 0);
    f1.set_version (1, 3);
    f2.set_version (2, 3);
    verify (spoc::diff::diff (f1, f2) != 0);
    f1.set_version (10, 11);
    f2.set_version (10, 11);
    verify (spoc::diff::diff (f1, f2) == 0);
    f1.set_version (10, 11);
    f2.set_version (10, 12);
    verify (spoc::diff::diff (f1, f2) != 0);
    f1.set_version (10, 11);
    f2.set_version (10, 11);
    verify (spoc::diff::diff (f1, f2) == 0);
    f1.set_wkt ("WKT 1");
    f2.set_wkt ("WKT 2");
    verify (spoc::diff::diff (f1, f2) != 0);
    f1.set_wkt ("WKT");
    f2.set_wkt ("WKT");
    verify (spoc::diff::diff (f1, f2) == 0);
    f1.resize (2);
    f2.resize (3);
    verify (spoc::diff::diff (f1, f2) != 0);
    f1.resize (3);
    f2.resize (3);
    verify (spoc::diff::diff (f1, f2) == 0);
}

void test_diff_fields ()
{
    using namespace std;

    // Generate some records
    const auto p = get_random_point_records (10, true);

    // Generate a spoc_file
    stringstream s;
    write_spoc_file (s, string ("Test WKT"), p);
    write_spoc_file (s, string ("Test WKT"), p);
    auto f1 = spoc::read_spoc_file (s);
    auto f2 = spoc::read_spoc_file (s);

    verify (spoc::diff::diff (f1, f2) == 0);

    auto a = f1.get (3); a.x = 123; f2.set (3, a);
    verify (spoc::diff::diff (f1, f2) != 0);
    f1.set (3, a);
    verify (spoc::diff::diff (f1, f2) == 0);

    a.y = 123; f2.set (3, a);
    verify (spoc::diff::diff (f1, f2) != 0);
    f1.set (3, a);
    verify (spoc::diff::diff (f1, f2) == 0);

    a.z = 123; f2.set (3, a);
    verify (spoc::diff::diff (f1, f2) != 0);
    f1.set (3, a);
    verify (spoc::diff::diff (f1, f2) == 0);

    a.c = 123; f2.set (3, a);
    verify (spoc::diff::diff (f1, f2) != 0);
    f1.set (3, a);
    verify (spoc::diff::diff (f1, f2) == 0);

    a.p = 123; f2.set (3, a);
    verify (spoc::diff::diff (f1, f2) != 0);
    f1.set (3, a);
    verify (spoc::diff::diff (f1, f2) == 0);

    a.i = 123; f2.set (3, a);
    verify (spoc::diff::diff (f1, f2) != 0);
    f1.set (3, a);
    verify (spoc::diff::diff (f1, f2) == 0);

    a.r = 123; f2.set (3, a);
    verify (spoc::diff::diff (f1, f2) != 0);
    f1.set (3, a);
    verify (spoc::diff::diff (f1, f2) == 0);

    a.g = 123; f2.set (3, a);
    verify (spoc::diff::diff (f1, f2) != 0);
    f1.set (3, a);
    verify (spoc::diff::diff (f1, f2) == 0);

    a.b = 123; f2.set (3, a);
    verify (spoc::diff::diff (f1, f2) != 0);
    f1.set (3, a);
    verify (spoc::diff::diff (f1, f2) == 0);

    for (size_t i = 0; i < a.extra.size (); ++i)
    {
        a.extra[i] = 123; f2.set (3, a);
        verify (spoc::diff::diff (f1, f2) != 0);
        f1.set (3, a);
        verify (spoc::diff::diff (f1, f2) == 0);
    }
}

void test_diff_individual_fields ()
{
    using namespace std;

    // Generate some records
    const auto p = get_random_point_records (10, true);

    // Generate a spoc_file
    stringstream s;
    write_spoc_file (s, string ("Test WKT"), p);
    write_spoc_file (s, string ("Test WKT"), p);
    auto f1 = spoc::read_spoc_file (s);
    auto f2 = spoc::read_spoc_file (s);

    verify (spoc::diff::diff (f1, f2) == 0);

    vector<int> fields { 'x' };
    verify (spoc::diff::diff (f1, f2, false, false, fields) == 0);

    auto a = f1.get (5); a.x = 123; f2.set (5, a);
    verify (spoc::diff::diff (f1, f2, false, false, fields) != 0);

    fields[0] = 'y';
    verify (spoc::diff::diff (f1, f2, false, false, fields) == 0);
    a.y = 123; f2.set (5, a);
    verify (spoc::diff::diff (f1, f2, false, false, fields) != 0);

    fields[0] = 'z';
    verify (spoc::diff::diff (f1, f2, false, false, fields) == 0);
    a.z = 123; f2.set (5, a);
    verify (spoc::diff::diff (f1, f2, false, false, fields) != 0);

    fields[0] = 'c';
    verify (spoc::diff::diff (f1, f2, false, false, fields) == 0);
    a.c = 123; f2.set (5, a);
    verify (spoc::diff::diff (f1, f2, false, false, fields) != 0);

    fields[0] = 'p';
    verify (spoc::diff::diff (f1, f2, false, false, fields) == 0);
    a.p = 123; f2.set (5, a);
    verify (spoc::diff::diff (f1, f2, false, false, fields) != 0);

    fields[0] = 'i';
    verify (spoc::diff::diff (f1, f2, false, false, fields) == 0);
    a.i = 123; f2.set (5, a);
    verify (spoc::diff::diff (f1, f2, false, false, fields) != 0);

    fields[0] = 'r';
    verify (spoc::diff::diff (f1, f2, false, false, fields) == 0);
    a.r = 123; f2.set (5, a);
    verify (spoc::diff::diff (f1, f2, false, false, fields) != 0);

    fields[0] = 'g';
    verify (spoc::diff::diff (f1, f2, false, false, fields) == 0);
    a.g = 123; f2.set (5, a);
    verify (spoc::diff::diff (f1, f2, false, false, fields) != 0);

    fields[0] = 'b';
    verify (spoc::diff::diff (f1, f2, false, false, fields) == 0);
    a.b = 123; f2.set (5, a);
    verify (spoc::diff::diff (f1, f2, false, false, fields) != 0);

    fields[0] = '0';
    verify (spoc::diff::diff (f1, f2, false, false, fields) == 0);
    a.extra[0] = 123; f2.set (5, a);
    verify (spoc::diff::diff (f1, f2, false, false, fields) != 0);

    // Reverse meaning
    fields[0] = '7';
    verify (spoc::diff::diff (f1, f2, false, false, fields, true) != 0);
    a.extra[7] = 123; f2.set (5, a);
    verify (spoc::diff::diff (f1, f2, false, false, fields, true) == 0);

    // Bad field name
    fields[0] = 'q';
    bool failed = false;
    try { spoc::diff::diff (f1, f2, false, false, fields); }
    catch (...) { failed = true; }
    verify (failed);
}

int main (int argc, char **argv)
{
    using namespace std;
    try
    {
        test_diff ();
        test_diff_header ();
        test_diff_fields ();
        test_diff_individual_fields ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
