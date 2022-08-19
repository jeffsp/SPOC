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
    // Test is_valid
    {
    header h;
    VERIFY (h.is_valid ());
    }

    // Test is_valid
    for (size_t i = 0; i < 4; ++i)
    {
        header h;
        h.signature[i] = '!';
        VERIFY (!h.is_valid ());
    }
}

template<typename T>
void test_equal (const T &h1, const T &h2)
{
    VERIFY (h1 == h2);
    VERIFY (!(h1 != h2));
    VERIFY (h2 == h1);
    VERIFY (!(h2 != h1));
}

template<typename T>
void test_not_equal (const T &h1, const T &h2)
{
    VERIFY (h1 != h2);
    VERIFY (!(h1 == h2));
    VERIFY (h2 != h1);
    VERIFY (!(h2 == h1));
}

void test_relational ()
{
    header h1, h2;
    h1.wkt = "WKT";
    h2.wkt = "WKT";

    // Change signature
    test_equal (h1, h2);
    h1.signature[3] = 'X';
    test_not_equal (h1, h2);
    h1 = h2;

    // Change major_version
    test_equal (h1, h2);
    h1.major_version += 1;
    test_not_equal (h1, h2);
    h1 = h2;

    // Change minor_version
    test_equal (h1, h2);
    h1.minor_version += 1;
    test_not_equal (h1, h2);
    h1 = h2;

    // Change wkt
    test_equal (h1, h2);
    h1.wkt[0] += 1;
    test_not_equal (h1, h2);
    h1 = h2;

    // Change extra_fields
    test_equal (h1, h2);
    h1.extra_fields += 1;
    test_not_equal (h1, h2);
    h1 = h2;

    // Change total_points
    test_equal (h1, h2);
    h1.total_points += 1;
    test_not_equal (h1, h2);
    h1 = h2;

    // Change compressed
    test_equal (h1, h2);
    h1.compressed = !h1.compressed;
    test_not_equal (h1, h2);
    h1 = h2;
}

void test_read_write ()
{
    // Test read/write
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

    // Fail when wkt is too long
    {
    vector<char> s (0x10000);
    string wkt (&s[0], 0x10000);
    VERIFY_THROWS (header h (wkt, 0, 0, false);)
    }

    // Fail when wkt is too long
    {
    vector<char> s (0x10000);
    string wkt (&s[0], 0x10000);
    header h ("WKT", 0, 0, false);
    h.wkt = wkt;
    VERIFY (!h.is_valid ());
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
        test_relational ();
        test_read_write ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
