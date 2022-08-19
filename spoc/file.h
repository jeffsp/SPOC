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

// Helper function
inline void check_extra_fields (const point_record::point_records &prs)
{
    if (!all_same_size_extra (prs))
        throw std::runtime_error ("The point record extra fields have inconsistent sizes");
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
        : major_version (0)
        , minor_version (0)
        , compressed (false)
    {
    }
    /// @brief CTOR
    /// @param major_version Version information
    /// @param minor_version Version information
    /// @param wkt OGC WKT string
    /// @param compressed Compression flag
    /// @param prs Point records
    explicit spoc_file (const uint8_t major_version,
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
        check_extra_fields (prs);
    }
    /// @brief CTOR
    /// @param wkt OGC WKT string
    /// @param compressed Compression flag
    /// @param prs Point records
    explicit spoc_file (const std::string &wkt,
        const bool compressed = false,
        const point_record::point_records &prs = point_record::point_records ())
        : major_version (0)
        , minor_version (0)
        , wkt (wkt)
        , compressed (compressed)
        , prs (prs)
    {
        // Check for consistency
        check_extra_fields (prs);
    }
    /// @brief Copy CTOR
    /// @param f Spoc file to copy
    spoc_file (const spoc_file &f)
        : wkt (f.wkt)
        , compressed (f.compressed)
        , prs (f.prs)
    {
    }

    /// @brief Contract support
    bool is_valid () const
    {
        if (!all_same_size_extra (prs))
            return false;
        return true;
    }

    /// @brief Readonly wkt access
    std::string get_wkt () const { return wkt; }

    /// @brief Readonly compressed access
    bool get_compressed () const { return compressed; }

    /// @brief Readonly point records access
    const point_record::point_records &get_point_records () const { return prs; }

    /// @brief Build a header from the file information
    const header::header get_header () const
    {
        const size_t extra_fields = prs.empty () ? 0 : prs[0].extra.size ();
        const size_t total_points = prs.size ();
        header::header h (wkt, extra_fields, total_points, compressed);

        ENSURE (h.is_valid ());
        return h;
    }

    /// @brief Set the point records in a SPOC file
    /// @param prs The point records to set
    void set_point_records (const point_record::point_records &prs)
    {
        // Check for consistency
        check_extra_fields (prs);

        // Set the point records
        this->prs = prs;
    }
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
    /// @param new_size The new size of the point records
    void resize_point_records (const size_t new_size)
    {
        prs.resize (new_size);
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
    /*

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
