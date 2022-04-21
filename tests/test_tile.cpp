#include "tile.h"
#include "test_utils.h"
#include <iostream>
#include <stdexcept>

void test_tile ()
{
}

int main (int argc, char **argv)
{
    using namespace std;
    try
    {
        test_tile ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
