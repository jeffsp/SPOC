#include "compress.h"
#include "spoc.h"
#include "test_utils.h"
#include <iostream>
#include <random>
#include <vector>
#include <stdexcept>

using namespace std;
using namespace spoc;

void test_header ()
{
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
    bool failed = false;
    try
    {
        const auto q = read_header (s);
    }
    catch (...)
    {
        failed = true;
    }
    verify (failed);
    }

    // Fail when writing
    {
    stringstream s;
    header h;
    h.signature[3] = 'X';
    bool failed = false;
    try
    {
        write_header (s, h);
    }
    catch (...)
    {
        failed = true;
    }
    verify (failed);
    }
}

void test_point_record ()
{
    const auto p = generate_random_point_records (10, 8);

    for (const auto &i : p)
    {
        verify (i.x != 0);
        verify (i.y != 0);
        verify (i.z != 0);
        verify (i.c != 0);
        verify (i.p != 0);
        verify (i.i != 0);
        verify (i.r != 0);
        verify (i.g != 0);
        verify (i.b != 0);
    }

    const auto q (p);
    verify (p == q);

    auto a = p[0];
    auto b = a;
    auto c = p[1];
    verify (a == b); a.x = c.x; verify (a != b); a.x = b.x;
    verify (a == b); a.y = c.y; verify (a != b); a.y = b.y;
    verify (a == b); a.z = c.z; verify (a != b); a.z = b.z;
    verify (a == b); a.c = c.c; verify (a != b); a.c = b.c;
    verify (a == b); a.p = c.p; verify (a != b); a.p = b.p;
    verify (a == b); a.i = c.i; verify (a != b); a.i = b.i;
    verify (a == b); a.r = c.r; verify (a != b); a.r = b.r;
    verify (a == b); a.g = c.g; verify (a != b); a.g = b.g;
    verify (a == b); a.b = c.b; verify (a != b); a.b = b.b;
    for (size_t i = 0; i < a.extra.size (); ++i)
    {
        verify (a == b); a.extra[i] = c.extra[i];
        verify (a != b); a.extra[i] = b.extra[i];
    }
}

void test_spoc_file ()
{
    const size_t total_points = 1000;
    const size_t extra_fields = 3;
    auto p = generate_random_point_records (total_points, extra_fields);
    p[100].extra.resize (extra_fields + 1);
    spoc::header h ("WKT", extra_fields, total_points, false);

    bool failed = false;
    try { spoc_file f (h, p); }
    catch (...)
    { failed = true; }
    verify (failed);

    failed = false;
    try {
        stringstream s;
        const string wkt = "Test wkt";
        write_spoc_file_uncompressed (s, spoc_file (wkt, p));
    }
    catch (...)
    { failed = true; }
    verify (failed);

    failed = false;
    try {
        stringstream s;
        const string wkt = "Test wkt";
        write_spoc_file_compressed (s, spoc_file (wkt, p));
    }
    catch (...)
    { failed = true; }
    verify (failed);

    spoc::header h2 ("WKT", extra_fields, total_points + 1, false);
    failed = false;
    try { spoc_file f (h2, p); }
    catch (...)
    { failed = true; }
    verify (failed);

    spoc::header h3 ("WKT", extra_fields + 1, total_points, false);
    failed = false;
    try { spoc_file f (h3, p); }
    catch (...)
    { failed = true; }
    verify (failed);

    {
    const size_t total_points = 100;
    const size_t extra_fields = 3;
    auto p = generate_random_point_records (total_points, extra_fields);
    const string wkt = "Test wkt";
    auto f = spoc_file (wkt, p);
    f.set_wkt ("Test WKT 2");
    for (size_t i = 0; i < f.get_point_records ().size (); ++i)
    {
        const auto q = f.get_point_record (i);
        f.set_point_record (i, q);
    }
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

    // Fail when reading signature
    {
    stringstream s;
    // Write an invalid signature
    s << "SPOX" << endl;
    bool failed = false;
    try
    {
        const auto f = read_spoc_file_uncompressed (s);
    }
    catch (...)
    {
        failed = true;
    }
    verify (failed);
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
    bool failed = false;
    try { write_spoc_file_uncompressed (s, spoc_file ("WKT", true, p)); }
    catch (...) { failed = true; }
    verify (failed);
    }

    // Write uncompressed with compressed writer
    {
    stringstream s;
    bool failed = false;
    try { write_spoc_file_compressed (s, spoc_file ("WKT", false, p)); }
    catch (...) { failed = true; }
    verify (failed);
    }

    // Read uncompressed with compressed reader
    {
    stringstream s;
    bool failed = false;
    write_spoc_file_uncompressed (s, spoc_file ("WKT", false, p));
    try { const auto f = read_spoc_file_compressed (s); }
    catch (...) { failed = true; }
    verify (failed);
    }

    // Read compressed with uncompressed reader
    {
    stringstream s;
    bool failed = false;
    write_spoc_file_compressed (s, spoc_file ("WKT", true, p));
    try { const auto f = read_spoc_file_uncompressed (s); }
    catch (...) { failed = true; }
    verify (failed);
    }
}

int main (int argc, char **argv)
{
    try
    {
        test_header ();
        test_point_record ();
        test_spoc_file ();
        test_spoc_file_io ();
        test_spoc_file_compressed_io ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
