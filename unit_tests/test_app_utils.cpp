#include "app_utils.h"
#include "test_utils.h"
#include <iostream>
#include <sstream>
#include <stdexcept>

using namespace spoc::app_utils;

void test_input_stream ()
{
    for (auto verbose : {true, false})
    {
        // Empty filename
        {
            auto fn = std::string ();
            std::stringstream log;
            input_stream is (verbose, fn, log);
        }
        // Invalid filename
        {
            auto fn = std::string ("////INVALID!!!////");
            std::stringstream log;
            bool failed = false;
            try { input_stream is (verbose, fn, log); }
            catch (...) { failed = true; }
            verify (failed);
        }
        // Use stdin
        input_stream is (verbose, std::string ());
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
            output_stream os (verbose, fn, log);
        }
        // Invalid filename
        {
            auto fn = std::string ("////INVALID!!!////");
            std::stringstream log;
            bool failed = false;
            try { output_stream os (verbose, fn, log); }
            catch (...) { failed = true; }
            verify (failed);
        }
        // Use stdout
        output_stream os (verbose, std::string ());
        auto &tmp = os ();
        (void)tmp; // Disable unused variable warning
    }
}

void test_consume ()
{
    {
    std::string f ("x");
    auto g = consume_field_name (f);
    verify (f.empty ());
    verify (g == "x");

    bool failed = false;
    try {
        std::string f ("q");
        consume_field_name (f); }
    catch (...) { failed = true; }
    verify (failed);
    }

    {
    std::string f ("123");
    auto g = consume_int (f);
    verify (f.empty ());
    verify (g == 123);

    bool failed = false;
    try {
        std::string f ("q");
        consume_int (f); }
    catch (...) { failed = true; }
    verify (failed);
    }

    {
    std::string f ("1.23");
    auto g = consume_double (f);
    verify (f.empty ());
    verify (int(g) == 1);

    bool failed = false;
    try {
        std::string f ("q");
        consume_double (f); }
    catch (...) { failed = true; }
    verify (failed);
    }
}

int main (int argc, char **argv)
{
    using namespace std;
    try
    {
        test_input_stream ();
        test_output_stream ();
        test_consume ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
