#include "spoc/point_record.h"
#include "spoc/test_utils.h"
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

int main (int argc, char **argv)
{
    try
    {
        test_point_hash<int> ();
        test_point_hash<float> ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
