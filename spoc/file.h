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
    /// @brief CTOR
    /// @param h Spoc header struct
    /// @param p Point records
    spoc_file (const header::header &h, const point_record::point_records &p)
        : h (h)
        , p (p)
    {
        if (h.total_points != p.size ())
            throw std::runtime_error ("The header total points does not match the data total points");
        if (!p.empty () && h.extra_fields != p[0].extra.size ())
            throw std::runtime_error ("The number of extra fields does not match the point records");
        if (!check_records (p, h.extra_fields))
            throw std::runtime_error ("The point records are inconsistent");
    }
    /// @brief CTOR
    /// @param wkt OGC WKT string
    /// @param extra_fields Number fo extra fields in each point record
    /// @param p Point records
    /// @param compressed Compression flag
    spoc_file (const std::string &wkt,
        const size_t extra_fields,
        const point_record::point_records &p,
        const bool compressed)
        : spoc_file (header::header (wkt, extra_fields, p.size (), compressed), p)
    {
    }
    /// @brief CTOR
    /// @param wkt OGC WKT string
    explicit spoc_file (const std::string &wkt)
        : spoc_file (wkt, 0, point_record::point_records (), false)
    {
    }
    /// @brief CTOR
    /// @param wkt OGC WKT string
    /// @param p Point records
    /// @param compressed Compression flag
    spoc_file (const std::string &wkt,
        const point_record::point_records &p,
        const bool compressed = false)
        : spoc_file (wkt, p.empty () ? 0 : p[0].extra.size (), p, compressed)
    {
    }
    /// @brief CTOR
    /// @param wkt OGC WKT string
    /// @param extra_fields Number fo extra fields in each point record
    /// @param compressed Compression flag
    spoc_file (const std::string &wkt,
        const size_t extra_fields,
        const bool compressed = false)
        : spoc_file (wkt, extra_fields, point_record::point_records (), compressed)
    {
    }
    /// @brief Copy CTOR
    /// @param f Spoc file to copy
    spoc_file (const spoc_file &f)
        : h (f.h)
        , p (f.p)
    {
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

    /// @brief Get number of point records in the file
    /// @return The total number of point records in the file
    size_type size () const
    { return p.size (); }
    /// @brief Reserve space for more point records
    /// @param n Number of point records to reserve
    void reserve (size_type n)
    { p.reserve (n); }

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
    /// @brief Assign all point record values
    /// @param v Point record value to assign
    void assign (const value_type &v)
    { p.assign (p.size (), v); }

    /// @brief Readonly point record access
    const_reference front () const
    { return p.front (); }
    /// @brief Readonly point record access
    const_reference back () const
    { return p.back (); }

    /// @brief Random access
    /// @param i Point record index
    reference operator[] (size_type i)
    {
        assert (i < p.size ());
        return p[i];
    }
    /// @brief Readonly random access
    /// @param i Point record index
    const_reference operator[] (size_type i) const
    {
        assert (i < p.size ());
        return p[i];
    }
    /// @brief Checked random access
    /// @param i Point record index
    ///
    /// Throws if the subscript is invalid.
    reference at (size_type i)
    { return p.at (i); }
    /// @brief Readonly checked random access
    /// @param i Point record index
    ///
    /// Throws if the subscript is invalid.
    const_reference at (size_type i) const
    { return p.at (i); }

    /// @brief Iterator access
    iterator begin ()
    { return p.begin (); }
    /// @brief Readonly iterator access
    const_iterator begin () const
    { return p.begin (); }
    /// @brief Iterator access
    iterator end ()
    { return p.end (); }
    /// @brief Readonly iterator access
    const_iterator end () const
    { return p.end (); }
    /// @brief Readonly iterator access
    const_reverse_iterator rbegin () const
    { return p.rbegin (); }
    /// @brief Readonly iterator access
    const_reverse_iterator rend () const
    { return p.rend (); }

    /// @brief Clear all point records
    void clear ()
    {
        p.clear ();
        h.total_points = 0;
    }
    /// @brief Checked point record write access
    void push_back (const point_record::point_record &pr)
    {
        if (pr.extra.size () != h.extra_fields)
            throw std::runtime_error ("spoc_file::push_back(): the point record number of extra fields does not match the header");
        p.push_back (pr);
        ++h.total_points;
        ENSURE (is_valid ());
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
};

} // namespace file

} // namespace spoc
