#include "spoc/compression.h"
#include "spoc/test_utils.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <zlib.h>

using namespace std;
using namespace spoc::compression;

void test_compress (const char *fn)
{
    const auto levels = {
        Z_DEFAULT_COMPRESSION,
        Z_NO_COMPRESSION,
        Z_BEST_SPEED,
        Z_BEST_COMPRESSION,
    };

    // Create datasets
    vector<vector<uint8_t>> datasets;

    // Some numbers
    datasets.push_back ({1, 2, 3, 4, 5, 6, 7, 8, 9, 0});

    // Random bytes
    const size_t N = 1'000'000;
    vector<uint8_t> x (N);
    default_random_engine g;
    uniform_int_distribution<int> b (0, 255);
    for (auto &y : x)
        y = b (g);

    // Save it
    datasets.push_back (x);

    // This executable
    ifstream ifs (fn);
    if (!ifs)
        throw runtime_error ("Could not open file for reading");
    string s ((istreambuf_iterator<char>(ifs)),
              istreambuf_iterator<char>());

    datasets.push_back (vector<uint8_t> (s.begin (), s.end ()));

    // Compress them all at all compression levels
    for (auto l : levels)
    {
        for (const auto &x : datasets)
        {
            // Vector interface
            {
            const auto y = compress (x, l);
            const auto z = decompress (y);
            VERIFY (x == z);
            }

            // Stream interface
            {
            stringstream is1 (string (x.begin (), x.end ()));
            stringstream os1;
            compress (is1, os1, l);
            stringstream is2 (os1.str ());
            stringstream os2;
            decompress (is2, os2);
            const auto s = os2.str ();
            vector<uint8_t> z (s.begin (), s.end ());
            VERIFY (x == z);
            }
        }
    }
}

int main (int argc, char **argv)
{
    try
    {
        test_compress (argv[0]);

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
