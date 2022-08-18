#include "spoc/app_utils.h"
#include "spoc/file.h"
#include "spoc/header.h"
#include "spoc/io.h"
#include "spoc/test_utils.h"
#include <iostream>
#include <random>
#include <vector>
#include <stdexcept>

using namespace std;
using namespace spoc::app_utils;
using namespace spoc::file;
using namespace spoc::header;
using namespace spoc::io;

void test_spoc_file ()
{
    // Test spoc_file::is_valid()
    {
    spoc_file f;
    // Force header to be invalid. Don't ever do this.
    header &h = const_cast<header &> (f.get_header ());
    VERIFY (f.is_valid ());
    h.total_points = 1;
    VERIFY (!f.is_valid ());
    }

    // Test push_back
    {
    spoc_file f;
    f.push_back (spoc::point_record::point_record (0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0));
    header &h = const_cast<header &> (f.get_header ());
    VERIFY (f.is_valid ());
    h.extra_fields = 1;
    VERIFY (!f.is_valid ());
    }

    // Test set_point_records
    const size_t total_points = 100;
    const size_t extra_fields = 3;
    auto p = generate_random_point_records (total_points, extra_fields);
    p[10].extra.resize (extra_fields + 1);

    spoc_file f ("WKT", extra_fields, false);
    VERIFY_THROWS (f.set_point_records (p);)

    p[10].extra.resize (extra_fields - 1);
    spoc_file g ("WKT", extra_fields + 1, false);
    VERIFY_THROWS (g.set_point_records (p);)
}

void test_spoc_file_header ()
{
    const size_t total_points = 100;
    const size_t extra_fields = 3;
    const string wkt ("WKT");
    auto p = generate_random_point_records (total_points, extra_fields);

    spoc_file f (wkt, extra_fields);
    VERIFY (f.get_header ().wkt == wkt);
    VERIFY (f.get_header ().extra_fields == extra_fields);
    VERIFY (f.get_header ().total_points == 0);
    VERIFY (f.get_header ().compressed == false);
    f.set_point_records (p);
    VERIFY (f.get_header ().wkt == wkt);
    VERIFY (f.get_header ().extra_fields == extra_fields);
    VERIFY (f.get_header ().total_points == total_points);
    VERIFY (f.get_header ().compressed == false);

    spoc_file g (wkt, extra_fields, true);
    VERIFY (g.get_header ().wkt == wkt);
    VERIFY (g.get_header ().extra_fields == extra_fields);
    VERIFY (g.get_header ().total_points == 0);
    VERIFY (g.get_header ().compressed == true);
    g.set_point_records (p);
    VERIFY (g.get_header ().wkt == wkt);
    VERIFY (g.get_header ().extra_fields == extra_fields);
    VERIFY (g.get_header ().total_points == total_points);
    VERIFY (g.get_header ().compressed == true);
}

int main (int argc, char **argv)
{
    try
    {
        test_spoc_file ();
        test_spoc_file_header ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
