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
    verify (h.is_valid ());
    }

    {
    for (size_t i = 0; i < 4; ++i)
    {
        header h;
        h.signature[i] = '!';
        verify (!h.is_valid ());
    }
    }

    {
    header h;
    h.wkt = "Test WKT";
    verify (h.check_signature ());
    stringstream s;
    write_header (s, h);

    const auto g = read_header (s);

    verify (h == g);
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
    verify (g == wkt);
    f.set_wkt ("Test WKT 2");
    for (size_t i = 0; i < f.get_point_records ().size (); ++i)
    {
        const auto q = f.get_point_record (i);
        f.set_point_record (i, q);
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
    f.set_point_record (0, q);
    f.set_compressed (true);
    const auto g = f.get_compressed ();
    verify (g);

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

    verify (f.get_header ().wkt == wkt);
    verify (p == f.get_point_records ());
    }

    {
    stringstream s;
    const string wkt = "Test wkt";
    write_spoc_file_uncompressed (s, spoc_file (wkt, p));
    const auto f = read_spoc_file (s);

    verify (f.get_header ().wkt == wkt);
    verify (p == f.get_point_records ());
    }

    {
    stringstream s;
    const string wkt = "Test wkt";
    write_spoc_file_compressed (s, spoc_file (wkt, true, p));
    const auto f = read_spoc_file (s);

    verify (f.get_header ().wkt == wkt);
    verify (p == f.get_point_records ());
    }

    // Fail when reading signature
    {
    stringstream s;
    // Write an invalid signature
    s << "SPOX" << endl;
    VERIFY_THROWS (const auto f = read_spoc_file_uncompressed (s);)
    }
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

    verify (wkt == f.get_header ().wkt);
    verify (p == f.get_point_records ());
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
    verify (s1.str ().size () > s2.str ().size ());
    // File with zero fields should be smaller
    verify (s2.str ().size () > s3.str ().size ());
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
}

void test_field_name ()
{
    string s = "e100";
    verify (is_extra_field (s));
    auto i = get_extra_index (s);
    verify (i == 100);
    verify (!is_extra_field ("x100"));
    verify (is_extra_field ("e1000001"));
    i = get_extra_index ("x100");
    verify (i == -1);
    i = get_extra_index ("exyz");
    verify (i == -1);
    verify (check_field_name ("e10000") == true);
    verify (check_field_name ("x10000") == false);
    verify (check_field_name ("e0") == true);
    verify (check_field_name ("exxx") == false);
}

int main (int argc, char **argv)
{
    try
    {
        test_header ();
        test_spoc_file ();
        test_spoc_file_io ();
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
