#include "benchmark_utils.h"
#include "spoc.h"
#include "test_utils.h"
#include <iostream>
#include <vector>
#include <stdexcept>

using namespace std;
using namespace spoc::benchmark_utils;
using namespace spoc::file;
using namespace spoc::test_utils;

void benchmark_push_back ()
{
    const size_t total_points = 1'000;
    const size_t iterations = 100'000;
    const size_t extra_fields = 3;
    auto p = generate_random_point_records (total_points, extra_fields);
    decltype(p) q;

    timer t;

    t.start ();
    for (size_t i = 0; i < iterations; ++i)
        for (const auto &r : p)
            q.push_back (r);
    t.stop ();

    clog << "vector::push_back(): " << t.elapsed_ms () << "ms" << endl;

    const bool compressed = false;
    spoc_file f ("WKT", compressed);

    t.start ();
    for (size_t i = 0; i < iterations; ++i)
        for (const auto &r : p)
            f.push_back (r);
    t.stop ();

    clog << "spoc_file::push_back(): " << t.elapsed_ms () << "ms" << endl;
}

void benchmark_change_point_records ()
{
    const size_t total_points = 100'000'000;
    const size_t extra_fields = 5;
    clog << "Creating point records..." << endl;
    auto p = generate_random_point_records (total_points, extra_fields);
    decltype(p) q;

    timer t;

    const bool compressed = false;
    spoc_file f ("WKT", compressed, p);

    clog << "Starting benchmark..." << endl;

    t.start ();
    for (size_t i = 0; i < f.get_point_records ().size (); ++i)
    {
        // Get record
        auto r = f.get_point_record (i);
        // Change it
        r.x += 1.23;
        r.extra[extra_fields - 1] += 123;
        // Set it
        f.set_point_record (i, r);
    }
    t.stop ();
    clog << "spoc_file::get_point_record(i)/set_point_record(i,r): " << t.elapsed_ms () << "ms" << endl;

    t.start ();
    // Copy them all
    auto prs = f.get_point_records ();
    for (size_t i = 0; i < prs.size (); ++i)
    {
        // Change it
        prs[i].x += 1.23;
        prs[i].extra[extra_fields - 1] += 123;
    }
    // Set them all
    f.set_point_records (prs);
    t.stop ();
    clog << "spoc_file::get_point_records()/set_point_records(): " << t.elapsed_ms () << "ms" << endl;

    {
    t.start ();
    // Move them all
    auto prs = f.move_point_records ();
    for (size_t i = 0; i < prs.size (); ++i)
    {
        // Change it
        prs[i].x += 1.23;
        prs[i].extra[extra_fields - 1] += 123;
    }
    // Move them back
    f.move_point_records (prs);
    t.stop ();
    clog << "spoc_file::move_point_records()/move_point_records(prs): " << t.elapsed_ms () << "ms" << endl;
    }
}

int main (int argc, char **argv)
{
    try
    {
        benchmark_push_back ();
        benchmark_change_point_records ();

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
