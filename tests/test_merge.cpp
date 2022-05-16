#include "merge.h"
#include "test_utils.h"
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace spoc::merge;

void test_merge ()
{
    // Generate spoc files
    const auto p1 = get_random_point_records (100);
    stringstream s1;
    write_spoc_file (s1, string ("Text WKT"), p1);
    auto f1 = spoc::read_spoc_file (s1);

    const auto p2 = get_random_point_records (100);
    stringstream s2;
    write_spoc_file (s2, string ("Text WKT"), p2);
    auto f2 = spoc::read_spoc_file (s2);

    spoc::spoc_file f;
    const auto id = -1;
    const auto quiet = true;
    append (f1, f, id, quiet);
    append (f2, f, id, quiet);
    verify (f.get_npoints () == 200);
}

int main (int argc, char **argv)
{
    try
    {
        test_merge ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
