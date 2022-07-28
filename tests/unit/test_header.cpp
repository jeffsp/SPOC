#include "spoc/app_utils.h"
#include "spoc/header.h"
#include "spoc/test_utils.h"
#include <iostream>
#include <random>
#include <vector>
#include <stdexcept>

using namespace std;
using namespace spoc::app_utils;
using namespace spoc::header;

void test_header ()
{
    {
    header h;
    VERIFY (h.is_valid ());
    }

    {
    for (size_t i = 0; i < 4; ++i)
    {
        header h;
        h.signature[i] = '!';
        VERIFY (!h.is_valid ());
    }
    }

    {
    header h;
    h.wkt = "Test WKT";
    VERIFY (h.check_signature ());
    stringstream s;
    write_header (s, h);

    const auto g = read_header (s);

    VERIFY (h == g);
    }

    // Fail when reading signature
    {
    stringstream s;
    // Write an invalid signature
    s << "SPOX" << endl;
    VERIFY_THROWS (read_header (s);)
    }

    // Fail when writing
    {
    stringstream s;
    header h;
    h.signature[3] = 'X';
    VERIFY_THROWS (write_header (s, h);)
    }
}

int main (int argc, char **argv)
{
    try
    {
        test_header ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
