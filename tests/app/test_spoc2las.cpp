#include "spoc2las.h"
#include "spoc/spoc.h"
#include "spoc/test_utils.h"
#include <filesystem>
#include <iostream>
#include <stdexcept>

using namespace std;

void test_spoc2las ()
{
    LASheader lasheader;

    // LASlib will add the extension ".txt" if you don't include one
    const string fn = generate_tmp_filename () + ".las";

    // Open the las file for writing
    spoc::spoc2las_app::las_writer w (fn, lasheader);

    // Cleanup
    std::filesystem::remove (fn);
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
