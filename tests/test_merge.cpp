#include "merge.h"
#include "test_utils.h"
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace spoc::merge;

void test_merge ()
{
    verify (true);
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
