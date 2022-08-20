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

// Helper function
inline bool all_same_size_extra (const point_record::point_records &prs)
{
    // Degenerate case
    if (prs.empty ())
        return true;

    // Get the size of the first record
    const size_t extra_fields = prs[0].extra.size ();

    // Make sure they all have the same number of extra fields
    if (std::any_of (prs.cbegin(), prs.cend(),
        [&](const point_record::point_record &p)
        { return p.extra.size () != extra_fields; }))
        return false;

    return true;
}

/// @brief SPOC format file
class spoc_file
{
    private:
    uint8_t major_version;
    uint8_t minor_version;
    std::string wkt;
    bool compressed;
    point_record::point_records prs;

    public:
    /// @brief CTOR
    spoc_file ()
        : major_version (MAJOR_VERSION)
        , minor_version (MINOR_VERSION)
        , compressed (false)
    {
    }
    /// @brief CTOR
    /// @param major_version Version information
    /// @param minor_version Version information
    /// @param wkt OGC WKT string
    /// @param compressed Compression flag
    /// @param prs Point records
    spoc_file (const uint8_t major_version,
        const uint8_t minor_version,
        const std::string &wkt,
        const bool compressed = false,
        const point_record::point_records &prs = point_record::point_records ())
        : major_version (major_version)
        , minor_version (minor_version)
        , wkt (wkt)
        , compressed (compressed)
        , prs (prs)
    {
        // Check for consistency
        if (!all_same_size_extra (prs))
            throw std::runtime_error ("The point record extra fields have inconsistent sizes");
    }
    /// @brief CTOR
    /// @param wkt OGC WKT string
    /// @param compressed Compression flag
    /// @param prs Point records
    spoc_file (const std::string &wkt,
        const bool compressed,
        const point_record::point_records &prs = point_record::point_records ())
        : spoc_file (MAJOR_VERSION, MINOR_VERSION, wkt, compressed, prs)
    {
    }
    /// @brief Copy CTOR
    /// @param f Spoc file to copy
    spoc_file (const spoc_file &f)
        : spoc_file (f.major_version, f.minor_version, f.wkt, f.compressed, f.prs)
    {
    }

    /// @brief Get a clone containing no point records
    spoc_file clone_empty () const
    {
        return spoc_file (major_version,
            minor_version,
            wkt,
            compressed,
            point_record::point_records ());
    }

    /// @brief Contract support
    bool is_valid () const
    {
        if (!all_same_size_extra (prs))
            return false;
        return true;
    }

    /// @brief Readonly version access
    unsigned get_major_version () const { return major_version; }

    /// @brief Readonly version access
    unsigned get_minor_version () const { return minor_version; }

    /// @brief Readonly wkt access
    std::string get_wkt () const { return wkt; }

    /// @brief Readonly compressed access
    bool get_compressed () const { return compressed; }

    /// @brief Readonly point records access
    const point_record::point_records &get_point_records () const { return prs; }

    /// @brief Readonly point record access
    const point_record::point_record &get_point_record (const size_t i) const
    {
        assert (i < prs.size ());
        return prs[i];
    }

    /// @brief Build a header from the file information
    const header::header get_header () const
    {
        const size_t extra_fields = prs.empty () ? 0 : prs[0].extra.size ();
        const size_t total_points = prs.size ();
        header::header h (major_version, minor_version, wkt, extra_fields, total_points, compressed);

        return h;
    }

    /// @brief Write compressed access
    void set_wkt (const std::string &s) { wkt = s; }

    /// @brief Write compressed access
    void set_compressed (const bool flag) { compressed = flag; }

    /// @brief Add a point record
    void push_back (const point_record::point_record &pr)
    {
        // Check size of extra fields
        if (!prs.empty () && pr.extra.size () != prs[0].extra.size ())
            throw std::runtime_error ("The number of extra fields is incorrect");

        // Save the point
        prs.push_back (pr);
    }

    /// @brief Point record write access
    /// @param extra_fields The new size of the extra fields
    ///
    /// This is an expensive operation because all extra
    /// fields in all point records must be resized
    void resize_extra_fields (const size_t extra_fields)
    {
        for (auto &p : prs)
            p.extra.resize (extra_fields);
    }

    /// @brief Swap this spoc_file with another
    void swap (spoc_file &f)
    {
        std::swap (major_version, f.major_version);
        std::swap (minor_version, f.minor_version);
        std::swap (wkt, f.wkt);
        std::swap (compressed, f.compressed);
        std::swap (prs, f.prs);
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
    void set_point_record (const size_t i, const point_record::point_record &p)
    {
        REQUIRE (i < prs.size ());
        const size_t extra_fields = prs.empty () ? 0 : prs[0].extra.size ();
        if (p.extra.size () != extra_fields)
            throw std::runtime_error ("spoc_file::set_point_record(): the point record extra fields size is inconsistent with existing point records");
        prs[i] = p;
    }

    /// @brief Set the point records in a SPOC file
    /// @param prs The point records to set
    void set_point_records (const point_record::point_records &prs)
    {
        // Check for consistency
        if (!all_same_size_extra (prs))
            throw std::runtime_error ("The point record extra fields have inconsistent sizes");

        // Set the point records
        this->prs = prs;
    }

    /// @brief Move point records from the spoc_file object
    /// @return rvalue reference
    ///
    /// Upon return, the spoc_file point records will be empty
    point_record::point_records &&move_point_records ()
    {
        return std::move (prs);
    }
    /// @brief Move point records to the spoc_file object
    /// @param prs The point records to move from
    ///
    /// Upon return, the passed point record vector will be empty
    void move_point_records (point_record::point_records &prs)
    {
        // Check for consistency
        if (!all_same_size_extra (prs))
            throw std::runtime_error ("The point record extra fields have inconsistent sizes");

        // Move the point records
        this->prs = std::move (prs);
    }
};

} // namespace file

} // namespace spoc
