#include "transform.h"
#include "test_utils.h"
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace spoc::transform;

void test_transform ()
{
}

int main (int argc, char **argv)
{
    using namespace std;
    try
    {
        test_transform ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
