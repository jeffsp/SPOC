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

/// @brief spoc file container adapter
///
/// This is a simple spoc file container that provides 2D
/// subscripting and STL compatibility.
class spoc_file
{
    private:
    header::header h;
    point_record::point_records p;

    // STL support
    //
    // By adding random access iterator support, we get
    // support for STL algorithms and iterators.
    //
    // See "C++ Concepts: RandomAccessIterator" for more details.
    public:
    using size_type                     = size_t;
    using value_type                    = point_record::point_record;
    using container_type                = std::vector<value_type>;
    using allocator_type                = container_type::allocator_type;
    using iterator                      = container_type::iterator;
    using const_iterator                = container_type::const_iterator;
    using reverse_iterator              = container_type::reverse_iterator;
    using const_reverse_iterator        = container_type::const_reverse_iterator;
    using pointer                       = container_type::pointer;
    using const_pointer                 = container_type::const_pointer;
    using reference                     = container_type::reference;
    using const_reference               = container_type::const_reference;

    public:
    /// @brief Default CTOR
    spoc_file () { }
    /// @brief Copy CTOR
    /// @param f Spoc file to copy
    spoc_file (const spoc_file &f)
        : h (f.h)
        , p (f.p)
    {
    }
    /// @brief CTOR
    /// @param wkt OGC WKT string
    /// @param compressed Compression flag
    /// @param p Point records
    spoc_file (const std::string &wkt, const bool compressed, const point_record::point_records &p)
        : h (header::header (wkt, 0, p.size (), compressed))
        , p (p)
    {
        if (!p.empty ())
            h.extra_fields = p[0].extra.size ();
        if (!check_records (p))
            throw std::runtime_error ("The point records are inconsistent");
    }
    /// @brief CTOR
    /// @param wkt OGC WKT string
    /// @param p Point records
    spoc_file (const std::string &wkt, const point_record::point_records &p)
        : spoc_file (wkt, false, p)
    {
    }
    /// @brief CTOR
    /// @param h Spoc header struct
    /// @param p Point records
    spoc_file (const header::header &h, const point_record::point_records &p)
        : h (h)
        , p (p)
    {
        if (h.total_points != p.size ())
            throw std::runtime_error ("The header total points does not match the data total points");
        if (!p.empty () && p[0].extra.size () != h.extra_fields)
            throw std::runtime_error ("The header extra fields size does not match the point records");
        if (!check_records (p))
            throw std::runtime_error ("The point records are inconsistent");
    }
    /// @brief Create a clone of a spoc file with 0 point records
    spoc_file clone_empty () const
    {
        // Copy the header and set its points to 0
        header::header h0 = h;
        h0.total_points = 0;

        // Get empty point records vector
        point_record::point_records pr;

        // Create the empty clone
        return spoc_file (h0, pr);
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

    /// @brief Readonly point records access
    const point_record::point_records &get_point_records () const { return p; }
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
    /// @brief Point record write access
    /// @param new_size The new size of the point records
    void resize (const size_t new_size)
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

    // Point record R/W access
    void add (const point_record::point_record &pr)
    {
        REQUIRE (pr.extra.size () == h.extra_fields);
        p.push_back (pr);
        ++h.total_points;
        ENSURE (is_valid ());
    }
    void set (const size_t n, const point_record::point_record &r)
    {
        assert (n < p.size ());
        p[n] = r;
    }

    // Point record R/W access
    point_record::point_record &operator[] (const size_t n)
    {
        assert (n < p.size ());
        return p[n];
    }
    const point_record::point_record &operator[] (const size_t n) const
    {
        assert (n < p.size ());
        return p[n];
    }
    std::vector<point_record::point_record>::iterator begin ()
    {
        return p.begin ();
    }
    std::vector<point_record::point_record>::const_iterator begin () const
    {
        return p.begin ();
    }
    std::vector<point_record::point_record>::iterator end ()
    {
        return p.end ();
    }
    std::vector<point_record::point_record>::const_iterator end () const
    {
        return p.end ();
    }

    // Operators
    bool operator== (const spoc_file &other) const
    {
        return
            (h == other.h)
            && (p == other.p);
    }
    bool operator!= (const spoc_file &other) const
    {
        return !(*this == other);
    }
};

} // namespace file

} // namespace spoc
