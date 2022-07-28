#include "spoc2text.h"
#include "spoc/spoc.h"
#include "spoc/test_utils.h"
#include <iostream>
#include <stdexcept>

using namespace std;

void test_spoc2text ()
{
}

int main (int argc, char **argv)
{
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
