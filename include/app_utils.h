#pragma once

#include <fstream>
#include <iostream>
#include <string>

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

} // namespace app_utils

} // namespace spoc
