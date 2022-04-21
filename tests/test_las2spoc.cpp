#include "las2spoc.h"
#include "test_utils.h"
#include <iostream>
#include <stdexcept>

void test_las2spoc ()
{
}

int main (int argc, char **argv)
{
    using namespace std;
    try
    {
        test_las2spoc ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
