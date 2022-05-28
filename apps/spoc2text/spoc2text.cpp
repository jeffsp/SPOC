#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include "spoc.h"
#include "spoc2text_cmd.h"

int main (int argc, char **argv)
{
    using namespace std;
    using namespace spoc;

    try
    {
        // Parse command line
        const cmd::args args = cmd::get_args (argc, argv,
            string (argv[0]) + " [options] < spocfile > textfile");

        // If you are getting help, exit without an error
        if (args.help)
            return 0;

        // Read spoc file
        if (args.verbose)
            clog << "reading spoc file" << endl;

        const auto f = read_spoc_file (cin);
        const auto wkt = f.get_header ().wkt;
        const auto &point_records = f.get_point_records ();

        if (args.verbose)
            clog << f.get_point_records ().size () << " point records read" << endl;

        if (args.verbose)
            clog << "read " << wkt.size () << " byte WKT" << endl;

        if (args.verbose)
            clog << "writing point records to stdout" << endl;

        // Write to stdout
        cout << wkt << endl;
        for (size_t i = 0; i < point_records.size (); ++i)
        {
            const point_record &p = point_records[i];
            cout << setprecision(std::numeric_limits<double>::digits10);
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
