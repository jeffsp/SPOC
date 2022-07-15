#include "app_utils.h"
#include "compression.h"
#include "spoc.h"
#include "test_utils.h"
#include <iostream>
#include <random>
#include <vector>
#include <stdexcept>

using namespace std;
using namespace spoc::point_record;

void test_point_record_ctors ()
{
    const auto p = point_record ();
    verify (p.x == 0.0);
    verify (p.y == 0.0);
    verify (p.z == 0.0);
    verify (p.c == 0);
    verify (p.p == 0);
    verify (p.i == 0);
    verify (p.r == 0);
    verify (p.g == 0);
    verify (p.b == 0);
    verify (p.extra.size () == 0);
    const auto q = point_record {1.0, 2.0, 3.0};
    verify (about_equal (q.x, 1.0));
    verify (about_equal (q.y, 2.0));
    verify (about_equal (q.z, 3.0));
    verify (q.c == 0);
    verify (q.p == 0);
    verify (q.i == 0);
    verify (q.r == 0);
    verify (q.g == 0);
    verify (q.b == 0);
    verify (q.extra.size () == 0);
    const auto r = point_record {10};
    verify (r.x == 0.0);
    verify (r.y == 0.0);
    verify (r.z == 0.0);
    verify (r.c == 0);
    verify (r.p == 0);
    verify (r.i == 0);
    verify (r.r == 0);
    verify (r.g == 0);
    verify (r.b == 0);
    verify (r.extra.size () == 10);
}

void test_point_record_fields ()
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

int main (int argc, char **argv)
{
    try
    {
        test_point_record_ctors ();
        test_point_record_fields ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
