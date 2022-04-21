#include "info.h"
#include "test_utils.h"
#include <any>
#include <iostream>
#include <map>
#include <vector>
#include <stdexcept>

void test_info ()
{
    using namespace std;

    // Generate some records
    const auto p = get_random_point_records (100, true);
    const auto q = get_random_point_records (100, false);

    // Generate a spoc_file
    stringstream s;
    write_spoc_file (s, string ("Text WKT"), p);
    write_spoc_file (s, string ("Text WKT"), q);
    auto f1 = spoc::read_spoc_file (s);
    auto f2 = spoc::read_spoc_file (s);

    for (auto json : {true, false})
    {
        for (auto header_info : {true, false})
        {
            for (auto summary_info : {true, false})
            {
                for (auto compact : {true, false})
                {
                    stringstream t;
                    spoc::info::process (t, f1, json, header_info, summary_info, compact);
                    spoc::info::process (t, f2, json, header_info, summary_info, compact);
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
