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

class spoc_file
{
    private:
    header::header h;
    point_record::point_records p;
    public:
    spoc_file () { }
    spoc_file (const spoc_file &f)
        : h (f.h)
        , p (f.p)
    {
    }
    spoc_file (const std::string &wkt, const bool compressed, const point_record::point_records &p)
        : h (header::header (wkt, 0, p.size (), compressed))
        , p (p)
    {
        if (!p.empty ())
            h.extra_fields = p[0].extra.size ();
        if (!check_records (p))
            throw std::runtime_error ("The point records are inconsistent");
    }
    spoc_file (const std::string &wkt, const point_record::point_records &p)
        : spoc_file (wkt, false, p)
    {
    }
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

    // Contract support
    bool is_valid () const
    {
        if (p.size () != h.total_points)
            return false;
        if (!p.empty () && p[0].extra.size () != h.extra_fields)
            return false;
        return true;
    }

    // Readonly access
    const header::header &get_header () const { return h; }
    const point_record::point_records &get_point_records () const { return p; }
    const point_record::point_record &get_point_record (const size_t n) const { return p[n]; }

    // Read header
    uint8_t get_major_version () const { return h.major_version; }
    uint8_t get_minor_version () const { return h.minor_version; }
    std::string get_wkt () const { return h.wkt; }
    size_t get_extra_fields () const { return h.extra_fields; }
    size_t get_total_points () const { return h.total_points; }
    bool get_compressed () const { return h.compressed; }

    // Change header and point records
    void set_wkt (const std::string &s)
    {
        h.wkt = s;
    }
    void resize (const size_t new_size)
    {
        p.resize (new_size);
        h.total_points = new_size;
    }
    void resize_extra (const size_t extra_fields)
    {
        for (size_t i = 0; i < p.size (); ++i)
            p[i].extra.resize (extra_fields);
        h.extra_fields = extra_fields;
    }
    void set_compressed (const bool flag)
    {
        h.compressed = flag;
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
