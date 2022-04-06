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
    remove_const<decltype(p)>::type q;
    read_spoc_file (s, q, wkt2);

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
}

int main (int argc, char **argv)
{
    try
    {
        test_point_record ();
        test_spoc_file_io ();
        test_spoc_file ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
