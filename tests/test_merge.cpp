#include "merge.h"
#include "test_utils.h"
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace spoc::merge;

void test_merge ()
{
    // Generate spoc files
    auto f1 = generate_random_spoc_file (100, 8, true);
    auto f2 = generate_random_spoc_file (100, 8, false);

    spoc::spoc_file f;
    const auto id = -1;
    const auto quiet = true;
    append (f1, f, id, quiet);
    append (f2, f, id, quiet);
    verify (f.h.total_points == 200);
    verify (f.p.size () == 200);
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
