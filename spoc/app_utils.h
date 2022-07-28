#pragma once

#include <cassert>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_set>

namespace spoc
{

namespace app_utils
{

// Helper class to read from either cin or an input file
class input_stream
{
    public:
    input_stream (const bool verbose, const std::string &fn, std::ostream &logstream = std::clog)
    {
        if (fn.empty ())
        {
            if (verbose)
                logstream << "Reading from stdin" << std::endl;
            is = &std::cin;
        }
        else
        {
            if (verbose)
                logstream << "Reading from " << fn << std::endl;
            ifs.open (fn);
            if (!ifs)
                throw std::runtime_error ("Could not open file for reading");
            is = &ifs;
        }
    }
    std::istream &operator() ()
    {
        return *is;
    }
    private:
    std::istream *is;
    std::ifstream ifs;
};

// Helper class to write to either cout or an output file
class output_stream
{
    public:
    output_stream (const bool verbose, const std::string &fn, std::ostream &logstream = std::clog)
    {
        if (fn.empty ())
        {
            if (verbose)
                logstream << "Writing to stdout" << std::endl;
            os = &std::cout;
        }
        else
        {
            if (verbose)
                logstream << "Writing to " << fn << std::endl;
            ofs.open (fn);
            if (!ofs)
                throw std::runtime_error ("Could not open file for writing");
            os = &ofs;
        }
    }
    std::ostream &operator() ()
    {
        return *os;
    }
    private:
    std::ostream *os;
    std::ofstream ofs;
};

const std::unordered_set<std::string> field_strings {"x", "y", "z", "c", "p", "i", "r", "g", "b"};

bool is_extra_field (const std::string &f)
{
    // The field name should look like 'e#'
    return f[0] == 'e';
}

int get_extra_index (const std::string &f)
{
    if (!is_extra_field (f))
        return -1;

    // Copy the field name
    std::string e (f);

    // Skip over the 'e'
    e.erase (0, 1);

    // Convert to int
    int n;
    try { n = stoi (e); }
    catch (...) { return -1; }
    return n;
}

bool check_field_name (const std::string &f)
{
    // Does it match a string?
    if (field_strings.find (f) != field_strings.end ())
        return true;
    // Is it an 'e' followed by a number?
    if (f.size () <= 1)
        return false;
    if (f[0] != 'e')
        return false;
    // Get the index
    const int n = get_extra_index (f);
    // Is it a positive integer?
    if (n >= 0)
        return true;
    return false;
}

std::string consume_field_name (std::string &s)
{
    // Check string
    assert (!s.empty ());

    // Get the char
    char c = s[0];
    std::string field_name;
    while (isalnum (c))
    {
        field_name += c;
        // Skip over the char
        s.erase (0, 1);
        c = s[0];
    }

    // Make sure it's a valid name
    if (!check_field_name (field_name))
        throw std::runtime_error (std::string ("Invalid field name: " + field_name));

    // Skip over the char
    s.erase (0, 1);

    return field_name;
}

int consume_int (std::string &s)
{
    size_t sz = 0;
    int v = 0.0;
    try { v = std::stoi (s, &sz); }
    catch (...) { throw std::runtime_error (std::string ("Could not parse int value string: ") + s); }
    s.erase (0, sz + 1);
    return v;
}

double consume_double (std::string &s)
{
    size_t sz = 0;
    double v = 0.0;
    try { v = std::stod (s, &sz); }
    catch (...) { throw std::runtime_error (std::string ("Could not parse number value string: ") + s); }
    s.erase (0, sz + 1);
    return v;
}

} // namespace app_utils

} // namespace spoc
