#pragma once

#include "compress.h"
#include "point.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

namespace spoc
{

struct point_record
{
    double x;
    double y;
    double z;
    unsigned c; // classification
    unsigned p; // point source ID
    unsigned i; // intensity
    unsigned r; // red
    unsigned g; // green
    unsigned b; // blue
    std::array<unsigned,8> extra;

    point_record ()
        : x (0.0) , y (0.0) , z (0.0)
        , c (0) , p (0) , i (0)
        , r (0) , g (0) , b (0)
        , extra ({0})
    {
    }
};

inline std::ostream &operator<< (std::ostream &s, const point_record &p)
{
    s << '\t' << p.x << '\t' << p.y << '\t' << p.z;
    s << '\t' << p.c << '\t' << p.p << '\t' << p.i;
    s << '\t' << p.r << '\t' << p.g << '\t' << p.b;
    for (auto i : p.extra)
        s << '\t' << i;
    return s;
}

inline bool operator== (const point_record &a, const point_record &b)
{
    if (a.x != b.x) return false;
    if (a.y != b.y) return false;
    if (a.z != b.z) return false;
    if (a.c != b.c) return false;
    if (a.p != b.p) return false;
    if (a.i != b.i) return false;
    if (a.r != b.r) return false;
    if (a.g != b.g) return false;
    if (a.b != b.b) return false;
    if (a.extra != b.extra) return false;
    return true;
}

inline bool operator!= (const point_record &a, const point_record &b)
{
    return !(a == b);
}

class spoc_file
{
    public:
    spoc_file ()
    {
        signature[0] = 'S'; // Simple
        signature[1] = 'P'; // Point
        signature[2] = 'O';
        signature[3] = 'C'; // Cloud
    }
    bool check () const
    {
        // Check signature
        if (signature[0] != 'S') return false;
        if (signature[1] != 'P') return false;
        if (signature[2] != 'O') return false;
        if (signature[3] != 'C') return false;

        // Check data
        if (x.size () != npoints && !x.empty ()) return false;
        if (y.size () != npoints && !y.empty ()) return false;
        if (z.size () != npoints && !z.empty ()) return false;
        if (c.size () != npoints && !c.empty ()) return false;
        if (p.size () != npoints && !p.empty ()) return false;
        if (i.size () != npoints && !i.empty ()) return false;
        if (r.size () != npoints && !r.empty ()) return false;
        if (g.size () != npoints && !g.empty ()) return false;
        if (b.size () != npoints && !b.empty ()) return false;
        for (size_t j = 0; j < extra.size (); ++j)
            if (extra[j].size () != npoints && !extra[j].empty ()) return false;
        return true;
    }
    // Get record number 'n'
    point_record get (const size_t n)
    {
        assert (n < npoints);
        point_record pr;
        pr.x = x.empty () ? 0.0 : x[n];
        pr.y = y.empty () ? 0.0 : y[n];
        pr.z = z.empty () ? 0.0 : z[n];
        pr.c = c.empty () ? 0 : c[n];
        pr.p = p.empty () ? 0 : p[n];
        pr.i = i.empty () ? 0 : i[n];
        pr.r = r.empty () ? 0 : r[n];
        pr.g = g.empty () ? 0 : g[n];
        pr.b = b.empty () ? 0 : b[n];
        for (size_t j = 0; j < extra.size (); ++j)
            pr.extra[j] = pr.extra.empty () ? 0 : extra[j][n];
        return pr;
    }
    // Set record number 'n'
    void set (const size_t n, const point_record &pr)
    {
        assert (n < npoints);
        set (x, n, pr.x);
        set (y, n, pr.y);
        set (z, n, pr.z);
        set (c, n, pr.c);
        set (p, n, pr.p);
        set (i, n, pr.i);
        set (r, n, pr.r);
        set (g, n, pr.g);
        set (b, n, pr.b);
        for (size_t j = 0; j < extra.size (); ++j)
            set (extra[j], n, pr.extra[j]);
    }
    // Resize the number of records in the spoc_file to 'n'
    void resize (const size_t n)
    {
        npoints = n;
        if (!x.empty ()) x.resize (n);
        if (!y.empty ()) y.resize (n);
        if (!z.empty ()) z.resize (n);
        if (!c.empty ()) c.resize (n);
        if (!p.empty ()) p.resize (n);
        if (!i.empty ()) i.resize (n);
        if (!r.empty ()) r.resize (n);
        if (!g.empty ()) g.resize (n);
        if (!b.empty ()) b.resize (n);
        for (size_t j = 0; j < extra.size (); ++j)
            if (!extra[j].empty ()) extra[j].resize (n);
    }

    // I/O
    friend std::ostream &operator<< (std::ostream &s, const spoc_file &f);
    friend void write_spoc_file_compressed (std::ostream &s, const spoc_file &f);
    friend void write_spoc_file (std::ostream &s, const std::vector<point_record> &point_records, const std::string &wkt);
    friend void read_spoc_file_compressed (std::istream &s, spoc_file &f);
    friend void read_spoc_file (std::istream &s, std::vector<point_record> &point_records, std::string &wkt);

    private:
    // Data
    char signature[4];
    size_t npoints;
    std::vector<double> x;
    std::vector<double> y;
    std::vector<double> z;
    std::vector<uint16_t> c;
    std::vector<uint16_t> p;
    std::vector<uint16_t> i;
    std::vector<uint16_t> r;
    std::vector<uint16_t> g;
    std::vector<uint16_t> b;
    std::array<std::vector<uint64_t>,8> extra;
    std::string wkt;

    // Set the field 'x' at location 'n' to value 'y'
    //
    // The container holding field 'x' may be empty, so this function will
    // ensure that the container will get resized appropriately.
    template<typename T,typename U>
    void set (T &x, const size_t n, const U y)
    {
        // Assigning 0 to empty is a no-op
        if (x.empty () && y == 0)
            return;
        // Resize if needed
        if (x.empty ())
            x.resize (npoints);
        // Check logic
        assert (n < npoints);
        assert (n < x.size ());
        // Do the assignment
        x[n] = y;
    }
};

inline std::ostream &operator<< (std::ostream &s, const spoc_file &f)
{
    s << f.signature[0];
    s << f.signature[1];
    s << f.signature[2];
    s << f.signature[3] << std::endl;
    s << "npoints " << f.npoints << std::endl;
    s << "x " << f.x.size () << std::endl;
    s << "y " << f.y.size () << std::endl;
    s << "z " << f.z.size () << std::endl;
    s << "c " << f.c.size () << std::endl;
    s << "p " << f.p.size () << std::endl;
    s << "i " << f.i.size () << std::endl;
    s << "r " << f.r.size () << std::endl;
    s << "g " << f.g.size () << std::endl;
    s << "b " << f.b.size () << std::endl;
    for (size_t j = 0; j < f.extra.size (); ++j)
        s << "extra[" << j << "]" << f.extra[j].size () << std::endl;
    return s;
}

template<typename T>
inline bool all_zero (const std::vector<T> &x)
{
    for (const auto i : x)
        if (i != 0)
            return false;
    return true;
}

template<typename T>
inline void write_compressed (std::ostream &s, const std::vector<T> &x)
{
    // Check to see if they are all zero
    if (all_zero (x))
    {
        // If they are all zero, write the length as '0'
        const uint64_t n = 0;
        s.write (reinterpret_cast<const char*>(&n), sizeof(uint64_t));
        return;
    }

    // Compress
    const auto c = spoc::compress (reinterpret_cast<const uint8_t *> (&x[0]), x.size () * sizeof(T));

    // Write compressed length
    const uint64_t n = c.size ();
    s.write (reinterpret_cast<const char*>(&n), sizeof(uint64_t));

    // Write compressed bytes
    s.write (reinterpret_cast<const char*>(&c[0]), c.size ());
}

inline void write_spoc_file_compressed (std::ostream &s, const spoc_file &f)
{
    // Make sure 'f' is valid
    if (!f.check ())
        throw std::runtime_error ("Invalid spoc file format");

    // Signature
    s.write (&f.signature[0], 4);

    // Number of points
    s.write (reinterpret_cast<const char*>(&f.npoints), sizeof(uint64_t));

    // Data
    write_compressed (s, f.x);
    write_compressed (s, f.y);
    write_compressed (s, f.z);
    write_compressed (s, f.c);
    write_compressed (s, f.p);
    write_compressed (s, f.i);
    write_compressed (s, f.r);
    write_compressed (s, f.g);
    write_compressed (s, f.b);

    // Extra fields
    for (size_t j = 0; j < f.extra.size (); ++j)
        write_compressed (s, f.extra[j]);

    // WKT
    const uint64_t len = f.wkt.size ();
    s.write (reinterpret_cast<const char*>(&len), sizeof(uint64_t));
    s.write (reinterpret_cast<const char*>(&f.wkt[0]), f.wkt.size ());
}

inline void write_spoc_file (std::ostream &s,
    const std::vector<point_record> &point_records,
    const std::string &wkt)
{
    // Stuff the records into a spoc_file struct
    spoc_file f;
    f.npoints = point_records.size ();
    f.x.resize (f.npoints);
    f.y.resize (f.npoints);
    f.z.resize (f.npoints);
    f.c.resize (f.npoints);
    f.p.resize (f.npoints);
    f.i.resize (f.npoints);
    f.r.resize (f.npoints);
    f.g.resize (f.npoints);
    f.b.resize (f.npoints);
    for (size_t j = 0; j < f.extra.size (); ++j)
        f.extra[j].resize (f.npoints);
    for (size_t i = 0; i < f.npoints; ++i)
    {
        f.x[i] = point_records[i].x;
        f.y[i] = point_records[i].y;
        f.z[i] = point_records[i].z;
        f.c[i] = point_records[i].c;
        f.p[i] = point_records[i].p;
        f.i[i] = point_records[i].i;
        f.r[i] = point_records[i].r;
        f.g[i] = point_records[i].g;
        f.b[i] = point_records[i].b;
        for (size_t j = 0; j < f.extra.size (); ++j)
            f.extra[j][i] = point_records[i].extra[j];
    }
    f.wkt = wkt;

    std::clog << f << std::endl;
    write_spoc_file_compressed (s, f);
}

template<typename T>
inline void read_compressed (std::istream &s, std::vector<T> &x)
{
    // Get number of compressed bytes
    uint64_t n = 0;
    s.read (reinterpret_cast<char*>(&n), sizeof(uint64_t));

    // If a '0' was written, the bytes are all zeros
    if (n == 0)
    {
        std::fill (std::begin (x), std::end (x), 0);
        return;
    }

    // Read compressed bytes
    std::vector<uint8_t> c (n);
    s.read (reinterpret_cast<char *> (&c[0]), n);

    // Decompress
    std::vector<T> d (x.size ());
    spoc::decompress (c, reinterpret_cast<uint8_t *> (&d[0]), d.size () * sizeof(T));

    // Commit
    x = d;
}

inline void read_spoc_file_compressed (std::istream &s, spoc_file &f)
{
    // Read into tmp
    spoc_file tmp_f;

    // Read signature
    s.read (&tmp_f.signature[0], 4);

    // Check signature
    if (tmp_f.signature[0] != 'S' ||
        tmp_f.signature[1] != 'P' ||
        tmp_f.signature[2] != 'O' ||
        tmp_f.signature[3] != 'C')
        throw std::runtime_error ("Invalid SPOC file signature");

    // Read number of points
    uint64_t npoints = 0;
    s.read (reinterpret_cast<char*>(&npoints), sizeof(uint64_t));

    // Resize vectors
    tmp_f.x.resize (npoints);
    tmp_f.y.resize (npoints);
    tmp_f.z.resize (npoints);
    tmp_f.c.resize (npoints);
    tmp_f.p.resize (npoints);
    tmp_f.i.resize (npoints);
    tmp_f.r.resize (npoints);
    tmp_f.g.resize (npoints);
    tmp_f.b.resize (npoints);
    for (size_t j = 0; j < tmp_f.extra.size (); ++j)
        tmp_f.extra[j].resize (npoints);

    // Read data
    read_compressed (s, tmp_f.x);
    read_compressed (s, tmp_f.y);
    read_compressed (s, tmp_f.z);
    read_compressed (s, tmp_f.c);
    read_compressed (s, tmp_f.p);
    read_compressed (s, tmp_f.i);
    read_compressed (s, tmp_f.r);
    read_compressed (s, tmp_f.g);
    read_compressed (s, tmp_f.b);

    // Extra fields
    for (size_t j = 0; j < tmp_f.extra.size (); ++j)
        read_compressed (s, tmp_f.extra[j]);

    // WKT
    uint64_t len = 0;
    s.read (reinterpret_cast<char*>(&len), sizeof(uint64_t));
    tmp_f.wkt.resize (len);
    s.read (reinterpret_cast<char*>(&tmp_f.wkt[0]), len);

    // Make sure 'f' is valid
    if (!f.check ())
        throw std::runtime_error ("Invalid spoc file format");

    // Commit
    f = tmp_f;
}

inline void read_spoc_file (std::istream &s,
    std::vector<point_record> &point_records,
    std::string &wkt)
{
    // Read into spoc_file struct
    spoc_file f;
    read_spoc_file_compressed (s, f);

    // Stuff data points into a vector
    std::vector<point_record> tmp_point_records (f.x.size ());

    for (size_t i = 0; i < tmp_point_records.size (); ++i)
    {
        tmp_point_records[i].x = f.x[i];
        tmp_point_records[i].y = f.y[i];
        tmp_point_records[i].z = f.z[i];
        tmp_point_records[i].c = f.c[i];
        tmp_point_records[i].p = f.p[i];
        tmp_point_records[i].i = f.i[i];
        tmp_point_records[i].r = f.r[i];
        tmp_point_records[i].g = f.g[i];
        tmp_point_records[i].b = f.b[i];
        for (size_t j = 0; j < f.extra.size (); ++j)
            tmp_point_records[i].extra[j] = f.extra[j][i];
    }

    // Commit
    point_records = tmp_point_records;
    wkt = f.wkt;
}

} // namespace spoc
