#include "spoc.h"
#include "spoc_info_cmd.h"
#include "json.h"
#include <iostream>
#include <stdexcept>
#include <string>

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

        for (size_t i = 0; i < f.get_npoints(); ++i)
        {
            // Get the point record
            const auto p = f.get (i);
        }

        j["points"]["x"] = f.get_x ().size ();
        j["points"]["y"] = f.get_y ().size ();
        j["points"]["z"] = f.get_z ().size ();

        if (args.json)
            cout << j.dump(4) << endl;
        else
        {
            for (auto i = j.begin(); i != j.end(); ++i)
            {
                cout << i.key() << endl;
                for (auto k = i->begin(); k != i->end(); ++k)
                {
                    cout << "\t" << k.key() << "\t" << k.value() << endl;
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
