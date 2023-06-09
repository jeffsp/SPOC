#include "spoc/test_utils.h"
#include "spoc/voxel.h"
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace spoc::point;
using namespace spoc::voxel;

using P = vector<point<double>>;

void test_empty ()
{
    P pc;
    auto indexes = get_voxel_indexes (pc, 1.0);
    VERIFY (indexes.empty());
}

void test_single ()
{
    P pc;
    pc.push_back({0, 0, 0});
    auto v = get_voxel_indexes (pc, 1.0);
    VERIFY (v[0].i == 0);
    VERIFY (v[0].j == 0);
    VERIFY (v[0].k == 0);
}

void test_res ()
{
    P pc;
    pc.push_back({0, 0, 0});
    pc.push_back({4, 1, 1});
    auto v = get_voxel_indexes(pc, 1.5);
    VERIFY (v[1].i == 2);
    VERIFY (v[1].j == 0);
    VERIFY (v[1].k == 0);
}

void test_neg ()
{
    P pc;
    pc.push_back({-1, -1, -1});
    pc.push_back({1, 1, 1});
    auto v = get_voxel_indexes (pc, 1.0);
    VERIFY (v[1].i == 2);
    VERIFY (v[1].j == 2);
    VERIFY (v[1].k == 2);
}

void test_multiple ()
{
    P pc;
    pc.push_back({0, 0, 0});
    pc.push_back({1, 1, 1});
    pc.push_back({0, 1, 2});
    auto v = get_voxel_indexes (pc, 1.0);
    VERIFY (v.at(0).i == 0 && v.at(0).j == 0 && v.at(0).k == 0);
    VERIFY (v.at(1).i == 1 && v.at(1).j == 1 && v.at(1).k == 1);
    VERIFY (v.at(2).i == 0 && v.at(2).j == 1 && v.at(2).k == 2);
}

void test_vis ()
{
    P pc;
    pc.push_back({0, 1, 2});
    pc.push_back({3, 4, 5});
    pc.push_back({6, 7, 8});
    auto v1 = get_voxel_indexes (pc, 2.1); // {0,0,0} {1,1,1} {2,2,2}
    auto v2 = get_voxel_indexes (pc, 0.99); // {0,0,0} {3,3,3} {6,6,6}
    auto vis1 = get_voxel_index_set (v1);
    auto vis2 = get_voxel_index_set (v2);
    VERIFY (vis1.size () == 3);
    VERIFY (vis2.size () == 3);
    VERIFY (vis1.find ({0, 0, 0}) != vis1.end ());
    VERIFY (vis1.find ({1, 1, 1}) != vis1.end ());
    VERIFY (vis1.find ({2, 2, 2}) != vis1.end ());
    VERIFY (vis1.find ({1, 2, 1}) == vis1.end ());
    VERIFY (vis2.find ({0, 0, 0}) != vis2.end ());
    VERIFY (vis2.find ({3, 3, 3}) != vis2.end ());
    VERIFY (vis2.find ({6, 6, 6}) != vis2.end ());
    VERIFY (vis2.find ({0, 0, 3}) == vis2.end ());
}

void test_vim ()
{
    P pc;
    pc.push_back({0, 0, 0});
    pc.push_back({1, 1, 1});
    pc.push_back({2, 2, 2});
    auto v1 = get_voxel_indexes (pc, 1.5); // {0,0,0} {0,0,0} {1,1,1}
    auto v2 = get_voxel_indexes (pc, 0.99); // {0,0,0} {1,1,1} {2,2,2}
    auto vim1 = get_voxel_index_map (v1);
    auto vim2 = get_voxel_index_map (v2);
    VERIFY (vim1.at ({0, 0, 0}).size() == 2);
    VERIFY (vim2.at ({1, 1, 1}).size() == 1);
    VERIFY (vim1.size () == 2);
    VERIFY (vim2.size () == 3);
    VERIFY (vim1.find ({0, 0, 0}) != vim1.end ());
    VERIFY (vim1.find ({1, 1, 1}) != vim1.end ());
    VERIFY (vim1.find ({2, 2, 2}) == vim1.end ());
    VERIFY (vim1.find ({1, 2, 1}) == vim1.end ());
    VERIFY (vim2.find ({0, 0, 0}) != vim2.end ());
    VERIFY (vim2.find ({1, 1, 1}) != vim2.end ());
    VERIFY (vim2.find ({2, 2, 2}) != vim2.end ());
    VERIFY (vim2.find ({0, 0, 3}) == vim2.end ());
}

void test_get_voxel_indexes_w_indexes ()
{
    P pc;
    pc.push_back({0, 0, 0});
    pc.push_back({1, 1, 1});
    pc.push_back({2, 2, 2});
    // Don't use the last point
    std::vector<int> idxs {0, 1};
    // Should end up with {0, 0, 0}, {1, 1, 1}
    auto v1 = get_voxel_indexes (pc, idxs, 0.99); // {0,0,0} {1,1,1}
    VERIFY (v1[0] == voxel_index(0, 0, 0));
    VERIFY (v1[1] == voxel_index(1, 1, 1));
    VERIFY (v1.size() == 2);
}



int main ()
{
    try
    {
        test_empty ();
        test_single ();
        test_res ();
        test_neg ();
        test_multiple ();
        test_vis ();
        test_vim ();

        return 0;
    }
    catch (const exception &e)
    {
        cerr << "exception: " << e.what () << endl;
    }
    return -1;
}
