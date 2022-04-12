#include "compress.h"
#include "spoc.h"
#include "test_utils.h"
#include <iostream>
#include <random>
#include <vector>
#include <stdexcept>

using namespace std;
using namespace spoc;

vector<point_record> get_random_point_records (const size_t n)
{
    default_random_engine g;
    uniform_int_distribution<int> di (0, 1 << 15);
    uniform_real_distribution<double> dr (-1.0, 1.0);

    vector<point_record> p (n);

    for (auto &i : p)
    {
        i.x = dr (g); i.y = dr (g); i.z = dr (g);
        i.c = di (g); i.p = di (g); i.i = di (g);
        i.r = di (g); i.g = di (g); i.b = di (g);
        for (auto &j : i.extra)
            j = di (g);
    }

    return p;
}

void test_point_record ()
{
    const auto p = get_random_point_records (10);

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

void test_spoc_file_io ()
{
    const auto p = get_random_point_records (1000);

    stringstream s;
    const string wkt1 = "Test wkt";
    write_spoc_file (s, p, wkt1);
    string wkt2;
    auto q = read_spoc_file (s, wkt2);

    verify (wkt1 == wkt2);
    verify (p == q);
}

void test_spoc_file ()
{
    spoc_file f;
    verify (f.get_signature ()[0] == 'S');
    verify (f.get_signature ()[1] == 'P');
    verify (f.get_signature ()[2] == 'O');
    verify (f.get_signature ()[3] == 'C');
    verify (f.get_npoints () == 0);
    verify (f.get_x ().empty () == true);
    verify (f.get_npoints () == 0);
    verify (f.get_x ().size () == 0);
    verify (f.get_y ().size () == 0);
    verify (f.get_z ().size () == 0);
    f.resize (1);
    verify (f.get_npoints () == 1);
    auto p = f.get (0);
    verify (f.get_x ().size () == 0);
    verify (f.get_y ().size () == 0);
    verify (f.get_z ().size () == 0);
    verify (p.x == 0);
    verify (p.y == 0);
    verify (p.z == 0);
    p.z = 123;
    f.set (0, p);
    verify (f.get_x ().size () == 0);
    verify (f.get_y ().size () == 0);
    verify (f.get_z ().size () == 1);
    verify (f.get_extra ()[7].size () == 0);
    p.extra[7] = 456;
    f.set (0, p);
    verify (f.get_extra ()[7].size () == 1);
    f.resize (10);
    verify (f.get_npoints () == 10);
    verify (f.get_x ().size () == 0);
    verify (f.get_z ().size () == 10);
    verify (f.get_extra ()[7].size () == 10);
    f.set (9, p);
    verify (f.get_z ().at (0) == 123);
    verify (f.get_z ().at (9) == 123);
    verify (f.get_extra ()[7].at (0) == 456);
    verify (f.get_extra ()[7].at (1) == 0);
    verify (f.get_extra ()[7].at (8) == 0);
    verify (f.get_extra ()[7].at (9) == 456);
    p.z = 0;
    f.set (0, p);
    verify (f.get_z ().at (0) == 0);
    f.set (9, p);
    verify (f.get_z ().at (9) == 0);
    verify (f.get_z ().size () == 10);
    f.reallocate ();
    verify (f.get_z ().size () == 0);
    verify (f.get_npoints () == 10);
    p.extra[7] = 0;
    f.set (0, p);
    f.set (9, p);
    f.resize (11);
    verify (f.get_npoints () == 11);
    const auto q = get_random_point_records (3);
    f.set (0, q[0]);
    f.set (9, q[1]);
    f.set (10, q[2]);
    point_record p0;
    f.set (0, p0);
    f.set (9, p0);
    f.reallocate ();
    verify (f.get_npoints () == 11);
    verify (f.get_x ().size () == 11);
    verify (f.get_y ().size () == 11);
    verify (f.get_z ().size () == 11);
    verify (f.get_c ().size () == 11);
    verify (f.get_p ().size () == 11);
    verify (f.get_i ().size () == 11);
    verify (f.get_r ().size () == 11);
    verify (f.get_g ().size () == 11);
    verify (f.get_b ().size () == 11);
    for (size_t j = 0; j < f.get_extra ().size (); ++j)
        verify (f.get_extra ()[j].size () == 11);
    f.set (10, p0);
    f.reallocate ();
    verify (f.get_x ().size () == 0);
    verify (f.get_y ().size () == 0);
    verify (f.get_z ().size () == 0);
    verify (f.get_c ().size () == 0);
    verify (f.get_p ().size () == 0);
    verify (f.get_i ().size () == 0);
    verify (f.get_r ().size () == 0);
    verify (f.get_g ().size () == 0);
    verify (f.get_b ().size () == 0);
    for (size_t j = 0; j < f.get_extra ().size (); ++j)
        verify (f.get_extra ()[j].size () == 0);
}

void test_free_wydu ()
{
    const size_t N = 1000;
    size_t s0, s1, s2, s3;
    vector<point_record> pc (N);
    {
    stringstream s;
    const string wkt = "WKT";
    write_spoc_file (s, pc, wkt);
    // Save the size on disk
    s0 = s.str().size ();
    }
    pc = get_random_point_records (1000);
    {
    stringstream s;
    const string wkt = "WKT";
    write_spoc_file (s, pc, wkt);
    // Save the size on disk
    s1 = s.str().size ();
    }
    // Zero out Z coordinate
    for (auto &p : pc)
        p.z = 0.0;
    {
    stringstream s;
    const string wkt = "WKT";
    write_spoc_file (s, pc, wkt);
    // Save the size on disk
    s2 = s.str().size ();
    }
    // Zero out all fields
    for (auto &p : pc)
    {
        p.x = p.y = p.c = p.p = p.i = p.r = p.g = p.b = 0;
        for (size_t j = 0; j < p.extra.size (); ++j)
            p.extra[j] = 0;
    }
    {
    stringstream s;
    const string wkt = "WKT";
    write_spoc_file (s, pc, wkt);
    // Save the size on disk
    s3 = s.str().size ();
    }
    // All zeros is much smaller than random data
    verify (s0 * 10 < s1);
    // Zeroing out a field makes it smaller
    verify (s2 < s1);
    // All zeros is same size as all zeros
    verify (s0 == s3);
}

int main (int argc, char **argv)
{
    try
    {
        test_point_record ();
        test_spoc_file_io ();
        test_spoc_file ();
        test_free_wydu ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
