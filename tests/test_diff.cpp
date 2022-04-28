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

void test_diff_fields ()
{
    using namespace std;

    // Generate some records
    const auto p = get_random_point_records (100, true);
    const auto q (p);

    // Generate a spoc_file
    stringstream s;
    write_spoc_file (s, string ("Text WKT"), p);
    write_spoc_file (s, string ("Text WKT"), q);
    auto f1 = spoc::read_spoc_file (s);
    auto f2 = spoc::read_spoc_file (s);

    verify (spoc::diff::diff (f1, f2) == 0);
    auto p1 = f1.get (0); p1.z = 123;
    auto p2 = p1;
    p2.z = 123;
    f2.set (0, p2);
    verify (spoc::diff::diff (f1, f2) != 0);
}

int main (int argc, char **argv)
{
    using namespace std;
    try
    {
        test_diff ();
        test_diff_fields ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
