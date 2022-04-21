#include "spoc2text.h"
#include "test_utils.h"
#include <iostream>
#include <stdexcept>

void test_spoc2text ()
{
}

int main (int argc, char **argv)
{
    using namespace std;
    try
    {
        test_spoc2text ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
