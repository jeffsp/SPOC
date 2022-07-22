#include "diff.h"
#include "test_utils.h"
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace spoc::diff_app;
using namespace spoc::header;
using namespace spoc::io;
using namespace spoc::point_record;

void test_diff ()
{
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
        diff (f1, f2, header_only, data_only, f, reverse);
        diff (f1, f1, header_only, data_only, f, reverse);
        diff (f2, f2, header_only, data_only, f, reverse);
    }
}

void test_diff_header ()
{
    {
    spoc_file f1;
    spoc_file f2;
    VERIFY (diff (f1, f2) == 0);
    f1.resize (2);
    f2.resize (3);
    VERIFY (diff (f1, f2) != 0);
    f1.resize (3);
    f2.resize (3);
    VERIFY (diff (f1, f2) == 0);
    }

    {
    header h1;
    header h2;
    h1.major_version = 1;
    h2.major_version = 2;
    spoc_file f1 (h1, point_records ());
    spoc_file f2 (h2, point_records ());
    VERIFY (diff (f1, f2) != 0);
    }

    {
    header h1;
    header h2;
    h1.minor_version = 1;
    h2.minor_version = 2;
    spoc_file f1 (h1, point_records ());
    spoc_file f2 (h2, point_records ());
    VERIFY (diff (f1, f2) != 0);
    }

    {
    header h1 ("A", 0, 0, false);
    header h2 ("B", 0, 0, false);
    spoc_file f1 (h1, point_records ());
    spoc_file f2 (h2, point_records ());
    VERIFY (diff (f1, f2) != 0);
    }
}

void test_diff_fields ()
{
    // Generate spoc files
    auto f1 = generate_random_spoc_file (100, 5, true);
    auto f2 (f1);

    VERIFY (diff (f1, f2) == 0);

    f1[0].x = f1[0].x + 1;
    VERIFY (diff (f1, f2) != 0);
    f2[0].x = f1[0].x;
    VERIFY (diff (f1, f2) == 0);

    f1[0].y = f1[0].y + 1;
    VERIFY (diff (f1, f2) != 0);
    f2[0].y = f1[0].y;
    VERIFY (diff (f1, f2) == 0);

    f1[0].z = f1[0].z + 1;
    VERIFY (diff (f1, f2) != 0);
    f2[0].z = f1[0].z;
    VERIFY (diff (f1, f2) == 0);

    f1[0].c = f1[0].c + 1;
    VERIFY (diff (f1, f2) != 0);
    f2[0].c = f1[0].c;
    VERIFY (diff (f1, f2) == 0);

    f1[0].p = f1[0].p + 1;
    VERIFY (diff (f1, f2) != 0);
    f2[0].p = f1[0].p;
    VERIFY (diff (f1, f2) == 0);

    f1[0].i = f1[0].i + 1;
    VERIFY (diff (f1, f2) != 0);
    f2[0].i = f1[0].i;
    VERIFY (diff (f1, f2) == 0);

    f1[0].r = f1[0].r + 1;
    VERIFY (diff (f1, f2) != 0);
    f2[0].r = f1[0].r;
    VERIFY (diff (f1, f2) == 0);

    f1[0].g = f1[0].g + 1;
    VERIFY (diff (f1, f2) != 0);
    f2[0].g = f1[0].g;
    VERIFY (diff (f1, f2) == 0);

    f1[0].b = f1[0].b + 1;
    VERIFY (diff (f1, f2) != 0);
    f2[0].b = f1[0].b;
    VERIFY (diff (f1, f2) == 0);

    for (size_t i = 0; i < f1[0].extra.size (); ++i)
    {
        f1[0].extra[i] = f1[0].extra[i] + 1;
        VERIFY (diff (f1, f2) != 0);
        f2[0].extra[i] = f1[0].extra[i];
        VERIFY (diff (f1, f2) == 0);
    }

    // Generate spoc files
    auto f3 = generate_random_spoc_file (100, 5, true);
    auto f4 (f3);
    f4.resize_extra (6);
    VERIFY (diff (f3, f4) != 0);
}

void test_diff_individual_fields ()
{
    // Generate spoc files
    auto f1 = generate_random_spoc_file (100, 5, true);
    auto f2 (f1);

    VERIFY (diff (f1, f2) == 0);

    vector<int> fields { 'x' };
    VERIFY (diff (f1, f2, false, false, fields) == 0);

    f1[0].x = f1[0].x + 1;
    VERIFY (diff (f1, f2, false, false, fields) != 0);
    // Reverse meaning
    VERIFY (diff (f1, f2, false, false, fields, true) == 0);
    f2[0].x = f1[0].x;
    VERIFY (diff (f1, f2, false, false, fields) == 0);
    // Reverse meaning
    VERIFY (diff (f1, f2, false, false, fields, true) != 0);

    // Different field
    f2 = generate_random_spoc_file (100, 5, true, false, 456);
    for (auto f : {'y', 'z', 'c', 'p', 'i', 'r', 'g', 'b', '0', '4' })
    {
        fields[0] = f;
        f1[0].x = f1[0].x + 1;
        VERIFY (diff (f1, f2, false, false, fields) != 0);
    }

    // Bad field name
    fields[0] = 'q';
    VERIFY_THROWS ({ diff (f1, f2, false, false, fields); })
}

int main (int argc, char **argv)
{
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
