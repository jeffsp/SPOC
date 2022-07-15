#include "spoc2las.h"
#include "test_utils.h"
#include <iostream>
#include <stdexcept>

using namespace std;

void test_spoc2las ()
{
    const string fn = tmpnam (nullptr);

    LASheader lasheader;
    spoc::spoc2las_app::las_writer w (fn, lasheader);
}

int main (int argc, char **argv)
{
    try
    {
        test_spoc2las ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
