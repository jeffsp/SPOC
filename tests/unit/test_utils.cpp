#include "contracts.h"
#include "test_utils.h"
#include "utils.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <unordered_set>

using namespace std;
using namespace spoc::utils;
using namespace spoc::test_utils;

template<typename T>
void test_hash_value ()
{
    // Different seeds should produce different hashes
    size_t seed1 = 1;
    size_t seed2 = 2;
    T h1 = hash_value (seed1, 123);
    T h2 = hash_value (seed2, 123);
    VERIFY (h1 != h2);

    // Hash a bunch of values
    std::unordered_set<T> s;
    const T total = 1000;
    for (T v = 0; v < total; ++v)
    {
        size_t seed = 0;
        s.insert (hash_value (seed, v));
    }

    // No two should be the same
    VERIFY (s.size () == total);
}

template<typename T>
void test_hash_combine ()
{
    T a = 1;
    T b = 2;
    T c = 3;
    size_t h1 = 0;
    hash_combine (h1, a, b, c);
    size_t h2 = 0;
    hash_combine (h2, a, b, c);
    size_t h3 = 0;
    hash_combine (h3, c, b, a);
    size_t h4 = 0;
    hash_combine (h4, a, b);
    VERIFY (h1 == h2);
    VERIFY (h1 != h3);
    VERIFY (h1 != h4);
    VERIFY (h2 != h3);
    VERIFY (h3 != h4);
}

void test_quantize ()
{
    auto q = generate_points (10);
    quantize (q, 0.001);
    quantize (q, 0.1);
    quantize (q, 1.0);
}

int main (int argc, char **argv)
{
    try
    {
        test_hash_value<int> ();
        test_hash_value<float> ();
        test_hash_value<double> ();
        test_hash_combine<int> ();
        test_hash_combine<float> ();
        test_hash_combine<double> ();
        test_quantize ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
