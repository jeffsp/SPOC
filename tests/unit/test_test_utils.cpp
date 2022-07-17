#include "contracts.h"
#include "test_utils.h"
#include <iostream>
#include <sstream>
#include <stdexcept>

using namespace std;

void test_verify ()
{
    bool failed = false;
    try
    {
        VERIFY (false);
    }
    catch (...)
    {
        failed = true;
    }
    VERIFY (failed);
}

void test_verify_throws ()
{
    VERIFY_THROWS (throw -1;)
}

int main (int argc, char **argv)
{
    try
    {
        test_verify ();
        test_verify_throws ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
