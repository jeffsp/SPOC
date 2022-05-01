#pragma once

#include "spoc.h"
#include <cassert>
#include <iostream>
#include <limits>
#include <vector>
#include <unordered_map>

namespace spoc
{

namespace transform
{

spoc_file run_set_command (const spoc_file &f,
    const bool verbose,
    const char field_name,
    const double v)
{
    // Get number of points
    const size_t n = f.get_npoints ();

    // Return value
    spoc_file g;

    // Set the values
    switch (field_name)
    {
        default:
        {
            std::string s ("Unknown field name '");
            s += field_name;
            s += "'";
            throw std::runtime_error (s);
        }
        case 'x': for (size_t i = 0; i < n; ++i) g.set_x (i, v); break;
        case 'y': for (size_t i = 0; i < n; ++i) g.set_y (i, v); break;
        case 'z': for (size_t i = 0; i < n; ++i) g.set_z (i, v); break;
        case 'c': for (size_t i = 0; i < n; ++i) g.set_c (i, v); break;
        case 'p': for (size_t i = 0; i < n; ++i) g.set_p (i, v); break;
        case 'i': for (size_t i = 0; i < n; ++i) g.set_i (i, v); break;
        case 'r': for (size_t i = 0; i < n; ++i) g.set_r (i, v); break;
        case 'g': for (size_t i = 0; i < n; ++i) g.set_g (i, v); break;
        case 'b': for (size_t i = 0; i < n; ++i) g.set_b (i, v); break;
        case '0': case '1': case '2': case '3':
        case '4': case '5': case '6': case '7':
        {
              const size_t j = field_name - '0';
              for (size_t i = 0; i < n; ++i)
                  g.set_extra (i, j, v);
              break;
        }
    }

    return g;
}

template<typename T>
spoc_file run_replace_command (const spoc_file &f,
    const bool verbose,
    const char field_name,
    const T &replace_pairs)
{
    // Get number of points
    const size_t n = f.get_npoints ();

    // Return value
    spoc_file g;

    // Do each replacement in order
    for (auto r : replace_pairs)
    {
        // Get the pair values
        const auto v1 = r.first;
        const auto v2 = r.second;
        switch (field_name)
        {
            default:
            {
                std::string s ("Unknown field name '");
                s += field_name;
                s += "'";
                throw std::runtime_error (s);
            }
            case 'x': for (size_t i = 0; i < n; ++i) if (g.get_x ()[i] == v1) g.set_x (i, v2); break;
            case 'y': for (size_t i = 0; i < n; ++i) if (g.get_y ()[i] == v1) g.set_y (i, v2); break;
            case 'z': for (size_t i = 0; i < n; ++i) if (g.get_z ()[i] == v1) g.set_z (i, v2); break;
            case 'c': for (size_t i = 0; i < n; ++i) if (g.get_c ()[i] == v1) g.set_c (i, v2); break;
            case 'p': for (size_t i = 0; i < n; ++i) if (g.get_p ()[i] == v1) g.set_p (i, v2); break;
            case 'i': for (size_t i = 0; i < n; ++i) if (g.get_i ()[i] == v1) g.set_i (i, v2); break;
            case 'r': for (size_t i = 0; i < n; ++i) if (g.get_r ()[i] == v1) g.set_r (i, v2); break;
            case 'g': for (size_t i = 0; i < n; ++i) if (g.get_g ()[i] == v1) g.set_g (i, v2); break;
            case 'b': for (size_t i = 0; i < n; ++i) if (g.get_b ()[i] == v1) g.set_b (i, v2); break;
            case '0': case '1': case '2': case '3':
            case '4': case '5': case '6': case '7':
            {
                  const size_t j = field_name - '0';
                  for (size_t i = 0; i < n; ++i)
                      if (g.get_extra ()[i][j] == v1)
                          g.set_extra (i, j, v2);
                  break;
            }
        }
    }

    return g;
}

// Process a spoc_file using named pipes
spoc::spoc_file process_with_pipes (const spoc_file &f, std::ostream &ips, std::istream &ops)
{
    spoc::spoc_file g (f);

    // Write a header
    write_binary_header (ips,
        f.get_major_version (),
        f.get_minor_version (),
        f.get_npoints ());

    // For each record
    for (size_t i = 0; i < f.get_npoints (); ++i)
    {
        // Get the record
        auto p = f.get (i);

        // Write the record to the filter's input pipe
        spoc::write_binary (ips, p);

        // Read the transformed record from the filter's output pipe
        const spoc::point_record q = spoc::read_binary (ops);

        // Set it in the new spoc_file
        g.set (i, q);
    }

    return g;
}

spoc_file run_pipe_command (const spoc_file &f,
    const bool verbose,
    const std::string &input_pipe_name,
    const std::string &output_pipe_name)
{
    using namespace std;

    // Open the pipes
    if (input_pipe_name.empty ())
        throw runtime_error ("No input pipe name was specified");

    if (output_pipe_name.empty ())
        throw runtime_error ("No output pipe name was specified");

    // The transformer writes to the filter's input pipe
    if (verbose)
        clog << "Opening " << input_pipe_name << " for writing" << endl;

    ofstream ips (input_pipe_name);

    if (!ips)
        throw runtime_error ("Could not open file for writing");

    // The transformer reads from the filter's output pipe
    if (verbose)
        clog << "Opening " << output_pipe_name << " for reading" << endl;

    ifstream ops (output_pipe_name);

    if (!ops)
        throw runtime_error ("Could not open file for reading");

    if (verbose)
        clog << "Processing " << f.get_npoints () << " point records" << endl;

    spoc_file g = process_with_pipes (f, ips, ops);

    return g;
}

} // namespace transform

} // namespace spoc
