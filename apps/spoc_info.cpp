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

        clog << f.get_signature () << endl;
        clog << f.get_npoints () << endl;
        clog << f.get_x ().size () << endl;
        clog << f.get_wkt () << endl;
        if (args.header_info)
        {
            if (args.json)
            {
                /*
                nlohmann::json j;
                j["wkt"] = f.get_wkt ();
                j["npoints"] = f.get_npoints ();
                cout << j.dump(4) << endl;
                */
            }
            else
            {
                cout << "npoints\t" << f.get_npoints () << endl;
            }
        }

        for (size_t i = 0; i < f.get_npoints(); ++i)
        {
            // Get the point record
            const auto p = f.get (i);
        }

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
