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
