#include "app_utils.h"
#include "test_utils.h"
#include <iostream>
#include <sstream>
#include <stdexcept>

void test_input_stream ()
{
    for (auto verbose : {true, false})
    {
        // Empty filename
        {
            auto fn = std::string ();
            std::stringstream log;
            spoc::app_utils::input_stream is (verbose, fn, log);
        }
        // Invalid filename
        {
            auto fn = std::string ("////INVALID!!!////");
            std::stringstream log;
            bool failed = false;
            try { spoc::app_utils::input_stream is (verbose, fn, log); }
            catch (...) { failed = true; }
            verify (failed);
        }
        // Use stdin
        spoc::app_utils::input_stream is (verbose, std::string ());
        auto &tmp = is ();
        (void)tmp; // Disable unused variable warning
    }
}

void test_output_stream ()
{
    for (auto verbose : {true, false})
    {
        // Empty filename
        {
            auto fn = std::string ();
            std::stringstream log;
            spoc::app_utils::output_stream os (verbose, fn, log);
        }
        // Invalid filename
        {
            auto fn = std::string ("////INVALID!!!////");
            std::stringstream log;
            bool failed = false;
            try { spoc::app_utils::output_stream os (verbose, fn, log); }
            catch (...) { failed = true; }
            verify (failed);
        }
        // Use stdout
        spoc::app_utils::output_stream os (verbose, std::string ());
        auto &tmp = os ();
        (void)tmp; // Disable unused variable warning
    }
}

int main (int argc, char **argv)
{
    using namespace std;
    try
    {
        test_input_stream ();
        test_output_stream ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
