#pragma once

#include "lasreader.hpp"
#include <iostream>

namespace spoc
{

namespace las2spoc_app
{

struct las_reader
{
    explicit las_reader (const std::string &fn)
        : lasreader (nullptr)
    {
        lasreadopener.set_file_name (fn.c_str ());
        lasreader = lasreadopener.open ();
        // GCOV_EXCL_START
        if (lasreader == nullptr)
            throw std::runtime_error ("Could not open LASlib lasreader");
        // GCOV_EXCL_STOP
    }
    ~las_reader ()
    {
        // GCOV_EXCL_START
        if (lasreader == nullptr)
            return;
        // GCOV_EXCL_STOP
        lasreader->close();
        delete lasreader;
    }
    LASreadOpener lasreadopener;
    LASreader *lasreader;
};

} // namespace las2spoc_app

} // namespace spoc
