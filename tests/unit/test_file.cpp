#include "app_utils.h"
#include "compression.h"
#include "spoc.h"
#include "test_utils.h"
#include <iostream>
#include <random>
#include <vector>
#include <stdexcept>

using namespace std;
using namespace spoc::app_utils;
using namespace spoc::file;
using namespace spoc::header;
using namespace spoc::io;

void test_spoc_file ()
{
    // Test spoc_file::is_valid()
    {
    spoc_file f;
    header &h = const_cast<header &> (f.get_header ());
    VERIFY (f.is_valid ());
    h.total_points = 1;
    VERIFY (!f.is_valid ());
    }
    {
    spoc_file f;
    f.add (spoc::point_record::point_record (0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0));
    header &h = const_cast<header &> (f.get_header ());
    VERIFY (f.is_valid ());
    h.extra_fields = 1;
    VERIFY (!f.is_valid ());
    }

    // Test CTORs
    const size_t total_points = 1000;
    const size_t extra_fields = 3;
    auto p = generate_random_point_records (total_points, extra_fields);
    p[100].extra.resize (extra_fields + 1);
    header h ("WKT", extra_fields, total_points, false);

    VERIFY_THROWS (spoc_file f (h, p);)
    VERIFY_THROWS ({
        stringstream s;
        const string wkt = "Test wkt";
        write_spoc_file_uncompressed (s, spoc_file (wkt, p));
    })

    VERIFY_THROWS ({
        stringstream s;
        const string wkt = "Test wkt";
        write_spoc_file_compressed (s, spoc_file (wkt, p));
    })

    header h2 ("WKT", extra_fields, total_points + 1, false);
    VERIFY_THROWS (spoc_file f (h2, p);)

    header h3 ("WKT", extra_fields + 1, total_points, false);
    VERIFY_THROWS (spoc_file f (h3, p);)

    {
    const size_t total_points = 100;
    const size_t extra_fields = 3;
    auto p = generate_random_point_records (total_points, extra_fields);
    const string wkt = "Test wkt";
    auto f = spoc_file (wkt, p);
    auto g = f.get_wkt ();
    VERIFY (g == wkt);
    f.set_wkt ("Test WKT 2");
    for (size_t i = 0; i < f.get_point_records ().size (); ++i)
    {
        const auto q = f.get_point_record (i);
        f.set (i, q);
    }
    }

    {
    const size_t total_points = 100;
    const size_t extra_fields = 3;
    auto p = generate_random_point_records (total_points, extra_fields);
    const string wkt = "Test wkt";
    auto f = spoc_file (wkt, p);
    // Make the size check fail
    auto q = f.get_point_record (0);
    q.extra.resize (extra_fields + 1);
    f.set (0, q);
    f.set_compressed (true);
    const auto g = f.get_compressed ();
    VERIFY (g);

    // Throw because size check fails
    VERIFY_THROWS ({
        stringstream s;
        f.set_compressed (true);
        write_spoc_file_compressed (s, f);
    })

    // Throw because size check fails
    VERIFY_THROWS ({
        stringstream s;
        f.set_compressed (false);
        write_spoc_file_uncompressed (s, f);
    })
    }
}

void test_spoc_file_rw_access ()
{
    const size_t total_points = 10;
    const size_t extra_fields = 3;
    auto p = generate_random_point_records (total_points, extra_fields);
    auto f = spoc_file ("WKT", p);
    f[0].x = 2.0;
    // Test operator[]
    VERIFY (f[0].x == 2.0);
    // Test begin(), end()
    for (auto &i : f)
        i.x = 1.0;
}

int main (int argc, char **argv)
{
    try
    {
        test_spoc_file ();
        test_spoc_file_rw_access ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
