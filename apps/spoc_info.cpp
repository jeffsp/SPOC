#include "json.h"
#include "spoc.h"
#include "spoc_info_cmd.h"
#include "json.h"
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>

template<typename T,typename U>
spoc::json::object get_summary (const U &x)
{
    // Return value
    spoc::json::object s;

    // Handle the empty case
    if (x.empty ())
    {
        s["size"] = 0;
        s["min"] = 0;
        s["max"] = 0;
        return s;
    }

    // Copy
    auto y (x);

    // Get Quantiles
    const size_t n = y.size ();
    std::sort (begin (y), end (y));
    s["range"] = y.back() - y.front();
    s["q0"] = y.front();
    s["q1"] = y[n / 4];
    s["q2"] = y[n / 2];
    s["q3"] = y[3 * n / 4];
    s["q4"] = y.back();
    return s;
}

int main (int argc, char **argv)
{
    using namespace std;
    using namespace spoc;

    try
    {
        // Parse command line
        cmd::args args = cmd::get_args (argc, argv,
                string (argv[0]) + " [options] < spocfile");

        // If you are getting help, exit without an error
        if (args.help)
            return 0;

        // Show args
        if (args.verbose)
        {
            clog << "verbose\t" << args.verbose << endl;
            clog << "json\t " << args.json << endl;
            clog << "header-info\t " << args.header_info << endl;
            clog << "summary-info\t " << args.summary_info << endl;
        }

        if (args.verbose)
            clog << "Reading from stdin" << endl;

        // Read into spoc_file struct
        spoc_file f = read_spoc_file (cin);

        cout.precision (15);
        cout << fixed;

        if (args.json)
        {
            json::object j;

            if (args.header_info)
            {
                json::object h;
                h["major_version"] = f.get_major_version ();
                h["minor_version"] = f.get_minor_version ();
                h["wkt"] = f.get_wkt ();
                h["npoints"] = f.get_npoints ();
                j["header"] = h;
            }

            if (args.summary_info)
            {
                for (size_t i = 0; i < f.get_npoints(); ++i)
                {
                    // Get the point record
                    const auto p = f.get (i);
                }
                json::object s;

                s["x"] = get_summary<double> (f.get_x ());
                s["y"] = get_summary<double> (f.get_y ());
                s["z"] = get_summary<double> (f.get_z ());
                s["c"] = get_summary<uint16_t> (f.get_c ());
                s["p"] = get_summary<uint16_t> (f.get_p ());
                s["i"] = get_summary<uint16_t> (f.get_i ());
                s["r"] = get_summary<uint16_t> (f.get_r ());
                s["g"] = get_summary<uint16_t> (f.get_g ());
                s["b"] = get_summary<uint16_t> (f.get_b ());
                json::array a;
                for (size_t k = 0; k < f.get_extra ().size (); ++k)
                    a.push_back (get_summary<uint64_t> (f.get_extra ()[k]));
                s["extra"] = a;
                j["summary"] = s;
            }

            json::pretty_print (cout, j, 0);
        }
        else
        {
            /*
            for (auto i1 = j.begin(); i1 != j.end(); ++i1)
            {
                cout << i1.key() << endl;
                for (auto i2 = i1->begin(); i2 != i1->end(); ++i2)
                {
                    cout << "\t" << i2.key() << "\t" << i2.value() << endl;
                }
            }
            */
        }

        cout << endl;

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
