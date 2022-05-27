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
#include <vector>

namespace spoc
{

// File format
struct header
{
    header ()
        : extra_size (0)
        , total_points (0)
    {
        signature[0] = 'S'; // Simple
        signature[1] = 'P'; // Point
        signature[2] = 'O';
        signature[3] = 'C'; // Cloud
        signature[4] = '\0'; // Terminate
    }
    bool check_signature () const
    {
        if (signature[0] != 'S') return false;
        if (signature[1] != 'P') return false;
        if (signature[2] != 'O') return false;
        if (signature[3] != 'C') return false;
        return true;
    }

    char signature[5];
    uint8_t major_version = MAJOR_VERSION;
    uint8_t minor_version = MINOR_VERSION;
    std::string wkt;
    size_t extra_size;
    size_t total_points;
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
    s << "extra_size " << h.extra_size << std::endl;
    s << "total_points " << h.total_points << std::endl;
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
    if (a.extra_size != b.extra_size) return false;
    if (a.total_points != b.total_points) return false;
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
    s.write (reinterpret_cast<const char*>(&h.extra_size), sizeof(uint64_t));
    s.write (reinterpret_cast<const char*>(&h.total_points), sizeof(uint64_t));
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
    s.read (reinterpret_cast<char*>(&h.extra_size), sizeof(uint64_t));
    s.read (reinterpret_cast<char*>(&h.total_points), sizeof(uint64_t));
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
    explicit point_record (const size_t extra_size)
        : x (0.0) , y (0.0) , z (0.0)
        , c (0) , p (0) , i (0)
        , r (0) , g (0) , b (0)
        , extra (extra_size)
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
    s.write (reinterpret_cast<const char*>(&p.x), sizeof(double));
    s.write (reinterpret_cast<const char*>(&p.y), sizeof(double));
    s.write (reinterpret_cast<const char*>(&p.z), sizeof(double));
    s.write (reinterpret_cast<const char*>(&p.c), sizeof(uint32_t));
    s.write (reinterpret_cast<const char*>(&p.p), sizeof(uint32_t));
    s.write (reinterpret_cast<const char*>(&p.i), sizeof(uint16_t));
    s.write (reinterpret_cast<const char*>(&p.r), sizeof(uint16_t));
    s.write (reinterpret_cast<const char*>(&p.g), sizeof(uint16_t));
    s.write (reinterpret_cast<const char*>(&p.b), sizeof(uint16_t));
    for (size_t j = 0; j < p.extra.size (); ++j)
        s.write (reinterpret_cast<const char*>(&p.extra[j]), sizeof(uint64_t));
    s.flush ();
}

// Record I/O
inline point_record read_point_record (std::istream &s, const size_t extra_size)
{
    point_record p;
    p.extra.resize (extra_size);
    s.read (reinterpret_cast<char*>(&p.x), sizeof(double));
    s.read (reinterpret_cast<char*>(&p.y), sizeof(double));
    s.read (reinterpret_cast<char*>(&p.z), sizeof(double));
    s.read (reinterpret_cast<char*>(&p.c), sizeof(uint32_t));
    s.read (reinterpret_cast<char*>(&p.p), sizeof(uint32_t));
    s.read (reinterpret_cast<char*>(&p.i), sizeof(uint16_t));
    s.read (reinterpret_cast<char*>(&p.r), sizeof(uint16_t));
    s.read (reinterpret_cast<char*>(&p.g), sizeof(uint16_t));
    s.read (reinterpret_cast<char*>(&p.b), sizeof(uint16_t));
    for (size_t j = 0; j < p.extra.size (); ++j)
        s.read (reinterpret_cast<char*>(&p.extra[j]), sizeof(uint64_t));
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
using points = std::vector<point_record>;

// Helper function
bool check_records (const points &point_records)
{
    // Degenerate case
    if (point_records.empty ())
        return true;

    // Make sure they all have the same number of extra fields
    const size_t extra_size = point_records[0].extra.size ();

    if (std::any_of (point_records.cbegin(), point_records.cend(),
        [&](const point_record &p)
        { return p.extra.size () != extra_size; }))
        return false;

    return true;
}

// File I/O
inline void write_spoc_file (std::ostream &s,
    const std::string &wkt,
    const points &point_records)
{
    // Make sure the points records are OK
    if (!check_records (point_records))
        throw std::runtime_error ("Invalid point records");

    header h;
    h.wkt = wkt;
    h.extra_size = point_records.empty () ? 0 : point_records[0].extra.size ();
    h.total_points = point_records.size ();

    // Write the file
    write_header (s, h);
    for (const auto &p : point_records)
        write_point_record (s, p);
}

inline void read_spoc_file (std::istream &s,
    header &h,
    points &point_records)
{
    // Read the header
    h = read_header (s);

    // Read the data
    point_records.resize (h.total_points);
    for (size_t i = 0; i < point_records.size (); ++i)
        point_records[i] = read_point_record (s, h.extra_size);
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

inline void write_spoc_file_compressed (std::ostream &s,
    const std::string &wkt,
    const points &point_records)
{
    // Make sure the points records are OK
    if (!check_records (point_records))
        throw std::runtime_error ("Invalid point records");

    // Stuff the data into vectors
    const size_t total_points = point_records.size ();
    const size_t extra_size = point_records.empty ()
        ? 0
        : point_records[0].extra.size ();
    std::vector<double> x (total_points);
    std::vector<double> y (total_points);
    std::vector<double> z (total_points);
    std::vector<uint32_t> c (total_points);
    std::vector<uint32_t> p (total_points);
    std::vector<uint16_t> i (total_points);
    std::vector<uint16_t> r (total_points);
    std::vector<uint16_t> g (total_points);
    std::vector<uint16_t> b (total_points);
    std::vector<std::vector<uint64_t>> e (extra_size, std::vector<uint64_t>(total_points));

    for (size_t n = 0; n < total_points; ++n)
    {
        x[n] = point_records[n].x;
        y[n] = point_records[n].y;
        z[n] = point_records[n].z;
        c[n] = point_records[n].c;
        p[n] = point_records[n].p;
        i[n] = point_records[n].i;
        r[n] = point_records[n].r;
        g[n] = point_records[n].g;
        b[n] = point_records[n].b;
        for (size_t j = 0; j < extra_size; ++j)
            e[j][n] = point_records[n].extra[j];
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
    header h;
    h.wkt = wkt;
    h.extra_size = extra_size;
    h.total_points = total_points;
    write_header (s, h);

    // Write the compressed data
    for (size_t j = 0; j < fields.size (); ++j)
        write_compressed (s, fields[j]);

    for (size_t j = 0; j < extras.size (); ++j)
        write_compressed (s, extras[j]);
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

inline void read_spoc_file_compressed (std::istream &s,
    header &h,
    points &point_records)
{
    // Read the header
    h = read_header (s);

    // Read the data
    point_records.resize (h.total_points);

    // Read data
    std::vector<double> x = read_compressed<double> (s, h.total_points);
    std::vector<double> y = read_compressed<double> (s, h.total_points);
    std::vector<double> z = read_compressed<double> (s, h.total_points);
    std::vector<uint32_t> c = read_compressed<uint32_t> (s, h.total_points);
    std::vector<uint32_t> p = read_compressed<uint32_t> (s, h.total_points);
    std::vector<uint16_t> i = read_compressed<uint16_t> (s, h.total_points);
    std::vector<uint16_t> r = read_compressed<uint16_t> (s, h.total_points);
    std::vector<uint16_t> g = read_compressed<uint16_t> (s, h.total_points);
    std::vector<uint16_t> b = read_compressed<uint16_t> (s, h.total_points);

    std::vector<std::vector<uint64_t>> extra (h.extra_size);
    for (size_t j = 0; j < extra.size (); ++j)
        extra[j] = read_compressed<uint64_t> (s, h.total_points);

    // Copy them into the point records
    point_records.resize (h.total_points);

    #pragma omp parallel for
    for (size_t n = 0; n < point_records.size (); ++n)
    {
        if (!x.empty ()) point_records[n].x = x[n];
        if (!y.empty ()) point_records[n].y = y[n];
        if (!z.empty ()) point_records[n].z = z[n];
        if (!c.empty ()) point_records[n].c = c[n];
        if (!p.empty ()) point_records[n].p = p[n];
        if (!i.empty ()) point_records[n].i = i[n];
        if (!r.empty ()) point_records[n].r = r[n];
        if (!g.empty ()) point_records[n].g = g[n];
        if (!b.empty ()) point_records[n].b = b[n];
        point_records[n].extra.resize (h.extra_size);
        for (size_t j = 0; j < point_records[n].extra.size (); ++j)
            if (!extra[j].empty ()) point_records[n].extra[j] = extra[j][n];
    }
}

// Helper functions
std::vector<double> get_x (const points &p)
{
    std::vector<double> x (p.size ());
    for (size_t n = 0; n < p.size (); ++n)
        x[n] = p[n].x;
    return x;
}

std::vector<double> get_y (const points &p)
{
    std::vector<double> x (p.size ());
    for (size_t n = 0; n < p.size (); ++n)
        x[n] = p[n].y;
    return x;
}

std::vector<double> get_z (const points &p)
{
    std::vector<double> x (p.size ());
    for (size_t n = 0; n < p.size (); ++n)
        x[n] = p[n].z;
    return x;
}

std::vector<uint32_t> get_c (const points &p)
{
    std::vector<uint32_t> x (p.size ());
    for (size_t n = 0; n < p.size (); ++n)
        x[n] = p[n].c;
    return x;
}

std::vector<uint32_t> get_p (const points &p)
{
    std::vector<uint32_t> x (p.size ());
    for (size_t n = 0; n < p.size (); ++n)
        x[n] = p[n].p;
    return x;
}

std::vector<uint16_t> get_i (const points &p)
{
    std::vector<uint16_t> x (p.size ());
    for (size_t n = 0; n < p.size (); ++n)
        x[n] = p[n].i;
    return x;
}

std::vector<uint16_t> get_r (const points &p)
{
    std::vector<uint16_t> x (p.size ());
    for (size_t n = 0; n < p.size (); ++n)
        x[n] = p[n].r;
    return x;
}

std::vector<uint16_t> get_g (const points &p)
{
    std::vector<uint16_t> x (p.size ());
    for (size_t n = 0; n < p.size (); ++n)
        x[n] = p[n].g;
    return x;
}

std::vector<uint16_t> get_b (const points &p)
{
    std::vector<uint16_t> x (p.size ());
    for (size_t n = 0; n < p.size (); ++n)
        x[n] = p[n].b;
    return x;
}

size_t get_extra_size (const points &p)
{
    if (p.empty ())
        return 0;
    return p[0].extra.size ();
}

std::vector<uint64_t> get_extra (const size_t k, const points &p)
{
    std::vector<uint64_t> x (p.size ());
    for (size_t n = 0; n < p.size (); ++n)
    {
        assert (k < p[n].extra.size ());
        x[n] = p[n].extra[k];
    }
    return x;
}

struct spoc_file
{
    header h;
    points p;
};

} // namespace spoc
