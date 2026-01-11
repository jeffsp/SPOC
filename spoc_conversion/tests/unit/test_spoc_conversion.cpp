#include "app_utils.h"
#include "spoc_conversion.h"
#include "test_utils.h"
#include <iostream>

using namespace std;
using namespace spoc::app_utils;
using namespace spoc_conversion;

void test_spoc_conversion ()
{
    const auto f1 = read_las_file ("../test_data/lidar/juarez50.las");
    const auto f2 = read_spoc_or_las_file ("../test_data/lidar/juarez50.las");
    const auto f3 = read_spoc_or_las_file ("../test_data/lidar/juarez50.spoc");
    const auto f4 = read_spoc_or_las_file ("../test_data/lidar/juarez50.zpoc");
    VERIFY (f1.is_valid ());
    VERIFY (f2.is_valid ());
    VERIFY (f3.is_valid ());
    VERIFY (f4.is_valid ());
}

void test_spoc_conversion_extra_fields ()
{
    for (auto read_extra_fields : {true, false})
    {
        const auto f1 = read_las_file ("../test_data/lidar/juarez50.las", read_extra_fields);
        const auto f2 = read_spoc_or_las_file ("../test_data/lidar/juarez50.las", read_extra_fields);
        VERIFY (f1.is_valid ());
        VERIFY (f2.is_valid ());
        const auto h1 = f1.get_header ();
        const auto h2 = f2.get_header ();
        if (read_extra_fields)
        {
            VERIFY (h1.extra_fields == 2);
            VERIFY (h2.extra_fields == 2);
        }
        else
        {
            VERIFY (h1.extra_fields == 0);
            VERIFY (h2.extra_fields == 0);
        }
    }

    // Use default for extra_fields
    const auto f1 = read_las_file ("../test_data/lidar/juarez50.las");
    const auto f2 = read_spoc_or_las_file ("../test_data/lidar/juarez50.las");
    VERIFY (f1.is_valid ());
    VERIFY (f2.is_valid ());
    const auto h1 = f1.get_header ();
    const auto h2 = f2.get_header ();
    VERIFY (h1.extra_fields == 0);
    VERIFY (h2.extra_fields == 0);
}

int main (int argc, char **argv)
{
    try
    {
        test_spoc_conversion ();
        test_spoc_conversion_extra_fields ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
