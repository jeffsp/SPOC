#pragma once

#include "spoc.h"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>

namespace spoc
{

namespace transformer
{

template<typename T,typename U,typename V>
int transformer_entry (const bool verbose,
    const std::string &input_fn,
    const std::string &output_fn,
    T init_function,
    U preprocess_function,
    V process_function)
{
    using namespace std;
    using namespace spoc;

    try
    {
        // Read the input file
        spoc_file f;

        if (input_fn.empty ())
        {
            if (verbose)
                clog << "Reading from stdin" << endl;

            // Read into spoc_file struct
            f = read_spoc_file (cin);
        }
        else
        {
            if (verbose)
                clog << "Reading " << input_fn << endl;

            ifstream ifs (input_fn);

            if (!ifs)
                throw runtime_error ("Could not open file for reading");

            // Read into spoc_file struct
            f = read_spoc_file (ifs);
        }

        // Initialize the transformer
        init_function (f.get_npoints ());

        // Preprocess the points
        for (size_t i = 0; i < f.get_npoints (); ++i)
        {
            // Get a point
            const auto p = f.get (i);

            // Run it through the preprocessor
            preprocess_function (i, p);
        }

        // Process the points one by one
        for (size_t i = 0; i < f.get_npoints (); ++i)
        {
            // Get a point
            const auto p = f.get (i);

            // Run it through the transformer
            const auto q = process_function (i, p);

            // Set the transformed point
            f.set (i, q);
        }

        // Write the points back out
        if (output_fn.empty ())
        {
            if (verbose)
                clog << "Writing to stdout" << endl;

            // Write it out
            spoc::write_spoc_file (cout, f);
        }
        else
        {
            if (verbose)
                clog << "Writing " << output_fn << endl;

            ofstream ofs (output_fn);

            if (!ofs)
                throw runtime_error ("Could not open file for writing");

            // Write it out
            spoc::write_spoc_file (ofs, f);
        }

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}

} // namespace transformer

} // namespace spoc
