#include "spoc/benchmark_utils.h"
#include "spoc/test_utils.h"
#include <iostream>
#include <stdexcept>
#include <thread>

using namespace std;
using namespace spoc::benchmark_utils;
using namespace std::chrono_literals;

void test_timer ()
{
    timer t;
    t.start ();
    const auto e1 = t.elapsed_ms ();
    std::this_thread::sleep_for (200ms);
    t.stop ();
    const auto e2 = t.elapsed_ms ();
    VERIFY (e1 != e2);
}

int main (int argc, char **argv)
{
    try
    {
        test_timer ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
