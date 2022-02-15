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

int main (int argc, char **argv)
{
    using namespace std;

    try
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
        const size_t N = 10'000'000;
        vector<uint8_t> x (N);
        default_random_engine g;
        uniform_int_distribution<int> b (0, 255);
        for (auto &y : x)
            y = b (g);

        // Save it
        datasets.push_back (x);

        // This executable
        ifstream ifs (argv[0]);
        if (!ifs)
            throw runtime_error ("Could not open file for reading");
        auto ss = ostringstream {};
        ss << ifs.rdbuf();
        datasets.push_back (vector<uint8_t> (ss.str ().begin (), ss.str ().end ()));

        // Compress them all at all compression levels
        for (auto l : levels)
        {
            for (const auto &x : datasets)
            {
                // Vector interface
                {
                const auto y = spc::compress (x, l);
                const auto z = spc::decompress (y);
                verify (x == z);
                }

                // Stream interface
                {
                stringstream is1 (string (x.begin (), x.end ()));
                stringstream os1;
                spc::compress (is1, os1, l);
                stringstream is2 (os1.str ());
                stringstream os2;
                spc::decompress (is2, os2);
                const auto s = os2.str ();
                vector<uint8_t> z (s.begin (), s.end ());
                verify (x == z);
                }
            }
        }

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
