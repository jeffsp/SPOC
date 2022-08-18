#pragma once
#include "spoc/compression.h"
#include "spoc/contracts.h"
#include "spoc/file.h"
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

namespace io
{

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
    const auto c = spoc::compression::compress (reinterpret_cast<const uint8_t *> (&x[0]), x.size () * sizeof(T), compression_level);

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
    spoc::compression::decompress (c, reinterpret_cast<uint8_t *> (&x[0]), x.size () * sizeof(T));

    return x;
}

/// Helper I/O function
/// @param s Input stream
/// @param total_points Number of records to read
/// @param extra_fields Number of extra fields in each record
inline point_record::point_records read_uncompressed_points (std::istream &s,
    const size_t total_points,
    const size_t extra_fields)
{
    // Read the data
    point_record::point_records p (total_points);
    for (size_t i = 0; i < p.size (); ++i)
        p[i] = point_record::read_point_record (s, extra_fields);

    return p;
}

/// Helper I/O function
/// @param s Input stream
inline spoc::file::spoc_file read_spoc_file_uncompressed (std::istream &s)
{
    // Read the header
    header::header h = header::read_header (s);

    // Check compression flag
    if (h.compressed)
        throw std::runtime_error ("Uncompressed reader can't read a compressed file");

    // Read the data
    point_record::point_records prs = read_uncompressed_points (s, h.total_points, h.extra_fields);

    // Create the file
    spoc::file::spoc_file f (h.wkt, h.extra_fields, false);

    // Set the point records
    f.set_point_records (prs);

    return f;
}

/// Helper I/O function
/// @param s Input stream
/// @param total_points Number of records to read
/// @param extra_fields Number of extra fields in each record
inline point_record::point_records read_compressed_points (std::istream &s,
    const size_t total_points,
    const size_t extra_fields)
{
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
    point_record::point_records prs (total_points);

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

/// Helper I/O function
/// @param s Input stream
inline spoc::file::spoc_file read_spoc_file_compressed (std::istream &s)
{
    // Read the header
    header::header h = header::read_header (s);

    // Check compression flag
    if (!h.compressed)
        throw std::runtime_error ("Compressed reader can't read an uncompressed file");

    // Read the data
    point_record::point_records prs = read_compressed_points (s, h.total_points, h.extra_fields);

    // Create the file
    spoc::file::spoc_file f (h.wkt, h.extra_fields, true);

    // Set the point records
    f.set_point_records (prs);

    return f;
}

/// Helper I/O function
/// @param s Input stream
inline spoc::file::spoc_file read_spoc_file (std::istream &s)
{
    // Read the header
    header::header h = header::read_header (s);

    // The points
    point_record::point_records prs;

    // Check compression flag
    if (h.compressed)
        prs = read_compressed_points (s, h.total_points, h.extra_fields);
    else
        prs = read_uncompressed_points (s, h.total_points, h.extra_fields);

    // Create the file
    spoc::file::spoc_file f (h.wkt, h.extra_fields, h.compressed);

    // Set the point records
    f.set_point_records (prs);

    return f;
}

/// Helper I/O function
/// @param s Output stream
/// @param f File structure to write
inline void write_spoc_file_uncompressed (std::ostream &s, const spoc::file::spoc_file &f)
{
    // Check compression flag
    if (f.get_header ().compressed)
        throw std::runtime_error ("Uncompressed writer can't write a compressed file");

    const auto &prs = f.get_point_records ();
    const size_t extra_size = prs.empty () ? 0 : prs[0].extra.size ();

    // Make sure the points records are OK
    if (!check_records (f.get_point_records (), extra_size))
        throw std::runtime_error ("Invalid point records");

    // Write the file
    write_header (s, f.get_header ());

    for (const auto &p : prs)
        write_point_record (s, p);
}

/// Helper I/O function
/// @param s Output stream
/// @param f File structure to write
inline void write_spoc_file_compressed (std::ostream &s, const spoc::file::spoc_file &f)
{
    // Check compression flag
    if (!f.get_header ().compressed)
        throw std::runtime_error ("Compressed writer can't write an uncompressed file");

    const auto &prs = f.get_point_records ();
    const size_t extra_size = prs.empty () ? 0 : prs[0].extra.size ();

    // Make sure the points records are OK
    if (!check_records (prs, extra_size))
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
    const header::header &h = f.get_header ();
    write_header (s, h);

    // Write the compressed data
    for (size_t j = 0; j < fields.size (); ++j)
        write_compressed (s, fields[j]);

    for (size_t j = 0; j < extras.size (); ++j)
        write_compressed (s, extras[j]);
}

/// Helper I/O function
/// @param s Output stream
/// @param f File structure to write
inline void write_spoc_file (std::ostream &s, const spoc::file::spoc_file &f)
{
    // Check compression flag
    if (f.get_header ().compressed)
        write_spoc_file_compressed (s, f);
    else
        write_spoc_file_uncompressed (s, f);
}

} // namespace io

} // namespace spoc
