#include "tile.h"
#include "test_utils.h"
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace spoc::tile;

void test_get_tile_size ()
{
    {
    const vector<double> x { 1, 2, 3 };
    const vector<double> y { 4, 5, 6 };
    verify (about_equal (get_tile_size (x, y, 1), 2.0));
    verify (about_equal (get_tile_size (x, y, 2), 1.0));
    verify (about_equal (get_tile_size (x, y, 3), 0.66667));
    }
}

int main (int argc, char **argv)
{
    using namespace std;
    try
    {
        test_get_tile_size ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
