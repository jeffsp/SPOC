#include "text2spoc.h"
#include "test_utils.h"
#include <any>
#include <iostream>
#include <map>
#include <vector>
#include <stdexcept>

void test_text2spoc ()
{
}

int main (int argc, char **argv)
{
    using namespace std;
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
