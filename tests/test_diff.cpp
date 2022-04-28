#include "diff.h"
#include "test_utils.h"
#include <iostream>
#include <stdexcept>

void test_compare ()
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
}

int main (int argc, char **argv)
{
    using namespace std;
    try
    {
        test_compare ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
