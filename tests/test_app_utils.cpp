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
