#include "tile.h"
#include "test_utils.h"
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace spoc::tile_app;

void test_get_tile_size ()
{
    {
        const vector<double> empty { };
        const vector<double> x { 1, 2, 3 };
        const vector<double> y { 4, 5, 6 };
        VERIFY_THROWS (get_tile_size (empty, y, 1);)
        VERIFY_THROWS (get_tile_size (x, empty, 1);)
    }

    {
    const vector<double> x { 1, 2, 3 };
    const vector<double> y { 4, 5, 6 };
    VERIFY (about_equal (get_tile_size (x, y, 1), 2.0));
    VERIFY (about_equal (get_tile_size (x, y, 2), 1.0));
    VERIFY (about_equal (get_tile_size (x, y, 3), 0.66667));
    }

    {
    const vector<double> x { 1, 2, 2.9999 };
    const vector<double> y { 4, 5, 5.9999 };
    VERIFY (about_equal (get_tile_size (x, y, 1), 2.0));
    VERIFY (about_equal (get_tile_size (x, y, 2), 1.0));
    VERIFY (about_equal (get_tile_size (x, y, 3), 0.66667));
    }

    {
    const vector<double> x { 1, 2, 3 };
    const vector<double> y { 4, 5, 5 };
    VERIFY (about_equal (get_tile_size (x, y, 1), 2.0));
    VERIFY (about_equal (get_tile_size (x, y, 2), 1.0));
    VERIFY (about_equal (get_tile_size (x, y, 3), 0.66667));
    }
}

void test_get_tile_indexes ()
{
    {
    const vector<double> x { 1, 2, 3, 1, 2, 3, 1, 2, 3 };
    const vector<double> y { 4, 4, 4, 5, 5, 5, 6, 6, 6 };
    auto ind = get_tile_indexes (x, y, 1);
    vector<size_t> answer { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
    VERIFY (ind == answer);
    // 2X2 tiling
    ind = get_tile_indexes (x, y, 2);
    VERIFY (*max_element (ind.begin (), ind.end ()) == 3);
    ind = get_tile_indexes (x, y, 0.1);
    VERIFY (ind.size () == x.size ());
    // Lots of tiny tiles
    VERIFY (*max_element (ind.begin (), ind.end ()) > 400);
    }

    {
    const vector<double> x { 1, 2, 2.9999, 1, 2, 2.9999, 1, 2, 2.9999 };
    const vector<double> y { 4, 4, 4, 5, 5, 5, 4.5, 4.5, 5 };
    auto ind = get_tile_indexes (x, y, 1);
    vector<size_t> answer { 0, 1, 1, 2, 3, 3, 0, 1, 3 };
    VERIFY (ind == answer);
    }
}

void test_get_tile_map ()
{
    // 3X3 tiling
    const vector<double> x { 1, 2, 3, 1, 2, 3, 1, 2, 3 };
    const vector<double> y { 4, 4, 4, 5, 5, 5, 6, 6, 6 };
    auto ind = get_tile_indexes (x, y, 1);
    auto m = get_tile_map (ind);
    // Each point goes to its own tile
    for (size_t i = 0; i < 9; ++i)
    {
        VERIFY (m[i].size () == 1);
        VERIFY (m[i][0] == i);
    }

    // 2X2 tiling
    ind = get_tile_indexes (x, y, 2);
    m = get_tile_map (ind);
    VERIFY (m[0].size () == 4);
    VERIFY (m[1].size () == 2);
    VERIFY (m[2].size () == 2);
    VERIFY (m[3].size () == 1);

    // Lots of little tiles
    ind = get_tile_indexes (x, y, 0.1);
    m = get_tile_map (ind);
    for (auto i : m)
        VERIFY (i.second.size () == 1);
}

int main (int argc, char **argv)
{
    try
    {
        test_get_tile_size ();
        test_get_tile_indexes ();
        test_get_tile_map ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
