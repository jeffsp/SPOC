#include "spoc/point_record.h"
#include "spoc/test_utils.h"
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <unordered_set>

using namespace std;
using namespace spoc::point;
using namespace spoc::point_record;

template<typename T>
void test_point_hash ()
{
    unordered_set<point<T>,point_hash<T>> s;
    s.insert ({0, 0, 0});
    s.insert ({1, 0, 0});
    s.insert ({0, 1, 0});
    s.insert ({0, 0, 1});
    s.insert ({0, 0, 0});
    VERIFY (s.size () == 4);
    s.insert ({1, 0, 0});
    s.insert ({0, 1, 0});
    s.insert ({0, 0, 1});
    VERIFY (s.size () == 4);
    s.insert ({1, 0, 1});
    VERIFY (s.size () == 5);
}

void test_point_distance ()
{
    point<double> a {1, 1, 1};
    point<double> b {2, 2, 2};
    double dis = distance(a, b);
    VERIFY(about_equal(dis, std::sqrt(3)));
}


int main (int argc, char **argv)
{
    try
    {
        test_point_hash<int> ();
        test_point_hash<float> ();
        test_point_distance();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
