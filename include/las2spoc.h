#pragma once

#include "lasreader.hpp"
#include <iostream>

namespace spoc
{

namespace las2spoc
{

struct las_reader
{
    explicit las_reader (const std::string &fn)
        : lasreader (nullptr)
    {
        lasreadopener.set_file_name (fn.c_str ());
        lasreader = lasreadopener.open ();
        if (lasreader == nullptr)
            throw std::runtime_error ("Could not open LASlib lasreader");
    }
    ~las_reader ()
    {
        if (lasreader == nullptr)
            return;
        lasreader->close();
        delete lasreader;
    }
    LASreadOpener lasreadopener;
    LASreader *lasreader;
};

} // namespace las2spoc

} // namespace spoc
