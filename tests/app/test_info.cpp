#include "info.h"
#include "spoc/spoc.h"
#include "spoc/test_utils.h"
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace spoc::info_app;
using namespace spoc::test_utils;

void test_info_empty ()
{
    // Generate spoc files
    auto f1 = generate_random_spoc_file (0, 0, true);
    auto f2 = generate_random_spoc_file (0, 0, false);

    for (auto json : {true, false})
    {
        for (auto header_info : {true, false})
        {
            for (auto summary_info : {true, false})
            {
                for (auto classifications : {true, false})
                {
                    for (auto metric_info : {true, false})
                    {
                        for (auto compact : {true, false})
                        {
                            for (auto quartiles : {true, false})
                            {
                                stringstream t;
                                process (t, f1, json,
                                    header_info, summary_info,
                                    classifications, metric_info,
                                    compact, quartiles);
                                process (t, f2, json,
                                    header_info, summary_info,
                                    classifications, metric_info,
                                    compact, quartiles);
                            }
                        }
                    }
                }
            }
        }
    }
}

void test_info ()
{
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
                    for (auto metric_info : {true, false})
                    {
                        for (auto compact : {true, false})
                        {
                            for (auto quartiles : {true, false})
                            {
                                stringstream t;
                                process (t, f1, json,
                                    header_info, summary_info,
                                    classifications, metric_info,
                                    compact, quartiles);
                                process (t, f2, json,
                                    header_info, summary_info,
                                    classifications, metric_info,
                                    compact, quartiles);
                            }
                        }
                    }
                }
            }
        }
    }
}

int main (int argc, char **argv)
{
    try
    {
        test_info_empty ();
        test_info ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
