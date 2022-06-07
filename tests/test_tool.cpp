#include "tool.h"
#include "test_utils.h"
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace spoc;
using namespace spoc::transform;

void test_tool_quantize ()
{
    // Generate spoc files
    auto f = generate_random_spoc_file (10, 3, false);
    stringstream is, os;
    write_spoc_file_uncompressed (is, f);
    header h = read_header (is);
    quantize (is, os, h, 0.001);

    write_spoc_file_uncompressed (is, f);
    h = read_header (is);
    quantize (is, os, h, 0.01);

    write_spoc_file_uncompressed (is, f);
    h = read_header (is);
    quantize (is, os, h, 100);
}

void test_tool_recenter ()
{
    for (auto rgb : {true, false})
    {
        // Generate spoc files
        auto f = generate_random_spoc_file (1000, 8, false, rgb);
        stringstream is, os;

        write_spoc_file_uncompressed (is, f);
        header hdr = read_header (is);
        recenter (is, os, hdr);
        const auto g = read_spoc_file_uncompressed (os);

        write_spoc_file_uncompressed (is, f);
        hdr = read_header (is);
        recenter (is, os, hdr, true);
        const auto h = read_spoc_file_uncompressed (os);

        const auto fx = get_x (f.get_point_records ());
        const auto fy = get_y (f.get_point_records ());
        const auto fz = get_z (f.get_point_records ());
        const auto gx = get_x (g.get_point_records ());
        const auto gy = get_y (g.get_point_records ());
        const auto gz = get_z (g.get_point_records ());
        const auto hx = get_x (h.get_point_records ());
        const auto hy = get_y (h.get_point_records ());
        const auto hz = get_z (h.get_point_records ());
        const auto n = f.get_header ().total_points;
        const auto cfx = accumulate (begin (fx), end (fx), 0.0) / n;
        const auto cfy = accumulate (begin (fy), end (fy), 0.0) / n;
        const auto cfz = accumulate (begin (fz), end (fz), 0.0) / n;
        const auto cgx = accumulate (begin (gx), end (gx), 0.0) / n;
        const auto cgy = accumulate (begin (gy), end (gy), 0.0) / n;
        const auto cgz = accumulate (begin (gz), end (gz), 0.0) / n;
        const auto chx = accumulate (begin (hx), end (hx), 0.0) / n;
        const auto chy = accumulate (begin (hy), end (hy), 0.0) / n;
        const auto chz = accumulate (begin (hz), end (hz), 0.0) / n;
        verify (cfx != cgx);
        verify (cfy != cgy);
        verify (cfz == cgz);
        verify (cfx != chx);
        verify (cfy != chy);
        verify (cfz != chz);
    }
}

void test_tool_replace ()
{
    for (auto rgb : {true, false})
    {
        // Generate spoc files
        const size_t total_points = 100;
        const size_t extra_size = 8;
        auto f = generate_random_spoc_file (total_points, extra_size, false, rgb);
        for (size_t i = 0; i < total_points; ++i)
            for (size_t j = 0; j < extra_size; ++j)
                f[i].extra[j] = f[i].extra[j] % 5;

        for (auto v1 : { 1.0, 2.0, 3.0, 4.0, 5.0 })
        {
            for (auto v2 : { 1.0, 2.0, 3.0, 4.0, 5.0 })
            {
                for (auto c : { 'c', 'p', 'i', 'r', 'g', 'b',
                        '0', '1', '2', '3', '4', '5', '6', '7' })
                {
                    stringstream is, os;
                    write_spoc_file_uncompressed (is, f);
                    header h = read_header (is);
                    replace (is, os, h, c, v1, v2);
                    const auto g = read_spoc_file_uncompressed (os);
                }
                for (auto c : { 'x', 'y', 'z', 'q', 'w', '8'})
                {
                    bool failed = false;
                    try {
                        stringstream is, os;
                        write_spoc_file_uncompressed (is, f);
                        header h = read_header (is);
                        replace (is, os, h, c, v1, v2);
                        const auto g = read_spoc_file_uncompressed (os);
                    }
                    catch (...) { failed = true; }
                    verify (failed);
                }
            }
        }
    }
}

void test_tool_set ()
{
    for (auto rgb : {true, false})
    {
        // Generate spoc files
        auto f = generate_random_spoc_file (100, 8, false, rgb);

        for (auto c : { 'x', 'y', 'z',
                'c', 'p', 'i', 'r', 'g', 'b',
                '0', '1', '2', '3', '4', '5', '6', '7' })
        {
            stringstream is, os;
            write_spoc_file_uncompressed (is, f);
            header h = read_header (is);
            set (is, os, h, c, 123);
            const auto g = read_spoc_file_uncompressed (os);
        }
        for (auto c : { 'q', 'w', '9'})
        {
            bool failed = false;
            try {
                stringstream is, os;
                write_spoc_file_uncompressed (is, f);
                header h = read_header (is);
                set (is, os, h, c, 123);
                const auto g = read_spoc_file_uncompressed (os);
            }
            catch (...) { failed = true; }
            verify (failed);
        }
    }
}

void test_tool_subtract_min ()
{
    // Generate spoc files
    auto f = generate_random_spoc_file (100, 8, false, true);
    stringstream is, os;
    write_spoc_file_uncompressed (is, f);
    header hdr = read_header (is);
    subtract_min (is, os, hdr);
    const auto g = read_spoc_file_uncompressed (os);

    write_spoc_file_uncompressed (is, f);
    hdr = read_header (is);
    subtract_min (is, os, hdr);
    const auto h = read_spoc_file_uncompressed (os);
}

int main (int argc, char **argv)
{
    using namespace std;
    try
    {
        test_tool_quantize ();
        test_tool_recenter ();
        test_tool_replace ();
        test_tool_set ();
        test_tool_subtract_min ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
