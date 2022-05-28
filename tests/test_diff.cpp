#include "diff.h"
#include "test_utils.h"
#include <iostream>
#include <stdexcept>

void test_diff ()
{
    using namespace std;

    // Generate spoc files
    auto f1 = generate_random_spoc_file (100, 5, true);
    auto f2 = generate_random_spoc_file (100, 5, false);

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

    // Generate spoc files
    auto f1 = generate_random_spoc_file (100, 5, true);
    auto f2 (f1);

    verify (spoc::diff::diff (f1, f2) == 0);

    f1[0].x = f1[0].x + 1;
    verify (spoc::diff::diff (f1, f2) != 0);
    f2[0].x = f1[0].x;
    verify (spoc::diff::diff (f1, f2) == 0);

    f1[0].y = f1[0].y + 1;
    verify (spoc::diff::diff (f1, f2) != 0);
    f2[0].y = f1[0].y;
    verify (spoc::diff::diff (f1, f2) == 0);

    f1[0].z = f1[0].z + 1;
    verify (spoc::diff::diff (f1, f2) != 0);
    f2[0].z = f1[0].z;
    verify (spoc::diff::diff (f1, f2) == 0);

    for (size_t i = 0; i < f1[0].extra.size (); ++i)
    {
        f1[0].extra[i] = f1[0].extra[i] + 1;
        verify (spoc::diff::diff (f1, f2) != 0);
        f2[0].extra[i] = f1[0].extra[i];
        verify (spoc::diff::diff (f1, f2) == 0);
    }
}

void test_diff_individual_fields ()
{
    using namespace std;

    // Generate spoc files
    auto f1 = generate_random_spoc_file (100, 5, true);
    auto f2 (f1);

    verify (spoc::diff::diff (f1, f2) == 0);

    vector<int> fields { 'x' };
    verify (spoc::diff::diff (f1, f2, false, false, fields) == 0);

    f1[0].x = f1[0].x + 1;
    verify (spoc::diff::diff (f1, f2, false, false, fields) != 0);
    // Reverse meaning
    verify (spoc::diff::diff (f1, f2, false, false, fields, true) == 0);
    f2[0].x = f1[0].x;
    verify (spoc::diff::diff (f1, f2, false, false, fields) == 0);
    // Reverse meaning
    verify (spoc::diff::diff (f1, f2, false, false, fields, true) != 0);

    // Different field
    fields[0] = 'y';
    f1[0].x = f1[0].x + 1;
    verify (spoc::diff::diff (f1, f2, false, false, fields) == 0);

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
