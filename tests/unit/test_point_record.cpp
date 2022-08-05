#include "spoc/point_record.h"
#include "spoc/test_utils.h"
#include <iostream>
#include <random>
#include <vector>
#include <stdexcept>
#include <unordered_set>

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

void test_point_record_hash ()
{
    unordered_set<point_record,point_record_hash> s;
    s.insert ({ 1, 0, 0, 0, 0, 0, 0, 0, 0, { 0, 0, 0 }});
    s.insert ({ 0, 1, 0, 0, 0, 0, 0, 0, 0, { 0, 0, 0 }});
    s.insert ({ 0, 0, 1, 0, 0, 0, 0, 0, 0, { 0, 0, 0 }});
    s.insert ({ 0, 0, 0, 1, 0, 0, 0, 0, 0, { 0, 0, 0 }});
    s.insert ({ 0, 0, 0, 0, 1, 0, 0, 0, 0, { 0, 0, 0 }});
    s.insert ({ 0, 0, 0, 0, 0, 1, 0, 0, 0, { 0, 0, 0 }});
    s.insert ({ 0, 0, 0, 0, 0, 0, 1, 0, 0, { 0, 0, 0 }});
    s.insert ({ 0, 0, 0, 0, 0, 0, 0, 1, 0, { 0, 0, 0 }});
    s.insert ({ 0, 0, 0, 0, 0, 0, 0, 0, 1, { 0, 0, 0 }});
    s.insert ({ 0, 0, 0, 0, 0, 0, 0, 0, 0, { 1, 0, 0 }});
    s.insert ({ 0, 0, 0, 0, 0, 0, 0, 0, 0, { 0, 1, 0 }});
    s.insert ({ 0, 0, 0, 0, 0, 0, 0, 0, 0, { 0, 0, 1 }});
    VERIFY (s.size () == 12);

    // Generate hashes for each point_record and insert them into a set
    unordered_set<size_t> u;
    for (auto i : s)
        u.insert (point_record_hash {} (i));

    // They should all be unique
    VERIFY (u.size () == 12);


}

void test_point_record_hash_load ()
{
    // Generate a bunch of unique points
    const size_t n = 100;
    const auto p = generate_random_point_records (n, 8);
    // Put them in a set
    unordered_set<point_record,point_record_hash> s;
    for (const auto &i : p)
        s.insert (i);
    VERIFY (s.size () == n);
    // Make sure the hash load is acceptable
    //
    // If any bucket has more than 10% of the point, then that means our
    // hashing is not efficient
    for (size_t i = 0; i < s.bucket_count(); ++i)
        VERIFY (s.bucket_size (i) < n / 10);
}

void test_point_record_all_zero ()
{
    // Generate a bunch of unique points
    const size_t n = 10;
    const size_t extra_fields = 3;
    auto p = generate_random_point_records (n, extra_fields);

    // None are zero
    VERIFY (!has_all_zero_c (p));
    VERIFY (!has_all_zero_p (p));
    VERIFY (!has_all_zero_i (p));
    VERIFY (!has_all_zero_r (p));
    VERIFY (!has_all_zero_g (p));
    VERIFY (!has_all_zero_b (p));
    VERIFY (!has_all_zero_rgb (p));
    for (size_t i = 0; i < extra_fields; ++i)
        VERIFY (!has_all_zero_extra (i, p));

    // Set them to zero
    for_each (begin (p), end (p), [] (point_record &i) { i.c = 0; });
    VERIFY (has_all_zero_c (p));
    for_each (begin (p), end (p), [] (point_record &i) { i.p = 0; });
    VERIFY (has_all_zero_p (p));
    for_each (begin (p), end (p), [] (point_record &i) { i.i = 0; });
    VERIFY (has_all_zero_i (p));
    for_each (begin (p), end (p), [] (point_record &i) { i.r = 0; });
    VERIFY (has_all_zero_r (p));
    for_each (begin (p), end (p), [] (point_record &i) { i.g = 0; });
    VERIFY (has_all_zero_g (p));
    VERIFY (!has_all_zero_rgb (p));
    for_each (begin (p), end (p), [] (point_record &i) { i.b = 0; });
    VERIFY (has_all_zero_b (p));
    VERIFY (has_all_zero_rgb (p));
    for (size_t j = 0; j < extra_fields; ++j)
    {
        VERIFY (!has_all_zero_extra (j, p));
        for_each (begin (p), end (p), [&] (point_record &i) { i.extra[j] = 0; });
        VERIFY (has_all_zero_extra (j, p));
    }
}

int main (int argc, char **argv)
{
    try
    {
        test_point_record_ctors ();
        test_point_record_fields ();
        test_point_record_hash ();
        test_point_record_hash_load ();
        test_point_record_all_zero ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
