#include "info.h"
#include "test_utils.h"
#include <any>
#include <iostream>
#include <map>
#include <vector>
#include <stdexcept>

void test_info ()
{
}

int main (int argc, char **argv)
{
    using namespace std;
    try
    {
        test_info ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
