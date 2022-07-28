#include "spoc/app_utils.h"
#include "spoc/test_utils.h"
#include <filesystem>
#include <iostream>
#include <sstream>
#include <stdexcept>

using namespace std;
using namespace spoc::app_utils;

void test_input_stream ()
{
    for (auto verbose : {true, false})
    {
        // Send log output here
        stringstream log;

        // Empty filename
        {
            auto fn = string ();
            input_stream is (verbose, fn, log);
        }
        // Invalid filename
        {
            auto fn = string ("////INVALID!!!////");
            VERIFY_THROWS (input_stream is (verbose, fn, log);)
        }
        // Use stdin
        {
            input_stream is (verbose, string (), log);
            auto &tmp = is ();
            (void)tmp; // Disable unused variable warning
        }
        // Filename
        {
            // Get a temp filename
            const string fn = generate_tmp_filename ();

            // Create the file
            ofstream ofs (fn);
            VERIFY (ofs.good ());
            ofs.close ();

            // Use it
            input_stream is (verbose, fn, log);

            // Cleanup
            std::filesystem::remove (fn);
        }
    }
}

void test_output_stream ()
{
    for (auto verbose : {true, false})
    {
        // Send log output here
        stringstream log;

        // Empty filename
        {
            auto fn = string ();
            output_stream os (verbose, fn, log);
        }
        // Invalid filename
        {
            auto fn = string ("////INVALID!!!////");
            VERIFY_THROWS (output_stream os (verbose, fn, log);)
        }
        // Use stdout
        {
            output_stream os (verbose, string (), log);
            auto &tmp = os ();
            (void)tmp; // Disable unused variable warning
        }
        // Filename
        {
            // Get a temp filename
            const string fn = generate_tmp_filename ();

            // Use it
            output_stream os (verbose, fn, log);

            // Cleanup
            std::filesystem::remove (fn);
        }
    }
}

void test_consume ()
{
    {
    string f ("x");
    auto g = consume_field_name (f);
    VERIFY (f.empty ());
    VERIFY (g == "x");

    VERIFY_THROWS ( string h ("q"); consume_field_name (h); )
    }

    {
    string f ("123");
    auto g = consume_int (f);
    VERIFY (f.empty ());
    VERIFY (g == 123);

    VERIFY_THROWS ( string h ("q"); consume_int (h); )
    }

    {
    string f ("1.23");
    auto g = consume_double (f);
    VERIFY (f.empty ());
    VERIFY (int(g) == 1);

    VERIFY_THROWS ( string h ("q"); consume_double (h); )
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
