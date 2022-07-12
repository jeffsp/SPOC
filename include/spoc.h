#pragma once
#include "compress.h"
#include "point.h"
#include "version.h"
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

/// Top level namespace for all SPOC definitions
namespace spoc
{

///
/// POD struct for SPOC file header
///
struct header
{
    /// Constructor
    /// @param wkt Well known text string
    /// @param extra_fields Number of extra fields in each point record
    /// @param total_points Total points in the SPOC file
    /// @param compressed Compression flag
    header (const std::string &wkt,
            const size_t extra_fields,
            const size_t total_points,
            const bool compressed)
        : wkt (wkt)
        , extra_fields (extra_fields)
        , total_points (total_points)
        , compressed (compressed)
    {
        signature[0] = 'S'; // Simple
        signature[1] = 'P'; // Point
        signature[2] = 'O';
        signature[3] = 'C'; // Cloud
        signature[4] = '\0'; // Terminate
    }
    header () : header (std::string (), 0, 0, false)
    {
    }
    bool check_signature () const
    {
        if (signature[0] != 'S') return false;
        if (signature[1] != 'P') return false;
        if (signature[2] != 'O') return false;
        if (signature[3] != 'C') return false;
        return true;
    }
    bool is_valid () const
    {
        if (!check_signature ())
            return false;
        return true;
    }

    char signature[5];
    uint8_t major_version = MAJOR_VERSION;
    uint8_t minor_version = MINOR_VERSION;
    std::string wkt;
    size_t extra_fields;
    size_t total_points;
    uint8_t compressed;
};

// Helper I/O function
inline std::ostream &operator<< (std::ostream &s, const header &h)
{
    for (auto i : {0, 1, 2 ,3})
        s << h.signature[i];
    s << std::endl;
    s << static_cast<int> (h.major_version)
        << "."
        << static_cast<int> (h.minor_version)
        << std::endl;
    s << h.wkt << std::endl;
    s << "extra_fields " << h.extra_fields << std::endl;
    s << "total_points " << h.total_points << std::endl;
    s << "compressed " << (h.compressed ? "true" : "false")  << std::endl;
    return s;
}

// Helper operator
inline bool operator== (const header &a, const header &b)
{
    for (auto i : {0, 1, 2, 3})
        // cppcheck-suppress useStlAlgorithm
        if (a.signature[i] != b.signature[i]) return false;
    if (a.major_version != b.major_version) return false;
    if (a.minor_version != b.minor_version) return false;
    if (a.wkt != b.wkt) return false;
    if (a.extra_fields != b.extra_fields) return false;
    if (a.total_points != b.total_points) return false;
    if (a.compressed != b.compressed) return false;
    return true;
}

// Header I/O
inline void write_header (std::ostream &s, const header &h)
{
    if (!h.check_signature ())
        throw std::runtime_error ("Invalid spoc file format");
    s.write (reinterpret_cast<const char*>(h.signature), 4 * sizeof(char));
    s.write (reinterpret_cast<const char*>(&h.major_version), sizeof(uint8_t));
    s.write (reinterpret_cast<const char*>(&h.minor_version), sizeof(uint8_t));
    const uint64_t len = h.wkt.size ();
    s.write (reinterpret_cast<const char*>(&len), sizeof(uint64_t));
    s.write (reinterpret_cast<const char*>(&h.wkt[0]), h.wkt.size ());
    s.write (reinterpret_cast<const char*>(&h.extra_fields), sizeof(uint64_t));
    s.write (reinterpret_cast<const char*>(&h.total_points), sizeof(uint64_t));
    s.write (reinterpret_cast<const char*>(&h.compressed), sizeof(uint8_t));
    s.flush ();
}

// Header I/O
inline header read_header (std::istream &s)
{
    header h;
    s.read (reinterpret_cast<char*>(&h.signature), 4 * sizeof(char));
    if (!h.check_signature ())
        throw std::runtime_error ("Invalid spoc file format");
    s.read (reinterpret_cast<char*>(&h.major_version), sizeof(uint8_t));
    s.read (reinterpret_cast<char*>(&h.minor_version), sizeof(uint8_t));
    uint64_t len = 0;
    s.read (reinterpret_cast<char*>(&len), sizeof(uint64_t));
    h.wkt.resize (len);
    s.read (reinterpret_cast<char*>(&h.wkt[0]), len);
    s.read (reinterpret_cast<char*>(&h.extra_fields), sizeof(uint64_t));
    s.read (reinterpret_cast<char*>(&h.total_points), sizeof(uint64_t));
    s.read (reinterpret_cast<char*>(&h.compressed), sizeof(uint8_t));
    return h;
}

// Point record format
struct point_record
{
    double x;
    double y;
    double z;
    uint32_t c; // classification
    uint32_t p; // point ID
    uint16_t i; // intensity/NIR
    uint16_t r; // red
    uint16_t g; // green
    uint16_t b; // blue
    std::vector<uint64_t> extra;

    point_record ()
        : x (0.0) , y (0.0) , z (0.0)
        , c (0) , p (0) , i (0)
        , r (0) , g (0) , b (0)
        , extra (0)
    {
    }
    explicit point_record (const size_t extra_fields)
        : x (0.0) , y (0.0) , z (0.0)
        , c (0) , p (0) , i (0)
        , r (0) , g (0) , b (0)
        , extra (extra_fields)
    {
    }
};

// Helper I/O function
inline std::ostream &operator<< (std::ostream &s, const point_record &p)
{
    s << '\t' << p.x << '\t' << p.y << '\t' << p.z;
    s << '\t' << p.c << '\t' << p.p << '\t' << p.i;
    s << '\t' << p.r << '\t' << p.g << '\t' << p.b;
    for (auto i : p.extra)
        s << '\t' << i;
    return s;
}

// Record I/O
inline void write_point_record (std::ostream &s, const point_record &p)
{
    s.write (reinterpret_cast<const char*>(&p.x), sizeof(double)
        + sizeof(double)
        + sizeof(double)
        + sizeof(uint32_t)
        + sizeof(uint32_t)
        + sizeof(uint16_t)
        + sizeof(uint16_t)
        + sizeof(uint16_t)
        + sizeof(uint16_t));
    s.write (reinterpret_cast<const char*>(&p.extra[0]), p.extra.size () * sizeof(uint64_t));
    s.flush ();
}

// Record I/O
inline point_record read_point_record (std::istream &s, const size_t extra_fields)
{
    point_record p;
    s.read (reinterpret_cast<char*>(&p.x), sizeof(double)
        + sizeof(double)
        + sizeof(double)
        + sizeof(uint32_t)
        + sizeof(uint32_t)
        + sizeof(uint16_t)
        + sizeof(uint16_t)
        + sizeof(uint16_t)
        + sizeof(uint16_t));
    p.extra.resize (extra_fields);
    s.read (reinterpret_cast<char*>(&p.extra[0]), p.extra.size () * sizeof(uint64_t));
    return p;
}

// Helper operator
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

// Helper operator
inline bool operator!= (const point_record &a, const point_record &b)
{
    return !(a == b);
}

// Helper typedefs
using point_records = std::vector<point_record>;

// Helper function
bool check_records (const point_records &prs)
{
    // Degenerate case
    if (prs.empty ())
        return true;

    // Make sure they all have the same number of extra fields
    const size_t extra_fields = prs[0].extra.size ();

    if (std::any_of (prs.cbegin(), prs.cend(),
        [&](const point_record &p)
        { return p.extra.size () != extra_fields; }))
        return false;

    return true;
}

template<typename T>
inline bool all_zero (const std::vector<T> &x)
{
    if (std::any_of (x.begin (), x.end (),
        [](const T &i) { return i != 0; }))
        return false;
    return true;
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
    // Check for all-zero vector
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

// Helper functions
std::vector<double> get_x (const point_records &p)
{
    std::vector<double> x (p.size ());
    for (size_t n = 0; n < p.size (); ++n)
        x[n] = p[n].x;
    return x;
}

std::vector<double> get_y (const point_records &p)
{
    std::vector<double> x (p.size ());
    for (size_t n = 0; n < p.size (); ++n)
        x[n] = p[n].y;
    return x;
}

std::vector<double> get_z (const point_records &p)
{
    std::vector<double> x (p.size ());
    for (size_t n = 0; n < p.size (); ++n)
        x[n] = p[n].z;
    return x;
}

std::vector<uint32_t> get_c (const point_records &p)
{
    std::vector<uint32_t> x (p.size ());
    for (size_t n = 0; n < p.size (); ++n)
        x[n] = p[n].c;
    return x;
}

std::vector<uint32_t> get_p (const point_records &p)
{
    std::vector<uint32_t> x (p.size ());
    for (size_t n = 0; n < p.size (); ++n)
        x[n] = p[n].p;
    return x;
}

std::vector<uint16_t> get_i (const point_records &p)
{
    std::vector<uint16_t> x (p.size ());
    for (size_t n = 0; n < p.size (); ++n)
        x[n] = p[n].i;
    return x;
}

std::vector<uint16_t> get_r (const point_records &p)
{
    std::vector<uint16_t> x (p.size ());
    for (size_t n = 0; n < p.size (); ++n)
        x[n] = p[n].r;
    return x;
}

std::vector<uint16_t> get_g (const point_records &p)
{
    std::vector<uint16_t> x (p.size ());
    for (size_t n = 0; n < p.size (); ++n)
        x[n] = p[n].g;
    return x;
}

std::vector<uint16_t> get_b (const point_records &p)
{
    std::vector<uint16_t> x (p.size ());
    for (size_t n = 0; n < p.size (); ++n)
        x[n] = p[n].b;
    return x;
}

size_t get_extra_fields_size (const point_records &p)
{
    if (p.empty ())
        return 0;
    return p[0].extra.size ();
}

std::vector<uint64_t> get_extra (const size_t k, const point_records &p)
{
    std::vector<uint64_t> x (p.size ());
    for (size_t n = 0; n < p.size (); ++n)
    {
        assert (k < p[n].extra.size ());
        x[n] = p[n].extra[k];
    }
    return x;
}

class spoc_file
{
    private:
    header h;
    point_records p;
    public:
    spoc_file () { }
    spoc_file (const std::string &wkt, const bool compressed, const point_records &p)
        : h (header (wkt, 0, p.size (), compressed))
        , p (p)
    {
        if (!p.empty ())
            h.extra_fields = p[0].extra.size ();
        if (!check_records (p))
            throw std::runtime_error ("The point records are inconsistent");
    }
    spoc_file (const std::string &wkt, const point_records &p)
        : spoc_file (wkt, false, p)
    {
    }
    spoc_file (const header &h, const point_records &p)
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
    // Readonly access
    const header &get_header () const { return h; }
    const point_records &get_point_records () const { return p; }
    const point_record &get_point_record (const size_t n) const { return p[n]; }

    // R/W access
    void set_wkt (const std::string &s)
    {
        h.wkt = s;
    }
    void set_compressed (const bool flag)
    {
        h.compressed = flag;
    }
    void set_point_record (const size_t n, const point_record &r)
    {
        assert (n < p.size ());
        p[n] = r;
    }
    void resize (const size_t new_size)
    {
        p.resize (new_size);
        h.total_points = new_size;
    }
    void resize_extra (const size_t new_size)
    {
        for (size_t i = 0; i < p.size (); ++i)
            p[i].extra.resize (new_size);
        h.extra_fields = new_size;
    }

    // R/W access
    point_record &operator[] (const size_t n)
    {
        assert (n < p.size ());
        return p[n];
    }
};

// Spoc File I/O
inline point_records read_uncompressed_points (std::istream &s,
    const size_t total_points,
    const size_t extra_fields)
{
    // Read the data
    point_records p;
    p.resize (total_points);
    for (size_t i = 0; i < p.size (); ++i)
        p[i] = read_point_record (s, extra_fields);

    return p;
}

inline spoc_file read_spoc_file_uncompressed (std::istream &s)
{
    // Read the header
    header h = read_header (s);

    // Check compression flag
    if (h.compressed)
        throw std::runtime_error ("Uncompressed reader can't read a compressed file");

    // Read the data
    point_records p = read_uncompressed_points (s, h.total_points, h.extra_fields);

    return spoc_file (h, p);
}

inline point_records read_compressed_points (std::istream &s,
    const size_t total_points,
    const size_t extra_fields)
{
    // Read the data
    point_records prs;
    prs.resize (total_points);

    // Read data
    std::vector<double> x = read_compressed<double> (s, total_points);
    std::vector<double> y = read_compressed<double> (s, total_points);
    std::vector<double> z = read_compressed<double> (s, total_points);
    std::vector<uint32_t> c = read_compressed<uint32_t> (s, total_points);
    std::vector<uint32_t> p = read_compressed<uint32_t> (s, total_points);
    std::vector<uint16_t> i = read_compressed<uint16_t> (s, total_points);
    std::vector<uint16_t> r = read_compressed<uint16_t> (s, total_points);
    std::vector<uint16_t> g = read_compressed<uint16_t> (s, total_points);
    std::vector<uint16_t> b = read_compressed<uint16_t> (s, total_points);

    std::vector<std::vector<uint64_t>> extra (extra_fields);
    for (size_t j = 0; j < extra.size (); ++j)
        extra[j] = read_compressed<uint64_t> (s, total_points);

    // Copy them into the point records
    prs.resize (total_points);

    #pragma omp parallel for
    for (size_t n = 0; n < prs.size (); ++n)
    {
        if (!x.empty ()) prs[n].x = x[n];
        if (!y.empty ()) prs[n].y = y[n];
        if (!z.empty ()) prs[n].z = z[n];
        if (!c.empty ()) prs[n].c = c[n];
        if (!p.empty ()) prs[n].p = p[n];
        if (!i.empty ()) prs[n].i = i[n];
        if (!r.empty ()) prs[n].r = r[n];
        if (!g.empty ()) prs[n].g = g[n];
        if (!b.empty ()) prs[n].b = b[n];
        prs[n].extra.resize (extra_fields);
        for (size_t j = 0; j < prs[n].extra.size (); ++j)
            if (!extra[j].empty ()) prs[n].extra[j] = extra[j][n];
    }

    return prs;
}

inline spoc_file read_spoc_file_compressed (std::istream &s)
{
    // Read the header
    header h = read_header (s);

    // Check compression flag
    if (!h.compressed)
        throw std::runtime_error ("Compressed reader can't read an uncompressed file");

    // Read the data
    point_records prs = read_compressed_points (s, h.total_points, h.extra_fields);

    return spoc_file (h, prs);
}

inline spoc_file read_spoc_file (std::istream &s)
{
    // Read the header
    header h = read_header (s);

    // The points
    point_records p;

    // Check compression flag
    if (h.compressed)
        p = read_compressed_points (s, h.total_points, h.extra_fields);
    else
        p = read_uncompressed_points (s, h.total_points, h.extra_fields);

    return spoc_file (h, p);
}

inline void write_spoc_file_uncompressed (std::ostream &s, const spoc_file &f)
{
    // Check compression flag
    if (f.get_header ().compressed)
        throw std::runtime_error ("Uncompressed writer can't write a compressed file");

    // Make sure the points records are OK
    if (!check_records (f.get_point_records ()))
        throw std::runtime_error ("Invalid point records");

    // Write the file
    write_header (s, f.get_header ());

    const auto &prs = f.get_point_records ();
    for (const auto &p : prs)
        write_point_record (s, p);
}

inline void write_spoc_file_compressed (std::ostream &s, const spoc_file &f)
{
    // Check compression flag
    if (!f.get_header ().compressed)
        throw std::runtime_error ("Compressed writer can't write an uncompressed file");

    const auto &prs = f.get_point_records ();

    // Make sure the points records are OK
    if (!check_records (prs))
        throw std::runtime_error ("Invalid point records");

    // Stuff the data into vectors
    const size_t total_points = prs.size ();
    const size_t extra_fields = prs.empty ()
        ? 0
        : prs[0].extra.size ();
    std::vector<double> x (total_points);
    std::vector<double> y (total_points);
    std::vector<double> z (total_points);
    std::vector<uint32_t> c (total_points);
    std::vector<uint32_t> p (total_points);
    std::vector<uint16_t> i (total_points);
    std::vector<uint16_t> r (total_points);
    std::vector<uint16_t> g (total_points);
    std::vector<uint16_t> b (total_points);
    std::vector<std::vector<uint64_t>> e (extra_fields, std::vector<uint64_t>(total_points));

    for (size_t n = 0; n < total_points; ++n)
    {
        x[n] = prs[n].x;
        y[n] = prs[n].y;
        z[n] = prs[n].z;
        c[n] = prs[n].c;
        p[n] = prs[n].p;
        i[n] = prs[n].i;
        r[n] = prs[n].r;
        g[n] = prs[n].g;
        b[n] = prs[n].b;
        for (size_t j = 0; j < extra_fields; ++j)
            e[j][n] = prs[n].extra[j];
    }

    // Compress fields in parallel
    std::vector<std::vector<uint8_t>> fields (9);
    #pragma omp parallel sections
    {
        #pragma omp section
        { fields[0] = compress_field (x); }
        #pragma omp section
        { fields[1] = compress_field (y); }
        #pragma omp section
        { fields[2] = compress_field (z); }
        #pragma omp section
        { fields[3] = compress_field (c); }
        #pragma omp section
        { fields[4] = compress_field (p); }
        #pragma omp section
        { fields[5] = compress_field (i); }
        #pragma omp section
        { fields[6] = compress_field (r); }
        #pragma omp section
        { fields[7] = compress_field (g); }
        #pragma omp section
        { fields[8] = compress_field (b); }
    }

    // Compress extra fields in parallel
    std::vector<std::vector<uint8_t>> extras (e.size ());
    #pragma omp parallel for
    for (size_t j = 0; j < extras.size (); ++j)
        extras[j] = compress_field (e[j]);

    // Write the header
    const header &h = f.get_header ();
    write_header (s, h);

    // Write the compressed data
    for (size_t j = 0; j < fields.size (); ++j)
        write_compressed (s, fields[j]);

    for (size_t j = 0; j < extras.size (); ++j)
        write_compressed (s, extras[j]);
}

} // namespace spoc
