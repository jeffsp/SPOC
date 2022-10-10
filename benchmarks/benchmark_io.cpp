#include "spoc/benchmark_utils.h"
#include "spoc/point_record.h"
#include "spoc/spoc.h"
#include "spoc/test_utils.h"
#include <iostream>
#include <locale>
#include <stdexcept>
#include <vector>

using namespace std;
using namespace spoc::benchmark_utils;
using namespace spoc::file;
using namespace spoc::io;
using namespace spoc::point_record;
using namespace spoc::test_utils;

template<typename T>
void benchmark_write_uncompressed (const T &p, const size_t extra_fields)
{
    stringstream s;

    clog << "Writing " << p.size () << " point records uncompressed" << endl;

    // Write them to the stream and time it
    timer t;

    for (const auto &i : p)
        write_point_record (s, i);

    clog << t.elapsed_ms () << "ms" << endl;
}

template<typename T>
void benchmark_read_uncompressed (const T &p, const size_t extra_fields)
{
    stringstream s;

    // Write them to the stream
    for (const auto &i : p)
        write_point_record (s, i);

    clog << "Reading " << p.size () << " point records uncompressed" << endl;

    // Read them back out and time it
    timer t;

    // Allocate space for them
    point_records r = read_uncompressed_points (s, p.size (), extra_fields);

    clog << t.elapsed_ms () << "ms" << endl;

    VERIFY (p == r);
}

template<typename T>
void benchmark_write_compressed (const T &p, const size_t extra_fields)
{
    stringstream s;

    clog << "Writing " << p.size () << " point records compressed" << endl;

    const string wkt = "Test wkt";

    // Write them to the stream and time it
    timer t;

    write_spoc_file_compressed (s, spoc_file (wkt, true, p));

    clog << t.elapsed_ms () << "ms" << endl;
}

template<typename T>
void benchmark_read_compressed (const T &p, const size_t extra_fields)
{
    stringstream s;

    // Write them to the stream
    write_spoc_file_compressed (s, spoc_file ("WKT", true, p));

    clog << "Reading " << p.size () << " point records compressed" << endl;

    // Read them back out and time it
    timer t;

    const auto f = read_spoc_file_compressed (s);

    clog << t.elapsed_ms () << "ms" << endl;
}

int main (int argc, char **argv)
{
    try
    {
        const size_t total_points = 10'000'000;
        const size_t extra_fields = 8;
        auto p = generate_random_point_records (total_points, extra_fields);
        std::locale l ("");
        std::clog.imbue (l);
        std::cout.imbue (l);

        benchmark_write_uncompressed (p, extra_fields);
        benchmark_read_uncompressed (p, extra_fields);
        benchmark_write_compressed (p, extra_fields);
        benchmark_read_compressed (p, extra_fields);

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
