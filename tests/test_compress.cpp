#include "compress.h"
#include "test_utils.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <zlib.h>

void test_compress (const char *fn)
{
    using namespace std;

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
    string s ((std::istreambuf_iterator<char>(ifs)),
              std::istreambuf_iterator<char>());

    datasets.push_back (vector<uint8_t> (s.begin (), s.end ()));

    // Compress them all at all compression levels
    for (auto l : levels)
    {
        for (const auto &x : datasets)
        {
            // Vector interface
            {
            const auto y = spoc::compress (x, l);
            const auto z = spoc::decompress (y);
            verify (x == z);
            }

            // Stream interface
            {
            stringstream is1 (string (x.begin (), x.end ()));
            stringstream os1;
            spoc::compress (is1, os1, l);
            stringstream is2 (os1.str ());
            stringstream os2;
            spoc::decompress (is2, os2);
            const auto s = os2.str ();
            vector<uint8_t> z (s.begin (), s.end ());
            verify (x == z);
            }
        }
    }
}

int main (int argc, char **argv)
{
    using namespace std;

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
