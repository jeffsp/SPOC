#include "tile.h"
#include "spoc/spoc.h"
#include "spoc/test_utils.h"
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace spoc::extent;
using namespace spoc::point;
using namespace spoc::tile_app;

void test_get_tile_size ()
{
    {
    vector<point<double>> p {{1, 4, 0}, {2, 5, 0}, {3, 6, 0}};
    const auto e = get_extent (p);
    VERIFY (about_equal (get_tile_size (e, 1), 2.0));
    VERIFY (about_equal (get_tile_size (e, 2), 1.0));
    VERIFY (about_equal (get_tile_size (e, 3), 0.66667));
    }

    {
    vector<point<double>> p {{1, 4, 0}, {2, 5, 0}, {2.9999, 5.9999, 0}};
    const auto e = get_extent (p);
    VERIFY (about_equal (get_tile_size (e, 1), 2.0));
    VERIFY (about_equal (get_tile_size (e, 2), 1.0));
    VERIFY (about_equal (get_tile_size (e, 3), 0.66667));
    }

    {
    vector<point<double>> p {{1, 4, 0}, {2, 5, 0}, {3, 5, 0}};
    const auto e = get_extent (p);
    VERIFY (about_equal (get_tile_size (e, 1), 2.0));
    VERIFY (about_equal (get_tile_size (e, 2), 1.0));
    VERIFY (about_equal (get_tile_size (e, 3), 0.66667));
    }
}

void test_get_tile_indexes ()
{
    {
    vector<point<double>> p {
        {1, 4, 0},
        {2, 4, 0},
        {3, 4, 0},
        {1, 5, 0},
        {2, 5, 0},
        {3, 5, 0},
        {1, 6, 0},
        {2, 6, 0},
        {3, 6, 0},
    };
    const auto e = get_extent (p);
    auto ind = get_tile_indexes (p, e, 1.0, 1.0);
    vector<size_t> answer { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
    VERIFY (ind == answer);
    // 2X2 tiling
    ind = get_tile_indexes (p, e, 2, 2);
    VERIFY (*max_element (ind.begin (), ind.end ()) == 3);
    ind = get_tile_indexes (p, e, 0.1, 0.1);
    VERIFY (ind.size () == p.size ());
    // Lots of tiny tiles
    VERIFY (*max_element (ind.begin (), ind.end ()) > 400);
    }

    {
    vector<point<double>> p {
        {1, 4, 0},
        {2, 4, 0},
        {2.9999, 4, 0},
        {1, 5, 0},
        {2, 5, 0},
        {2.9999, 5, 0},
        {1, 4.5, 0},
        {2, 4.5, 0},
        {2.9999, 5, 0},
    };
    const auto e = get_extent (p);
    auto ind = get_tile_indexes (p, e, 1, 1);
    vector<size_t> answer { 0, 1, 1, 2, 3, 3, 0, 1, 3 };
    VERIFY (ind == answer);
    }
}

void test_get_tile_indexes_xy ()
{
    vector<point<double>> p {
        {1, 4, 0},
        {2, 4, 0},
        {3, 4, 0},
        {1, 5, 0},
        {2, 5, 0},
        {3, 5, 0},
        {1, 6, 0},
        {2, 6, 0},
        {3, 6, 0},
    };
    const auto e = get_extent (p);

    auto ind = get_tile_indexes (p, e, 1.0, 0.0);
    vector<size_t> answer1 { 0, 1, 2, 0, 1, 2, 0, 1, 2 };
    VERIFY (ind == answer1);

    ind = get_tile_indexes (p, e, -1.0, 1.0);
    vector<size_t> answer2 { 0, 0, 0, 1, 1, 1, 2, 2, 2 };
    VERIFY (ind == answer2);
}


void test_get_tile_map ()
{
    // 3X3 tiling
    vector<point<double>> p {
        {1, 4, 0},
        {2, 4, 0},
        {3, 4, 0},
        {1, 5, 0},
        {2, 5, 0},
        {3, 5, 0},
        {1, 6, 0},
        {2, 6, 0},
        {3, 6, 0},
    };
    const auto e = get_extent (p);
    auto ind = get_tile_indexes (p, e, 1.0, 1.0);
    auto m = get_tile_map (ind);
    // Each point goes to its own tile
    for (size_t i = 0; i < 9; ++i)
    {
        VERIFY (m[i].size () == 1);
        VERIFY (m[i][0] == i);
    }

    // 2X2 tiling
    ind = get_tile_indexes (p, e, 2.0, 2.0);
    m = get_tile_map (ind);
    VERIFY (m[0].size () == 4);
    VERIFY (m[1].size () == 2);
    VERIFY (m[2].size () == 2);
    VERIFY (m[3].size () == 1);

    // Lots of little tiles
    ind = get_tile_indexes (p, e, 0.1, 0.1);
    m = get_tile_map (ind);
    for (auto i : m)
        VERIFY (i.second.size () == 1);
}

void test_get_tile_map_xy ()
{
    // 3X1 tiling
    vector<point<double>> p {
        {1, 4, 0},
        {2, 4, 0},
        {3, 4, 0},
        {1, 5, 0},
        {2, 5, 0},
        {3, 5, 0},
        {1, 6, 0},
        {2, 6, 0},
        {3, 6, 0},
    };
    const auto e = get_extent (p);
    auto ind = get_tile_indexes (p, e, 1.0, 0.0);
    auto m = get_tile_map (ind);
    for (auto i : m)
    {
        // Three vertical slices of three points
        VERIFY (i.second.size () == 3);
        VERIFY (i.second[0] == i.first);
    }
    ind = get_tile_indexes (p, e, 0.0, 1.0);
    m = get_tile_map (ind);
    for (auto i : m)
    {
        // Three horizontal slices of three points
        VERIFY (i.second.size () == 3);
        VERIFY (i.second[0] == i.first * 3);
    }
}

int main (int argc, char **argv)
{
    try
    {
        test_get_tile_size ();
        test_get_tile_indexes ();
        test_get_tile_indexes_xy ();
        test_get_tile_map ();
        test_get_tile_map_xy ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
