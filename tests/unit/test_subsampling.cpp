#include "spoc/spoc.h"
#include "spoc/test_utils.h"
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace spoc::file;
using namespace spoc::point_record;
using namespace spoc::subsampling;

void test_subsample ()
{
    {
    spoc_file f;
    f.add (point_record (0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0));
    f.add (point_record (1.1, 1.1, 1.1, 1, 0, 0, 0, 0, 0));
    f.add (point_record (2.1, 2.1, 2.1, 2, 0, 0, 0, 0, 0));
    f.add (point_record (3.1, 3.1, 3.1, 3, 0, 0, 0, 0, 0));
    f.add (point_record (4.1, 4.1, 4.1, 4, 0, 0, 0, 0, 0));
    f.add (point_record (5.1, 5.1, 5.1, 5, 0, 0, 0, 0, 0));
    const size_t random_seed = 0;
    auto g = subsample (f, 6, random_seed);
    VERIFY (g.get_point_records ().size () == 1);
    VERIFY (g.get_point_records ().front ().c == 0);
    g = subsample (f, 1.0, random_seed);
    VERIFY (g.get_point_records ().size () == 6);
    VERIFY (g.get_point_records ().front ().c == 0);
    VERIFY (g.get_point_records ().back ().c == 5);
    g = subsample (f, 2.0, random_seed);
    VERIFY (g.get_point_records ().size () == 3);
    VERIFY (g.get_point_records ()[0].c == 0);
    VERIFY (g.get_point_records ()[1].c == 2);
    VERIFY (g.get_point_records ()[2].c == 4);
    }

    {
    spoc_file f;
    const size_t n = 1000;

    for (size_t i = 0; i < n; ++i)
    {
        // A point every 1/2 meter along the diagonal
        f.add (point_record (i, i, i, i, 0, 0, 0, 0, 0));
        const auto j = i + 0.5;
        f.add (point_record (j, j, j, i, 0, 0, 0, 0, 0));
    }

    VERIFY (f.get_point_records ().size () == n*2);

    auto g = subsample (f, 1.0, 0); // Don't randomize
    auto h = subsample (f, 1.0, 123); // Do randomize

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

int main (int argc, char **argv)
{
    try
    {
        test_subsample ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
