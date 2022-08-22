#include "diff.h"
#include "spoc/spoc.h"
#include "spoc/test_utils.h"
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace spoc::diff_app;
using namespace spoc::file;
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
    spoc_file f1 (1, 1, "WKT");
    spoc_file f2 (1, 2, "WKT");
    VERIFY (diff (f1, f2) != 0);
    }

    {
    spoc_file f1 (1, 2, "WKT");
    spoc_file f2 (1, 1, "WKT");
    VERIFY (diff (f1, f2) != 0);
    }

    {
    spoc_file f1 (1, 2, "WKT1");
    spoc_file f2 (1, 2, "WKT2");
    VERIFY (diff (f1, f2) != 0);
    }
}

void test_diff_fields ()
{
    // Generate spoc files
    const size_t total_points = 100;
    const size_t extra_fields = 5;
    auto f1 = generate_random_spoc_file (total_points, extra_fields);
    auto f2 (f1);

    VERIFY (diff (f1, f2) == 0);

    auto prs1 = f1.get_point_records ();
    auto prs2 = f2.get_point_records ();

    prs1[0].x = prs1[0].x + 1;
    f1.set_point_records (prs1);
    VERIFY (diff (f1, f2) != 0);
    f1.set_point_records (prs1 = prs2);
    VERIFY (diff (f1, f2) == 0);

    prs1[0].y = prs1[0].y + 1;
    f1.set_point_records (prs1);
    VERIFY (diff (f1, f2) != 0);
    f1.set_point_records (prs1 = prs2);
    VERIFY (diff (f1, f2) == 0);

    prs1[0].z = prs1[0].z + 1;
    f1.set_point_records (prs1);
    VERIFY (diff (f1, f2) != 0);
    f1.set_point_records (prs1 = prs2);
    VERIFY (diff (f1, f2) == 0);

    prs1[0].c = prs1[0].c + 1;
    f1.set_point_records (prs1);
    VERIFY (diff (f1, f2) != 0);
    f1.set_point_records (prs1 = prs2);
    VERIFY (diff (f1, f2) == 0);

    prs1[0].p = prs1[0].p + 1;
    f1.set_point_records (prs1);
    VERIFY (diff (f1, f2) != 0);
    f1.set_point_records (prs1 = prs2);
    VERIFY (diff (f1, f2) == 0);

    prs1[0].i = prs1[0].i + 1;
    f1.set_point_records (prs1);
    VERIFY (diff (f1, f2) != 0);
    f1.set_point_records (prs1 = prs2);
    VERIFY (diff (f1, f2) == 0);

    prs1[0].r = prs1[0].r + 1;
    f1.set_point_records (prs1);
    VERIFY (diff (f1, f2) != 0);
    f1.set_point_records (prs1 = prs2);
    VERIFY (diff (f1, f2) == 0);

    prs1[0].g = prs1[0].g + 1;
    f1.set_point_records (prs1);
    VERIFY (diff (f1, f2) != 0);
    f1.set_point_records (prs1 = prs2);
    VERIFY (diff (f1, f2) == 0);

    prs1[0].b = prs1[0].b + 1;
    f1.set_point_records (prs1);
    VERIFY (diff (f1, f2) != 0);
    f1.set_point_records (prs1 = prs2);
    VERIFY (diff (f1, f2) == 0);

    for (size_t i = 0; i < prs1[0].extra.size (); ++i)
    {
        prs1[0].extra[i] = prs1[0].extra[i] + 1;
        f1.set_point_records (prs1);
        VERIFY (diff (f1, f2) != 0);
        f1.set_point_records (prs1 = prs2);
        VERIFY (diff (f1, f2) == 0);
    }

    // Generate spoc files
    auto f3 = generate_random_spoc_file (100, 5, true);
    auto f4 (f3);
    f4.resize_extra_fields (6);
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

    auto prs1 = f1.get_point_records ();
    auto prs2 = f2.get_point_records ();

    prs1[0].x = prs1[0].x + 1;
    f1.set_point_records (prs1);
    VERIFY (diff (f1, f2, false, false, fields) != 0);

    // Reverse meaning
    VERIFY (diff (f1, f2, false, false, fields, true) == 0);
    f1.set_point_records (prs2);
    VERIFY (diff (f1, f2, false, false, fields) == 0);
    // Reverse meaning
    VERIFY (diff (f1, f2, false, false, fields, true) != 0);

    // Different field
    f2 = generate_random_spoc_file (100, 5, true, false, 456);
    for (auto f : {'y', 'z', 'c', 'p', 'i', 'r', 'g', 'b', '0', '4' })
    {
        fields[0] = f;
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
