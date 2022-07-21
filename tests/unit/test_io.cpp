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
using namespace spoc::io;

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

void test_spoc_file_io ()
{
    const size_t total_points = 1000;
    const size_t extra_fields = 8;
    auto p = generate_random_point_records (total_points, extra_fields);

    {
    stringstream s;
    const string wkt = "Test wkt";
    write_spoc_file_uncompressed (s, spoc_file (wkt, p));
    const auto f = read_spoc_file_uncompressed (s);

    VERIFY (f.get_header ().wkt == wkt);
    VERIFY (p == f.get_point_records ());
    }

    {
    stringstream s;
    const string wkt = "Test wkt";
    write_spoc_file_uncompressed (s, spoc_file (wkt, p));
    const auto f = read_spoc_file (s);

    VERIFY (f.get_header ().wkt == wkt);
    VERIFY (p == f.get_point_records ());
    }

    {
    stringstream s;
    const string wkt = "Test wkt";
    write_spoc_file_compressed (s, spoc_file (wkt, true, p));
    const auto f = read_spoc_file (s);

    VERIFY (f.get_header ().wkt == wkt);
    VERIFY (p == f.get_point_records ());
    }

    // Fail when reading signature
    {
    stringstream s;
    // Write an invalid signature
    s << "SPOX" << endl;
    VERIFY_THROWS (const auto f = read_spoc_file_uncompressed (s);)
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

void test_spoc_file_compressed_io ()
{
    const size_t total_points = 1000;
    const size_t extra_fields = 8;
    auto p = generate_random_point_records (total_points, extra_fields);

    {
    stringstream s;
    const string wkt = "Test wkt";
    write_spoc_file_compressed (s, spoc_file (wkt, true, p));
    const auto f = read_spoc_file_compressed (s);

    VERIFY (wkt == f.get_header ().wkt);
    VERIFY (p == f.get_point_records ());
    }

    // Compare sizes
    {
    const string wkt = "Test wkt";
    stringstream s1;
    stringstream s2;
    stringstream s3;
    write_spoc_file_uncompressed (s1, spoc_file (wkt, p));
    write_spoc_file_compressed (s2, spoc_file (wkt, true, p));
    // Zero some fields
    for (auto &i : p)
    {
        i.c = 0;
        i.extra[3] = 0;
        i.extra[7] = 0;
    }
    write_spoc_file_compressed (s3, spoc_file (wkt, true, p));
    const auto f1 = read_spoc_file_uncompressed (s1);
    const auto f2 = read_spoc_file_compressed (s2);
    const auto f3 = read_spoc_file_compressed (s3);
    // Compressed file should be smaller
    VERIFY (s1.str ().size () > s2.str ().size ());
    // File with zero fields should be smaller
    VERIFY (s2.str ().size () > s3.str ().size ());
    }

    // Write compressed with uncompressed writer
    {
    stringstream s;
    VERIFY_THROWS (write_spoc_file_uncompressed (s, spoc_file ("WKT", true, p));)
    }

    // Write uncompressed with compressed writer
    {
    stringstream s;
    VERIFY_THROWS (write_spoc_file_compressed (s, spoc_file ("WKT", false, p));)
    }

    // Read uncompressed with compressed reader
    {
    stringstream s;
    write_spoc_file_uncompressed (s, spoc_file ("WKT", false, p));
    VERIFY_THROWS (const auto f = read_spoc_file_compressed (s);)
    }

    // Read compressed with uncompressed reader
    {
    stringstream s;
    write_spoc_file_compressed (s, spoc_file ("WKT", true, p));
    VERIFY_THROWS (const auto f = read_spoc_file_uncompressed (s);)
    }

    {
    stringstream s;
    write_spoc_file (s, spoc_file ("WKT", false, p));
    auto f = read_spoc_file (s);
    write_spoc_file (s, spoc_file ("WKT", true, p));
    f = read_spoc_file (s);
    }
}

void test_field_name ()
{
    string s = "e100";
    VERIFY (is_extra_field (s));
    auto i = get_extra_index (s);
    VERIFY (i == 100);
    VERIFY (!is_extra_field ("x100"));
    VERIFY (is_extra_field ("e1000001"));
    i = get_extra_index ("x100");
    VERIFY (i == -1);
    i = get_extra_index ("exyz");
    VERIFY (i == -1);
    VERIFY (check_field_name ("e10000") == true);
    VERIFY (check_field_name ("x10000") == false);
    VERIFY (check_field_name ("e0") == true);
    VERIFY (check_field_name ("exxx") == false);
}

int main (int argc, char **argv)
{
    try
    {
        test_header ();
        test_spoc_file ();
        test_spoc_file_io ();
        test_spoc_file_rw_access ();
        test_spoc_file_compressed_io ();
        test_field_name ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
