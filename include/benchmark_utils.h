#pragma once

#include <chrono>
#include <thread>

namespace spoc
{

namespace benchmark_utils
{

class timer
{
private:
    std::chrono::time_point<std::chrono::system_clock> t1;
    std::chrono::time_point<std::chrono::system_clock> t2;
    bool running;

public:
    timer () : running (false)
    {
    }
    void start ()
    {
        t1 = std::chrono::system_clock::now ();
        running = true;
    }
    void stop ()
    {
        t2 = std::chrono::system_clock::now ();
        running = false;
    }
    double elapsed_ms()
    {
        using namespace std::chrono;
        return running
            ? duration_cast<milliseconds> (system_clock::now () - t1).count ()
            : duration_cast<milliseconds> (t2 - t1).count ();
    }
};

} // namespace benchmark_utils

} // namespace spoc
