#include "spoc/app_utils.h"
#include "spoc/spoc.h"
#include "spoc2text.h"
#include "spoc2text_cmd.h"
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>

int main (int argc, char **argv)
{
    using namespace std;
    using namespace spoc::io;
    using namespace spoc::spoc2text_cmd;

    try
    {
        // Parse command line
        const args args = get_args (argc, argv,
            string (argv[0]) + " [options] < spocfile > textfile");

        // If version was requested, print it and exit
        if (args.version)
        {
            cout << "Version "
                << static_cast<int> (spoc::MAJOR_VERSION)
                << "."
                << static_cast<int> (spoc::MINOR_VERSION)
                << endl;
            return 0;
        }

        // If you are getting help, exit without an error
        if (args.help)
            return 0;

        // Read spoc file
        if (args.verbose)
            clog << "Reading spoc file from stdin" << endl;

        const auto f = read_spoc_file (cin);
        const auto wkt = f.get_header ().wkt;
        const auto &point_records = f.get_point_records ();

        if (args.verbose)
        {
            clog << f.get_point_records ().size () << " point records read" << endl;

            clog << "read " << wkt.size () << " byte WKT" << endl;

            clog << "Writing point records to stdout" << endl;
        }

        // Write to stdout
        cout << wkt << endl;
        for (size_t i = 0; i < point_records.size (); ++i)
        {
            const spoc::point_record::point_record &p = point_records[i];
            cout << setprecision(numeric_limits<double>::digits10);
            cout << p.x;
            cout << '\t' << p.y;
            cout << '\t' << p.z;
            cout << '\t' << p.c;
            cout << '\t' << p.p;
            cout << '\t' << p.i;
            cout << '\t' << p.r;
            cout << '\t' << p.g;
            cout << '\t' << p.b;
            cout << endl;
        }

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
