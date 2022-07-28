#include "spoc/contracts.h"
#include "spoc/test_utils.h"
#include <iostream>
#include <sstream>
#include <stdexcept>

using namespace std;

void test_contracts ()
{
#ifndef NDEBUG
    // Debug build
    REQUIRE (true);
    ENSURE (true);

    VERIFY_THROWS (REQUIRE (false);)
    VERIFY_THROWS (ENSURE (false);)
#else
    // Release build
    //
    // Contracts are only valid on DEBUG builds
    REQUIRE (false);
    ENSURE (false);
#endif
}

int main (int argc, char **argv)
{
    try
    {
        test_contracts ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
