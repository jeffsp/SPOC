#include "app_utils.h"
#include "spoc.h"
#include "filter.h"
#include "filter_cmd.h"
#include <cctype>
#include <iostream>
#include <stdexcept>

int main (int argc, char **argv)
{
    using namespace std;
    using namespace spoc::app_utils;
    using namespace spoc::file;
    using namespace spoc::filter_app;
    using namespace spoc::filter_cmd;
    using namespace spoc::io;

    try
    {
        // Parse command line
        const args args = get_args (argc, argv,
                string (argv[0]) + " [options] [input] [output]");

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

        // Show args
        if (args.verbose)
        {
            clog << "verbose\t" << args.verbose << endl;
            clog << "keep-classes\t" << args.keep_classes.size () << endl;
            for (const auto i : args.keep_classes)
                clog << "\t" << i << endl;
            clog << "random-seed\t" << args.random_seed << endl;
            clog << "remove-classes\t" << args.remove_classes.size () << endl;
            for (const auto i : args.remove_classes)
                clog << "\t" << i << endl;
            clog << "unique-xyz\t" << args.unique_xyz << endl;
            clog << "save-voxel-indexes\t" << args.save_voxel_indexes << endl;
            clog << "subsample\t" << args.subsample << endl;
            clog << "input-filename\t" << args.input_fn << endl;
            clog << "output-filename\t" << args.output_fn << endl;
        }

        // Check the arguments
        if (args.save_voxel_indexes == true && args.subsample <= 0.0)
            throw runtime_error ("The 'save-voxel-indexes' parameter may only be set when 'subsample' is > 0.0");

        // Get the input stream
        input_stream is (args.verbose, args.input_fn);

        // Read the input file
        spoc_file f = read_spoc_file (is ());

        // Process it
        if (!args.keep_classes.empty ())
            f = keep_classes (f, args.keep_classes);
        if (!args.remove_classes.empty ())
            f = remove_classes (f, args.remove_classes);
        if (args.unique_xyz)
            f = unique_xyz (f, args.random_seed);
        if (args.subsample > 0.0)
            f = subsample (f,
                args.subsample,
                args.random_seed,
                args.save_voxel_indexes);

        // Get the output stream
        output_stream os (args.verbose, args.output_fn);

        // Write it out
        write_spoc_file (os (), f);

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
