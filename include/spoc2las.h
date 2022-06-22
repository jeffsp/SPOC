#pragma once

#include "laswriter.hpp"
#include <iostream>

namespace spoc
{

namespace spoc2las
{

struct las_writer
{
    explicit las_writer (const std::string &fn, const LASheader &lasheader)
        : laswriter (nullptr)
        , lasheader (lasheader)
    {
        laswriteopener.set_file_name(fn.c_str ());
        laswriter = laswriteopener.open(&lasheader);
        // GCOV_EXCL_START
        if (laswriter == nullptr)
            throw std::runtime_error ("Could not open LASlib laswriter");
        // GCOV_EXCL_STOP
    }
    ~las_writer ()
    {
        // GCOV_EXCL_START
        if (laswriter == nullptr)
            return;
        // GCOV_EXCL_STOP
        laswriter->update_header (&lasheader, true);
        laswriter->close();
        delete laswriter;
    }
    LASwriteOpener laswriteopener;
    LASwriter *laswriter;
    const LASheader &lasheader;

};

} // namespace spoc2las

} // namespace spoc
