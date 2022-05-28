#include "merge.h"
#include "test_utils.h"
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace spoc::merge;

void test_merge ()
{
    spoc::header h1 ("A", 0, 0);
    spoc::header h2 ("B", 0, 0);
    spoc::spoc_file f1 (h1, spoc::point_records ());
    spoc::spoc_file f2 (h2, spoc::point_records ());

    spoc::spoc_file f;
    const auto id = -1;
    const auto quiet = false;
    std::stringstream ss;
    append (f1, f, id, quiet, ss);
    append (f2, f, id, quiet, ss);
}

void test_merge_quiet ()
{
    // Generate spoc files
    auto f1 = generate_random_spoc_file (100, 8, true);
    auto f2 = generate_random_spoc_file (100, 8, false);

    spoc::spoc_file f;
    const auto id = -1;
    const auto quiet = true;
    append (f1, f, id, quiet);
    append (f2, f, id, quiet);
    verify (f.get_header ().total_points == 200);
    verify (f.get_point_records ().size () == 200);
}

int main (int argc, char **argv)
{
    try
    {
        test_merge ();
        test_merge_quiet ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
