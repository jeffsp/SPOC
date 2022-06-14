#include "transform.h"
#include "test_utils.h"
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace spoc;
using namespace spoc::transform;

void test_transform_quantize ()
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

    clog << fixed;
    auto q = generate_points (10);
    quantize (q, 0.001);
    quantize (q, 0.1);
    quantize (q, 1.0);
}

void test_transform_replace ()
{
    for (auto rgb : {true, false})
    {
        // Generate spoc files
        const size_t total_points = 100;
        const size_t extra_fields = 8;
        auto f = generate_random_spoc_file (total_points, extra_fields, false, rgb);
        for (size_t i = 0; i < total_points; ++i)
            for (size_t j = 0; j < extra_fields; ++j)
                f[i].extra[j] = f[i].extra[j] % 5;

        for (auto v1 : { 1.0, 2.0, 3.0, 4.0, 5.0 })
        {
            for (auto v2 : { 1.0, 2.0, 3.0, 4.0, 5.0 })
            {
                for (auto fn : { "c", "p", "i", "r", "g", "b",
                        "e0", "e1", "e2", "e3", "e4", "e5", "e6", "e7" })
                {
                    stringstream is, os;
                    write_spoc_file_uncompressed (is, f);
                    header h = read_header (is);
                    replace (is, os, h, fn, v1, v2);
                    const auto g = read_spoc_file_uncompressed (os);
                }
                for (auto fn : { "x", "y", "z", "q", "w", "e8"})
                {
                    bool failed = false;
                    try {
                        stringstream is, os;
                        write_spoc_file_uncompressed (is, f);
                        header h = read_header (is);
                        replace (is, os, h, fn, v1, v2);
                        const auto g = read_spoc_file_uncompressed (os);
                    }
                    catch (...) { failed = true; }
                    verify (failed);
                }
            }
        }
    }
}

void test_transform_set ()
{
    for (auto rgb : {true, false})
    {
        // Generate spoc files
        auto f = generate_random_spoc_file (100, 8, false, rgb);

        for (auto c : { "x", "y", "z",
                "c", "p", "i", "r", "g", "b",
                "e0", "e1", "e2", "e3", "e4", "e5", "e6", "e7" })
        {
            stringstream is, os;
            write_spoc_file_uncompressed (is, f);
            header h = read_header (is);
            set (is, os, h, c, 123);
            const auto g = read_spoc_file_uncompressed (os);
        }
        for (auto c : { "q", "w", "e9"})
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

int main (int argc, char **argv)
{
    using namespace std;
    try
    {
        test_transform_quantize ();
        test_transform_replace ();
        test_transform_set ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
