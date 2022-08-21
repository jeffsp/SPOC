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
    header h = f.get_header ();
    VERIFY (f.is_valid ());
    }

    // Test push_back
    {
    spoc_file f;
    f.push_back (spoc::point_record::point_record (0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0));
    header h = f.get_header ();
    VERIFY (h.total_points == 1);
    }

    // Test set_point_records
    {
    const size_t total_points = 100;
    const size_t extra_fields = 3;
    auto p = generate_random_point_records (total_points, extra_fields);
    p[10].extra.resize (extra_fields + 1);
    VERIFY_THROWS (spoc_file f ("WKT", false, p);)
    }
}

void test_spoc_file_get_set ()
{
    const size_t total_points = 100;
    const size_t extra_fields = 3;
    auto p = generate_random_point_records (total_points, extra_fields);
    spoc_file f ("WKT", false, p);

    VERIFY (f.get_wkt () == "WKT");
    f.set_wkt ("test");
    VERIFY (f.get_wkt () == "test");

    VERIFY (f.get_compressed () == false);
    f.set_compressed (true);
    VERIFY (f.get_compressed () == true);
    f.set_compressed (false);
    VERIFY (f.get_compressed () == false);

    p[10].extra.resize (extra_fields + 1);
    VERIFY_THROWS (f.set_point_records (p);)

    auto q = p[0];
    q.extra.resize (q.extra.size () + 1);
    VERIFY_THROWS (f.set_point_record (0, q);)
    VERIFY_THROWS (f.push_back (q);)

    spoc::point_record::point_records &prs = const_cast<spoc::point_record::point_records &> (f.get_point_records ());
    prs[10].extra.resize (extra_fields - 1);
    VERIFY (!f.is_valid ());
}

void test_spoc_file_header ()
{
    const size_t total_points = 100;
    const size_t extra_fields = 3;
    const string wkt ("WKT");
    auto p = generate_random_point_records (total_points, extra_fields);

    spoc_file f (wkt, false, p);
    VERIFY (f.get_header ().wkt == wkt);
    VERIFY (f.get_header ().extra_fields == extra_fields);
    VERIFY (f.get_header ().total_points == 100);
    VERIFY (f.get_header ().compressed == false);
    f.set_point_records (p);
    VERIFY (f.get_header ().wkt == wkt);
    VERIFY (f.get_header ().extra_fields == extra_fields);
    VERIFY (f.get_header ().total_points == total_points);
    VERIFY (f.get_header ().compressed == false);

    spoc_file g (wkt, true, p);
    VERIFY (g.get_header ().wkt == wkt);
    VERIFY (g.get_header ().extra_fields == extra_fields);
    VERIFY (g.get_header ().total_points == 100);
    VERIFY (g.get_header ().compressed == true);
    g.set_point_records (p);
    VERIFY (g.get_header ().wkt == wkt);
    VERIFY (g.get_header ().extra_fields == extra_fields);
    VERIFY (g.get_header ().total_points == total_points);
    VERIFY (g.get_header ().compressed == true);
}

void test_spoc_move ()
{
    const size_t total_points = 100;
    const size_t extra_fields = 3;
    auto p = generate_random_point_records (total_points, extra_fields);
    spoc_file f ("WKT", false, p);
    VERIFY (f.get_point_records ().size () == total_points);

    // Move from f to q
    auto q = f.move_point_records ();
    VERIFY (f.get_point_records ().empty ());
    VERIFY (q.size () == total_points);

    // Can't move from invalid point records
    q[10].extra.resize (extra_fields + 1);
    VERIFY_THROWS (f.move_point_records (q);)
    q[10].extra.resize (extra_fields);

    // Move from q to f
    f.move_point_records (q);
    VERIFY (q.empty ());
    VERIFY (f.get_point_records ().size () == total_points);
}

int main (int argc, char **argv)
{
    try
    {
        test_spoc_file ();
        test_spoc_file_get_set ();
        test_spoc_file_header ();
        test_spoc_move ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
