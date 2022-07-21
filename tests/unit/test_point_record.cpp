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
    VERIFY (p.x == 0.0);
    VERIFY (p.y == 0.0);
    VERIFY (p.z == 0.0);
    VERIFY (p.c == 0);
    VERIFY (p.p == 0);
    VERIFY (p.i == 0);
    VERIFY (p.r == 0);
    VERIFY (p.g == 0);
    VERIFY (p.b == 0);
    VERIFY (p.extra.size () == 0);
    const auto q = point_record {1.0, 2.0, 3.0};
    VERIFY (about_equal (q.x, 1.0));
    VERIFY (about_equal (q.y, 2.0));
    VERIFY (about_equal (q.z, 3.0));
    VERIFY (q.c == 0);
    VERIFY (q.p == 0);
    VERIFY (q.i == 0);
    VERIFY (q.r == 0);
    VERIFY (q.g == 0);
    VERIFY (q.b == 0);
    VERIFY (q.extra.size () == 0);
    const auto r = point_record {10};
    VERIFY (r.x == 0.0);
    VERIFY (r.y == 0.0);
    VERIFY (r.z == 0.0);
    VERIFY (r.c == 0);
    VERIFY (r.p == 0);
    VERIFY (r.i == 0);
    VERIFY (r.r == 0);
    VERIFY (r.g == 0);
    VERIFY (r.b == 0);
    VERIFY (r.extra.size () == 10);
    const auto s = point_record {1.0, 2.0, 3.0, 4};
    VERIFY (about_equal (s.x, 1.0));
    VERIFY (about_equal (s.y, 2.0));
    VERIFY (about_equal (s.z, 3.0));
    VERIFY (s.c == 4);
}

void test_point_record_fields ()
{
    const auto p = generate_random_point_records (10, 8);

    for (const auto &i : p)
    {
        VERIFY (i.x != 0);
        VERIFY (i.y != 0);
        VERIFY (i.z != 0);
        VERIFY (i.c != 0);
        VERIFY (i.p != 0);
        VERIFY (i.i != 0);
        VERIFY (i.r != 0);
        VERIFY (i.g != 0);
        VERIFY (i.b != 0);
    }

    const auto q (p);
    VERIFY (p == q);

    auto a = p[0];
    auto b = a;
    auto c = p[1];
    VERIFY (a == b); a.x = c.x; VERIFY (a != b); a.x = b.x;
    VERIFY (a == b); a.y = c.y; VERIFY (a != b); a.y = b.y;
    VERIFY (a == b); a.z = c.z; VERIFY (a != b); a.z = b.z;
    VERIFY (a == b); a.c = c.c; VERIFY (a != b); a.c = b.c;
    VERIFY (a == b); a.p = c.p; VERIFY (a != b); a.p = b.p;
    VERIFY (a == b); a.i = c.i; VERIFY (a != b); a.i = b.i;
    VERIFY (a == b); a.r = c.r; VERIFY (a != b); a.r = b.r;
    VERIFY (a == b); a.g = c.g; VERIFY (a != b); a.g = b.g;
    VERIFY (a == b); a.b = c.b; VERIFY (a != b); a.b = b.b;
    for (size_t i = 0; i < a.extra.size (); ++i)
    {
        VERIFY (a == b); a.extra[i] = c.extra[i];
        VERIFY (a != b); a.extra[i] = b.extra[i];
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
