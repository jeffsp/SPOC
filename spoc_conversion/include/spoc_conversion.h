#pragma once

#include "spoc.h"
#include "lasreader.hpp"
#include <iostream>

namespace spoc_conversion
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

inline spoc::file::spoc_file read_las_file (const std::string &fn,
    const bool read_extra_fields = false)
{
    using namespace spoc::file;
    using namespace spoc::header;
    using namespace spoc::point_record;

    // Try to read the las file
    las_reader l (fn);

    // Get the wkt
    std::string wkt;

    // Check the coordinate system
    if (l.lasreader->header.vlr_geo_ogc_wkt != nullptr)
        wkt = std::string (l.lasreader->header.vlr_geo_ogc_wkt);

    // Read the points
    const size_t total_points = l.lasreader->npoints;
    point_records prs;
    for (size_t i = 0; i < total_points; ++i)
    {
        if (!l.lasreader->read_point())
            throw std::runtime_error (std::string ("Error reading point record #") + std::to_string (i));

        point_record p;
        p.x = l.lasreader->point.get_x();
        p.y = l.lasreader->point.get_y();
        p.z = l.lasreader->point.get_z();
        p.c = l.lasreader->point.get_classification();
        p.p = l.lasreader->point.get_point_source_ID();
        p.i = l.lasreader->point.get_intensity();
        p.r = l.lasreader->point.rgb[0];
        p.g = l.lasreader->point.rgb[1];
        p.b = l.lasreader->point.rgb[2];

        // If we are reading extra fields, put the return number in
        // extra[0], and put the scan_angle_rank in extra[1].
        if (read_extra_fields)
        {
            p.extra.resize (2);
            p.extra[0] = l.lasreader->point.get_return_number();
            p.extra[1] = l.lasreader->point.get_scan_angle_rank();
        }
        prs.push_back (p);
    }

    const bool compressed = false;
    return spoc_file (wkt, compressed, prs);
}

inline spoc::file::spoc_file read_spoc_or_las_file (const std::string &fn,
    const bool read_extra_fields = false)
{
    // Try to open the file
    std::ifstream ifs (fn);

    if (!ifs)
        throw std::runtime_error ("Could not open file for reading");

    // Try to read the spoc file
    try { return spoc::io::read_spoc_file (ifs); }
    catch (...) { ifs.close (); }

    // If we got this far, we have failed, so try to read
    // the specified filename as a LAS file instead
    return read_las_file (fn, read_extra_fields);
}


} // namespace spoc_conversion
