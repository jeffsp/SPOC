#pragma once

#include "compress.h"
#include "point.h"
#include "version.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iomanip>
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

inline void write_binary_header (std::ostream &s,
    const uint8_t major_version,
    const uint8_t minor_version,
    const uint64_t npoints)
{
    const char *signature = "SPOC";
    s.write (reinterpret_cast<const char*>(signature), 4 * sizeof(char));
    s.write (reinterpret_cast<const char*>(&major_version), sizeof(uint8_t));
    s.write (reinterpret_cast<const char*>(&minor_version), sizeof(uint8_t));
    s.write (reinterpret_cast<const char*>(&npoints), sizeof(uint64_t));
    s.flush ();
}

inline void read_binary_header (std::istream &s,
    char signature[4],
    uint8_t &major_version,
    uint8_t &minor_version,
    size_t &npoints)
{
    s.read (reinterpret_cast<char*>(&signature), 4 * sizeof(char));
    s.read (reinterpret_cast<char*>(&major_version), sizeof(uint8_t));
    s.read (reinterpret_cast<char*>(&minor_version), sizeof(uint8_t));
    s.read (reinterpret_cast<char*>(&npoints), sizeof(uint64_t));
}

inline void write_binary (std::ostream &s, const point_record &p)
{
    s.write (reinterpret_cast<const char*>(&p.x), sizeof(double));
    s.write (reinterpret_cast<const char*>(&p.y), sizeof(double));
    s.write (reinterpret_cast<const char*>(&p.z), sizeof(double));
    s.write (reinterpret_cast<const char*>(&p.c), sizeof(uint16_t));
    s.write (reinterpret_cast<const char*>(&p.p), sizeof(uint16_t));
    s.write (reinterpret_cast<const char*>(&p.i), sizeof(uint16_t));
    s.write (reinterpret_cast<const char*>(&p.r), sizeof(uint16_t));
    s.write (reinterpret_cast<const char*>(&p.g), sizeof(uint16_t));
    s.write (reinterpret_cast<const char*>(&p.b), sizeof(uint16_t));
    for (size_t j = 0; j < p.extra.size (); ++j)
        s.write (reinterpret_cast<const char*>(&p.extra[j]), sizeof(uint64_t));
    s.flush ();
}

inline point_record read_binary (std::istream &s)
{
    point_record p;
    s.read (reinterpret_cast<char*>(&p.x), sizeof(double));
    s.read (reinterpret_cast<char*>(&p.y), sizeof(double));
    s.read (reinterpret_cast<char*>(&p.z), sizeof(double));
    s.read (reinterpret_cast<char*>(&p.c), sizeof(uint16_t));
    s.read (reinterpret_cast<char*>(&p.p), sizeof(uint16_t));
    s.read (reinterpret_cast<char*>(&p.i), sizeof(uint16_t));
    s.read (reinterpret_cast<char*>(&p.r), sizeof(uint16_t));
    s.read (reinterpret_cast<char*>(&p.g), sizeof(uint16_t));
    s.read (reinterpret_cast<char*>(&p.b), sizeof(uint16_t));
    for (size_t j = 0; j < p.extra.size (); ++j)
        s.read (reinterpret_cast<char*>(&p.extra[j]), sizeof(uint64_t));
    return p;
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
    point_record get (const size_t n) const
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
    // Set the version for testing
    void set_version (const uint8_t major, const uint8_t minor)
    {
        major_version = major;
        minor_version = minor;
    }
    // Set the well-known-text SRS string
    void set_wkt (const std::string &s)
    {
        wkt = s;
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
    const uint8_t get_major_version () const { return major_version; }
    const uint8_t get_minor_version () const { return minor_version; }
    const std::string &get_wkt () const { return wkt; }
    const size_t get_npoints () const { return npoints; }
    const std::vector<double> &get_x () const { return x; }
    const std::vector<double> &get_y () const { return y; }
    const std::vector<double> &get_z () const { return z; }
    const std::vector<uint16_t> &get_c () const { return c; }
    const std::vector<uint16_t> &get_p () const { return p; }
    const std::vector<uint16_t> &get_i () const { return i; }
    const std::vector<uint16_t> &get_r () const { return r; }
    const std::vector<uint16_t> &get_g () const { return g; }
    const std::vector<uint16_t> &get_b () const { return b; }
    const std::array<std::vector<uint64_t>,8> &get_extra () const { return extra; }

    // Write accessors
    void set_x (const size_t n, const double v) { set (x, n, v); }
    void set_y (const size_t n, const double v) { set (y, n, v); }
    void set_z (const size_t n, const double v) { set (z, n, v); }
    void set_c (const size_t n, const uint16_t v) { set (c, n, v); }
    void set_p (const size_t n, const uint16_t v) { set (p, n, v); }
    void set_i (const size_t n, const uint16_t v) { set (i, n, v); }
    void set_r (const size_t n, const uint16_t v) { set (r, n, v); }
    void set_g (const size_t n, const uint16_t v) { set (g, n, v); }
    void set_b (const size_t n, const uint16_t v) { set (b, n, v); }
    void set_extra (const size_t n, const size_t j, const uint64_t v) { set (extra[j], n, v); }

    // I/O
    friend std::ostream &operator<< (std::ostream &s, const spoc_file &f);
    friend void write_spoc_file (std::ostream &s, const spoc_file &f);
    friend spoc_file read_spoc_file (std::istream &s);

    private:
    // Data
    char signature[5];
    uint8_t major_version = MAJOR_VERSION;
    uint8_t minor_version = MINOR_VERSION;
    std::string wkt;
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
        for (auto i : f.extra[j])
            s << " " << i;
        s << std::endl;
    }
    return s;
}

template<typename T>
inline std::vector<uint8_t> compress_field (const std::vector<T> &x)
{
    // Check to see if they are all zero
    if (all_zero (x))
        return std::vector<uint8_t> (); // Return empty vector

    // Compress
    //  -1 = default compression
    //   0 = no compression
    //   1 = fastest compression
    //   9 = smallest compression
    const int compression_level = -1;
    const auto c = spoc::compress (reinterpret_cast<const uint8_t *> (&x[0]), x.size () * sizeof(T), compression_level);

    return c;
}

template<typename T>
inline void write_compressed (std::ostream &s, const std::vector<T> &x)
{
    // Check for empty vector
    if (x.empty ())
    {
        // Write the length as '0'
        const uint64_t n = 0;
        s.write (reinterpret_cast<const char*>(&n), sizeof(uint64_t));
        return;
    }

    // Write compressed length
    const uint64_t n = x.size ();
    s.write (reinterpret_cast<const char*>(&n), sizeof(uint64_t));

    // Write compressed bytes
    s.write (reinterpret_cast<const char*>(&x[0]), x.size ());
}

inline void write_spoc_file (std::ostream &s, const spoc_file &f)
{
    // Make sure 'f' is valid
    if (!f.check ())
        throw std::runtime_error ("Invalid spoc file format");

    // Signature
    s.write (&f.signature[0], 4);

    // Version
    s.write (reinterpret_cast<const char*>(&f.major_version), sizeof(uint8_t));
    s.write (reinterpret_cast<const char*>(&f.minor_version), sizeof(uint8_t));

    // WKT
    const uint64_t len = f.wkt.size ();
    s.write (reinterpret_cast<const char*>(&len), sizeof(uint64_t));
    s.write (reinterpret_cast<const char*>(&f.wkt[0]), f.wkt.size ());

    // Number of points
    s.write (reinterpret_cast<const char*>(&f.npoints), sizeof(uint64_t));

    // Compress fields in parallel
    std::vector<std::vector<uint8_t>> c (9);
    #pragma omp parallel sections
    {
        #pragma omp section
        { c[0] = compress_field (f.x); }
        #pragma omp section
        { c[1] = compress_field (f.y); }
        #pragma omp section
        { c[2] = compress_field (f.z); }
        #pragma omp section
        { c[3] = compress_field (f.c); }
        #pragma omp section
        { c[4] = compress_field (f.p); }
        #pragma omp section
        { c[5] = compress_field (f.i); }
        #pragma omp section
        { c[6] = compress_field (f.r); }
        #pragma omp section
        { c[7] = compress_field (f.g); }
        #pragma omp section
        { c[8] = compress_field (f.b); }
    }

    // Compress extra fields in parallel
    std::vector<std::vector<uint8_t>> e (f.extra.size ());
    #pragma omp parallel for
    for (size_t i = 0; i < f.extra.size (); ++i)
        e[i] = compress_field (f.extra[i]);

    // Write it out
    for (size_t i = 0; i < c.size (); ++i)
        write_compressed (s, c[i]);

    for (size_t i = 0; i < e.size (); ++i)
        write_compressed (s, e[i]);
}

inline void write_spoc_file (std::ostream &s,
    const std::string &wkt,
    const std::vector<point_record> &point_records)
{
    spoc_file f;

    // Set WKT
    f.set_wkt (wkt);

    // Stuff the records into a spoc_file struct
    f.resize (point_records.size ());
    for (size_t i = 0; i < f.get_npoints (); ++i)
        f.set (i, point_records[i]);

    // Release unused fields
    f.reallocate ();

    // Write it
    write_spoc_file (s, f);
}

template<typename T>
inline std::vector<T> read_compressed (std::istream &s, const size_t size)
{
    // Get number of compressed bytes
    uint64_t n = 0;
    s.read (reinterpret_cast<char*>(&n), sizeof(uint64_t));

    // If a '0' was written, the vector is empty
    if (n == 0)
        return std::vector<T> ();

    // Read compressed bytes
    std::vector<uint8_t> c (n);
    s.read (reinterpret_cast<char *> (&c[0]), n);

    // Decompress
    std::vector<T> x (size);
    spoc::decompress (c, reinterpret_cast<uint8_t *> (&x[0]), x.size () * sizeof(T));

    return x;
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

    // Read version
    s.read (reinterpret_cast<char*>(&f.major_version), sizeof(uint8_t));
    s.read (reinterpret_cast<char*>(&f.minor_version), sizeof(uint8_t));

    // WKT
    uint64_t len = 0;
    s.read (reinterpret_cast<char*>(&len), sizeof(uint64_t));
    f.wkt.resize (len);
    s.read (reinterpret_cast<char*>(&f.wkt[0]), len);

    // Read number of points
    s.read (reinterpret_cast<char*>(&f.npoints), sizeof(uint64_t));

    // Read data
    f.x = read_compressed<double> (s, f.npoints);
    f.y = read_compressed<double> (s, f.npoints);
    f.z = read_compressed<double> (s, f.npoints);
    f.c = read_compressed<uint16_t> (s, f.npoints);
    f.p = read_compressed<uint16_t> (s, f.npoints);
    f.i = read_compressed<uint16_t> (s, f.npoints);
    f.r = read_compressed<uint16_t> (s, f.npoints);
    f.g = read_compressed<uint16_t> (s, f.npoints);
    f.b = read_compressed<uint16_t> (s, f.npoints);

    // Extra fields
    for (size_t j = 0; j < f.extra.size (); ++j)
        f.extra[j] = read_compressed<uint64_t> (s, f.npoints);

    // GCOV_EXCL_START
    // Make sure 'f' is valid
    if (!f.check ())
        throw std::runtime_error ("Invalid spoc file format");
    // GCOV_EXCL_STOP

    return f;
}

inline std::vector<point_record> read_spoc_file (std::istream &s, std::string &wkt)
{
    // Read into spoc_file struct
    spoc_file f = read_spoc_file (s);

    // Stuff data points into a vector
    std::vector<point_record> point_records (f.get_npoints ());

    for (size_t i = 0; i < point_records.size (); ++i)
        point_records[i] = f.get (i);

    // Commit
    wkt = f.get_wkt ();

    return point_records;
}

} // namespace spoc
