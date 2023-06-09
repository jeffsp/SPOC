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
    point_records p;
    p.push_back (point_record (0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0));
    p.push_back (point_record (1.1, 1.1, 1.1, 1, 0, 0, 0, 0, 0));
    p.push_back (point_record (2.1, 2.1, 2.1, 2, 0, 0, 0, 0, 0));
    p.push_back (point_record (3.1, 3.1, 3.1, 3, 0, 0, 0, 0, 0));
    p.push_back (point_record (4.1, 4.1, 4.1, 4, 0, 0, 0, 0, 0));
    p.push_back (point_record (5.1, 5.1, 5.1, 5, 0, 0, 0, 0, 0));
    const size_t random_seed = 0;
    auto ind = get_subsample_indexes (p, 6, random_seed);
    VERIFY (ind.size () == 1);
    VERIFY (p[ind.front ()].c == 0);
    ind = get_subsample_indexes (p, 1.0, random_seed);
    VERIFY (ind.size () == 6);
    VERIFY (p[ind.front ()].c == 0);
    VERIFY (p[ind.back ()].c == 5);
    ind = get_subsample_indexes (p, 2.0, random_seed);
    VERIFY (ind.size () == 3);
    VERIFY (p[ind[0]].c == 0);
    VERIFY (p[ind[1]].c == 2);
    VERIFY (p[ind[2]].c == 4);
    }

    {
    point_records p;
    const size_t n = 1000;

    for (size_t i = 0; i < n; ++i)
    {
        // A point every 1/2 meter along the diagonal
        p.push_back (point_record (i, i, i, i, 0, 0, 0, 0, 0));
        const auto j = i + 0.5;
        p.push_back (point_record (j, j, j, i, 0, 0, 0, 0, 0));
    }

    VERIFY (p.size () == n*2);

    auto ind1 = get_subsample_indexes (p, 1.0, 0); // Don't randomize
    auto ind2 = get_subsample_indexes (p, 1.0, 123); // Do randomize

    // Half are gone
    VERIFY (ind1.size () == n);
    VERIFY (ind2.size () == n);

    // They should not have been selected randomly
    VERIFY (p[ind1.front ()].c == 0);
    VERIFY (p[ind1.back ()].c == n - 1);

    // They should have been selected randomly
    VERIFY (p[ind2.front ()].c != 0);
    VERIFY (p[ind2.back ()].c != n - 1);
    }

    {
    // Test subsample points function
    point_records p;
    p.push_back (point_record (0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0));
    p.push_back (point_record (1.1, 0.0, 0.0, 1, 0, 0, 0, 0, 0));
    p.push_back (point_record (2.1, 0.0, 0.0, 2, 0, 0, 0, 0, 0));
    p.push_back (point_record (3.1, 0.0, 0.0, 3, 0, 0, 0, 0, 0));
    const size_t random_seed = 0;
    // Subsample at resolution of 2. This should give us 2 filled voxels
    auto p_sub = subsample_points (p, 2, random_seed);
    VERIFY (p_sub.size () == 2);
    VERIFY (p_sub[0].c == 0);
    VERIFY (p_sub[1].c == 2);
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
