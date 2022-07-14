#include "spoc2las.h"
#include "test_utils.h"
#include <iostream>
#include <stdexcept>

void test_spoc2las ()
{
    const std::string fn = std::tmpnam (nullptr);

    LASheader lasheader;
    spoc::spoc2las::las_writer w (fn, lasheader);
}

int main (int argc, char **argv)
{
    using namespace std;
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
