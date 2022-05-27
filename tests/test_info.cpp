#include "info.h"
#include "test_utils.h"
#include <iostream>
#include <stdexcept>

void test_info ()
{
    using namespace std;

    // Generate spoc files
    auto f1 = generate_random_spoc_file (100, 5, true);
    auto f2 = generate_random_spoc_file (100, 5, false);

    for (auto json : {true, false})
    {
        for (auto header_info : {true, false})
        {
            for (auto summary_info : {true, false})
            {
                for (auto classifications : {true, false})
                {
                    for (auto compact : {true, false})
                    {
                        stringstream t;
                        spoc::info::process (t, f1, json,
                            header_info, summary_info,
                            classifications, compact);
                        spoc::info::process (t, f2, json,
                            header_info, summary_info,
                            classifications, compact);
                    }
                }
            }
        }
    }
}

int main (int argc, char **argv)
{
    using namespace std;
    try
    {
        test_info ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
