#pragma once
#include "spoc/compression.h"
#include "spoc/contracts.h"
#include "spoc/header.h"
#include "spoc/point_record.h"
#include "spoc/version.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <unordered_set>
#include <vector>

namespace spoc
{

namespace file
{

/// @brief SPOC format file
class spoc_file
{
    private:
    header::header h;
    point_record::point_records p;

    public:
    /// @brief Default CTOR
    spoc_file ()
    {
    }
    /// @brief CTOR
    /// @param h Spoc file header
    spoc_file (const header::header &h)
        : h (h)
    {
    }
    /// @brief CTOR
    /// @param wkt OGC WKT string
    /// @param extra_fields Number of extra fields
    /// @param compressed Compression flag
    spoc_file (const std::string &wkt,
        const size_t extra_fields,
        const bool compressed = false)
        : h (wkt, extra_fields, 0, compressed)
    {
    }
    /// @brief CTOR
    /// @param wkt OGC WKT string
    /// @param extra_fields Number of extra fields
    /// @param compressed Compression flag
    spoc_file (const std::string &wkt,
        const size_t extra_fields,
        const bool compressed,
        const point_record::point_records &prs)
        : h (wkt, extra_fields, 0, compressed)
    {
        set_point_records (prs);
    }
    /// @brief Copy CTOR
    /// @param f Spoc file to copy
    spoc_file (const spoc_file &f)
        : h (f.h)
        , p (f.p)
    {
    }

    /// @brief Set the point records in a SPOC file
    /// @param prs The point records to set
    void set_point_records (const point_record::point_records &prs)
    {
        // Update the header
        h.total_points = prs.size ();
        // Set the points
        p = prs;
        // Make sure the points are all consistent
        check_point_records ();
    }
    /// @brief Check the current point records to make sure
    ///        that they are consistent with the header
    void check_point_records () const
    {
        if (h.total_points != p.size ())
            throw std::runtime_error ("The header total points does not match the data total points");
        if (!p.empty () && h.extra_fields != p[0].extra.size ())
            throw std::runtime_error ("The number of extra fields does not match the point records");
        if (!check_records (p, h.extra_fields))
            throw std::runtime_error ("The point records are inconsistent");
    }
    /// @brief Add a point record
    void push_back (const point_record::point_record &pr)
    {
        // Check size of extra fields
        if (h.extra_fields != pr.extra.size ())
            throw std::runtime_error ("The number of extra fields is incorrect");
        // Save the point
        p.push_back (pr);
        // Increment counter
        ++h.total_points;
    }

    /// @brief Contract support
    bool is_valid () const
    {
        if (p.size () != h.total_points)
            return false;
        if (!p.empty () && p[0].extra.size () != h.extra_fields)
            return false;
        return true;
    }

    /// @brief Readonly header access
    const header::header &get_header () const { return h; }
    /// @brief Header write access
    /// @param h The new header
    void set_header (const header::header &new_header) { h = new_header; }

    /*
    /// @brief Readonly header access
    uint8_t get_major_version () const { return h.major_version; }
    /// @brief Readonly header access
    uint8_t get_minor_version () const { return h.minor_version; }
    /// @brief Readonly header access
    std::string get_wkt () const { return h.wkt; }
    /// @brief Readonly header access
    size_t get_extra_fields () const { return h.extra_fields; }
    /// @brief Readonly header access
    size_t get_total_points () const { return h.total_points; }
    /// @brief Readonly header access
    bool get_compressed () const { return h.compressed; }

    */
    /// @brief Readonly point records access
    const point_record::point_records &get_point_records () const { return p; }
    /*
    /// @brief Readonly individual point record access
    const point_record::point_record &get_point_record (const size_t n) const { return p[n]; }

    /// @brief Header write access
    /// @param wkt The new wkt
    void set_wkt (const std::string &wkt)
    {
        h.wkt = wkt;
    }
    /// @brief Header write access
    /// @param flag The new compression flag
    void set_compressed (const bool flag)
    {
        h.compressed = flag;
    }
    */
    /// @brief Point record write access
    /// @param new_size The new size of the point records
    void resize_point_records (const size_t new_size)
    {
        p.resize (new_size);
        h.total_points = new_size;
    }
    /// @brief Point record write access
    /// @param extra_fields The new size of the extra fields
    ///
    /// This is an expensive operation because all extra
    /// fields in all point records must be resized
    void resize_extra_fields (const size_t extra_fields)
    {
        for (size_t i = 0; i < p.size (); ++i)
            p[i].extra.resize (extra_fields);
        h.extra_fields = extra_fields;
    }
    /*

    /// @brief Swap this spoc_file with another
    void swap (spoc_file &f)
    {
        std::swap (h, f.h);
        std::swap (p, f.p);
    }
    /// @brief Copy assignment
    spoc_file &operator= (const spoc_file &rhs)
    {
        if (this != &rhs)
        {
            spoc_file tmp (rhs);
            swap (tmp);
        }
        return *this;
    }

    /// @brief Checked point record write access
    /// @param i Point record index
    /// @param pr Point record value
    void set (const size_t i, const point_record::point_record &pr)
    {
        REQUIRE (i < p.size ());
        if (pr.extra.size () != h.extra_fields)
            throw std::runtime_error ("spoc_file::set(): the point record number of extra fields does not match the header");
        p[i] = pr;
    }

    /// @brief Logical operator support
    bool operator== (const spoc_file &other) const
    { return (h == other.h) && (p == other.p); }
    /// @brief Logical operator support
    bool operator!= (const spoc_file &other) const
    { return !(*this == other); }
    */
};

} // namespace file

} // namespace spoc
