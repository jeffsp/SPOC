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

template<typename T>
inline bool all_zero (const std::vector<T> &x)
{
    if (std::any_of (x.begin (), x.end (),
        [](const T &i) { return i != 0; }))
        return false;
    return true;
}

// Resize a vector, following DPFWYDU rules
template<typename T>
inline void resize (std::vector<T> &x, const size_t n)
{
    // Only resize if it's being used
    if (!x.empty ())
        x.resize (n);
    // Resize can change the contents, so recheck for all zero
    if (all_zero (x))
        x.clear ();
    // Free memory
    x.shrink_to_fit ();
}

class spoc_file
{
    public:
    spoc_file ()
        : npoints (0)
    {
        signature[0] = 'S'; // Simple
        signature[1] = 'P'; // Point
        signature[2] = 'O';
        signature[3] = 'C'; // Cloud
        signature[4] = '\0'; // Terminate
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
            pr.extra[j] = extra[j].empty () ? 0 : extra[j][n];
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
        spoc::resize (x, n);
        spoc::resize (y, n);
        spoc::resize (z, n);
        spoc::resize (c, n);
        spoc::resize (p, n);
        spoc::resize (i, n);
        spoc::resize (r, n);
        spoc::resize (g, n);
        spoc::resize (b, n);
        for (size_t j = 0; j < extra.size (); ++j)
            spoc::resize (extra[j], n);
    }
    // Reallocate memory on all containers
    void reallocate ()
    {
        resize (npoints);
    }

    // Readonly accessors
    const char *get_signature () const { return signature; }
    const size_t get_npoints () const { return npoints; }
    const std::vector<double> get_x () const { return x; }
    const std::vector<double> get_y () const { return y; }
    const std::vector<double> get_z () const { return z; }
    const std::vector<uint16_t> get_c () const { return c; }
    const std::vector<uint16_t> get_p () const { return p; }
    const std::vector<uint16_t> get_i () const { return i; }
    const std::vector<uint16_t> get_r () const { return r; }
    const std::vector<uint16_t> get_g () const { return g; }
    const std::vector<uint16_t> get_b () const { return b; }
    const std::array<std::vector<uint64_t>,8> get_extra () const { return extra; }
    const std::string get_wkt () { return wkt; }

    // I/O
    friend std::ostream &operator<< (std::ostream &s, const spoc_file &f);
    friend void write_spoc_file (std::ostream &s, const spoc_file &f);
    friend void write_spoc_file (std::ostream &s, const std::vector<point_record> &point_records, const std::string &wkt);
    friend spoc_file read_spoc_file (std::istream &s);
    friend std::vector<point_record> read_spoc_file (std::istream &s, std::string &wkt);

    private:
    // Data
    char signature[5];
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
    s << "x"; for (auto i : f.x) s << " " << i; s << std::endl;
    s << "y"; for (auto i : f.y) s << " " << i; s << std::endl;
    s << "z"; for (auto i : f.z) s << " " << i; s << std::endl;
    s << "c"; for (auto i : f.c) s << " " << i; s << std::endl;
    s << "p"; for (auto i : f.p) s << " " << i; s << std::endl;
    s << "i"; for (auto i : f.i) s << " " << i; s << std::endl;
    s << "r"; for (auto i : f.r) s << " " << i; s << std::endl;
    s << "g"; for (auto i : f.g) s << " " << i; s << std::endl;
    s << "b"; for (auto i : f.b) s << " " << i; s << std::endl;
    for (size_t j = 0; j < f.extra.size (); ++j)
    {
        s << "extra[" << j << "]";
        for (auto i : f.extra[j]) s << " " << i; s << std::endl;
    }
    return s;
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

inline void write_spoc_file (std::ostream &s, const spoc_file &f)
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

    // Release unused fields
    f.reallocate ();

    // Write it
    write_spoc_file (s, f);
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

inline spoc_file read_spoc_file (std::istream &s)
{
    // Read into f
    spoc_file f;

    // Read signature
    s.read (&f.signature[0], 4);

    // Check signature
    if (f.signature[0] != 'S' ||
        f.signature[1] != 'P' ||
        f.signature[2] != 'O' ||
        f.signature[3] != 'C')
        throw std::runtime_error ("Invalid SPOC file signature");

    // Read number of points
    uint64_t npoints = 0;
    s.read (reinterpret_cast<char*>(&npoints), sizeof(uint64_t));

    // Resize vectors
    f.x.resize (npoints);
    f.y.resize (npoints);
    f.z.resize (npoints);
    f.c.resize (npoints);
    f.p.resize (npoints);
    f.i.resize (npoints);
    f.r.resize (npoints);
    f.g.resize (npoints);
    f.b.resize (npoints);
    for (size_t j = 0; j < f.extra.size (); ++j)
        f.extra[j].resize (npoints);

    // Read data
    read_compressed (s, f.x);
    read_compressed (s, f.y);
    read_compressed (s, f.z);
    read_compressed (s, f.c);
    read_compressed (s, f.p);
    read_compressed (s, f.i);
    read_compressed (s, f.r);
    read_compressed (s, f.g);
    read_compressed (s, f.b);

    // Extra fields
    for (size_t j = 0; j < f.extra.size (); ++j)
        read_compressed (s, f.extra[j]);

    // WKT
    uint64_t len = 0;
    s.read (reinterpret_cast<char*>(&len), sizeof(uint64_t));
    f.wkt.resize (len);
    s.read (reinterpret_cast<char*>(&f.wkt[0]), len);

    // Make sure 'f' is valid
    if (!f.check ())
        throw std::runtime_error ("Invalid spoc file format");

    return f;
}

inline std::vector<point_record> read_spoc_file (std::istream &s, std::string &wkt)
{
    // Read into spoc_file struct
    spoc_file f = read_spoc_file (s);

    // Stuff data points into a vector
    std::vector<point_record> point_records (f.x.size ());

    for (size_t i = 0; i < point_records.size (); ++i)
    {
        point_records[i].x = f.x[i];
        point_records[i].y = f.y[i];
        point_records[i].z = f.z[i];
        point_records[i].c = f.c[i];
        point_records[i].p = f.p[i];
        point_records[i].i = f.i[i];
        point_records[i].r = f.r[i];
        point_records[i].g = f.g[i];
        point_records[i].b = f.b[i];
        for (size_t j = 0; j < f.extra.size (); ++j)
            point_records[i].extra[j] = f.extra[j][i];
    }

    // Commit
    wkt = f.wkt;

    return point_records;
}

} // namespace spoc
