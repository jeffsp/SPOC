#include "benchmark_utils.h"
#include "point_record.h"
#include "spoc.h"
#include "test_utils.h"
#include <iostream>
#include <vector>
#include <stdexcept>

using namespace std;
using namespace spoc::benchmark_utils;
using namespace spoc::point_record;
using namespace spoc::io;

template<typename T>
void benchmark_write (const T &p, const size_t extra_fields)
{
    stringstream s;

    clog << "Writing " << p.size () << " point records" << endl;

    // Write them to the stream and time it
    timer t;

    for (const auto &i : p)
        write_point_record (s, i);

    clog << t.elapsed_ms () << "ms" << endl;
}

template<typename T>
void benchmark_read (const T &p, const size_t extra_fields)
{
    stringstream s;

    // Write them to the stream
    for (const auto &i : p)
        write_point_record (s, i);

    clog << "Reading " << p.size () << " point records" << endl;

    // Allocate space for them
    point_records r (p.size ());

    // Read them back out and time it
    timer t;

    for (size_t i = 0; i < r.size (); ++i)
        r[i] = read_point_record (s, extra_fields);

    clog << t.elapsed_ms () << "ms" << endl;
}

int main (int argc, char **argv)
{
    try
    {
        const size_t total_points = 10000000;
        const size_t extra_fields = 8;
        auto p = generate_random_point_records (total_points, extra_fields);

        benchmark_write (p, extra_fields);
        benchmark_read (p, extra_fields);

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
