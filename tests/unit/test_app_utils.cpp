#include "app_utils.h"
#include "test_utils.h"
#include <iostream>
#include <sstream>
#include <stdexcept>

using namespace std;
using namespace spoc::app_utils;

void test_input_stream ()
{
    for (auto verbose : {true, false})
    {
        // Empty filename
        {
            auto fn = string ();
            stringstream log;
            input_stream is (verbose, fn, log);
        }
        // Invalid filename
        {
            auto fn = string ("////INVALID!!!////");
            stringstream log;
            bool failed = false;
            try { input_stream is (verbose, fn, log); }
            catch (...) { failed = true; }
            verify (failed);
        }
        // Use stdin
        {
            stringstream log;
            input_stream is (verbose, string (), log);
            auto &tmp = is ();
            (void)tmp; // Disable unused variable warning
        }
    }
}

void test_output_stream ()
{
    for (auto verbose : {true, false})
    {
        // Empty filename
        {
            auto fn = string ();
            stringstream log;
            output_stream os (verbose, fn, log);
        }
        // Invalid filename
        {
            auto fn = string ("////INVALID!!!////");
            stringstream log;
            bool failed = false;
            try { output_stream os (verbose, fn, log); }
            catch (...) { failed = true; }
            verify (failed);
        }
        // Use stdout
        {
            stringstream log;
            output_stream os (verbose, string (), log);
            auto &tmp = os ();
            (void)tmp; // Disable unused variable warning
        }
    }
}

void test_consume ()
{
    {
    string f ("x");
    auto g = consume_field_name (f);
    verify (f.empty ());
    verify (g == "x");

    bool failed = false;
    try {
        string h ("q");
        consume_field_name (h); }
    catch (...) { failed = true; }
    verify (failed);
    }

    {
    string f ("123");
    auto g = consume_int (f);
    verify (f.empty ());
    verify (g == 123);

    bool failed = false;
    try {
        string h ("q");
        consume_int (h); }
    catch (...) { failed = true; }
    verify (failed);
    }

    {
    string f ("1.23");
    auto g = consume_double (f);
    verify (f.empty ());
    verify (int(g) == 1);

    bool failed = false;
    try {
        string h ("q");
        consume_double (h); }
    catch (...) { failed = true; }
    verify (failed);
    }
}

int main (int argc, char **argv)
{
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
