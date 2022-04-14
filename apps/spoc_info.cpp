#include "spoc.h"
#include "spoc_info_cmd.h"
#include "json.h"
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>

template<typename T,typename U>
std::map<std::string,T> get_summary (const U &x)
{
    std::map<std::string,T> m;
    if (x.empty ())
    {
        m.insert ({"size", 0});
        m.insert ({"min", 0});
        m.insert ({"max", 0});
        return m;
    }
    // Copy
    auto y (x);
    const size_t n = y.size ();
    std::sort (begin (y), end (y));
    m.insert ({"range", y.back() - y.front()});
    m.insert ({"q0", y.front()});
    m.insert ({"q1", y[n / 4]});
    m.insert ({"q2", y[n / 2]});
    m.insert ({"q3", y[3 * n / 4]});
    m.insert ({"q4", y.back()});
    return m;
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

        nlohmann::json j;

        if (args.header_info)
        {
            j["header"]["major_version"] = f.get_major_version ();
            j["header"]["minor_version"] = f.get_minor_version ();
            j["header"]["wkt"] = f.get_wkt ();
            j["header"]["npoints"] = f.get_npoints ();
        }

        if (args.summary_info)
        {
            for (size_t i = 0; i < f.get_npoints(); ++i)
            {
                // Get the point record
                const auto p = f.get (i);
            }

            j["summary"]["x"] = get_summary<double> (f.get_x ());
            j["summary"]["y"] = get_summary<double> (f.get_y ());
            j["summary"]["z"] = get_summary<double> (f.get_z ());
            j["summary"]["c"] = get_summary<uint16_t> (f.get_c ());
            j["summary"]["p"] = get_summary<uint16_t> (f.get_p ());
            j["summary"]["i"] = get_summary<uint16_t> (f.get_i ());
            j["summary"]["r"] = get_summary<uint16_t> (f.get_r ());
            j["summary"]["g"] = get_summary<uint16_t> (f.get_g ());
            j["summary"]["b"] = get_summary<uint16_t> (f.get_b ());
            for (size_t k = 0; k < f.get_extra ().size (); ++k)
                j["summary"]["extra"][std::to_string(k)] = get_summary<uint64_t> (f.get_extra ()[k]);
        }

        if (args.json)
        {
            cout << j.dump(4) << endl;
        }
        else
        {
            for (auto i1 = j.begin(); i1 != j.end(); ++i1)
            {
                cout << i1.key() << endl;
                for (auto i2 = i1->begin(); i2 != i1->end(); ++i2)
                {
                    cout << "\t" << i2.key() << "\t" << i2.value() << endl;
                }
            }
        }

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
