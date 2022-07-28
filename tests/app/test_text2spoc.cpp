#include "text2spoc.h"
#include "spoc/spoc.h"
#include "spoc/test_utils.h"
#include <iostream>
#include <stdexcept>

using namespace std;

void test_text2spoc ()
{
}

int main (int argc, char **argv)
{
    try
    {
        test_text2spoc ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
