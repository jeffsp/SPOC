#pragma once
#include "spoc/version.h"
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

namespace spoc
{

namespace header
{

/// SPOC file header
struct header
{
    /// @brief Constructor
    /// @param major_version Version information
    /// @param minor_version Version information
    /// @param wkt Well known text string
    /// @param extra_fields Number of extra fields in each point record
    /// @param total_points Total points in the SPOC file
    /// @param compressed Compression flag
    header (const uint8_t major_version,
            const uint8_t minor_version,
            const std::string &wkt,
            const uint8_t extra_fields,
            const size_t total_points,
            const bool compressed = false)
        : major_version (major_version)
        , minor_version (minor_version)
        , wkt (wkt)
        , extra_fields (extra_fields)
        , total_points (total_points)
        , compressed (compressed)
    {
        signature[0] = 'S'; // Simple
        signature[1] = 'P'; // Point
        signature[2] = 'O';
        signature[3] = 'C'; // Cloud
        signature[4] = '\0'; // Terminate
        if (wkt.size () > 0xFFFF)
            throw std::runtime_error ("The OGC WKT length may not exceed 65535");
    }
    /// @brief Constructor
    /// @param wkt Well known text string
    /// @param extra_fields Number of extra fields in each point record
    /// @param total_points Total points in the SPOC file
    /// @param compressed Compression flag
    header (const std::string &wkt,
            const uint8_t extra_fields,
            const size_t total_points,
            const bool compressed = false)
        : header (MAJOR_VERSION, MINOR_VERSION, wkt, extra_fields, total_points, compressed)
    {
    }
    /// @brief Constructor
    header ()
        : header (MAJOR_VERSION, MINOR_VERSION, std::string (), 0, 0, false)
    {
    }
    /// @brief Check to make sure the header contains a valid signature
    bool check_signature () const
    {
        if (signature[0] != 'S') return false;
        if (signature[1] != 'P') return false;
        if (signature[2] != 'O') return false;
        if (signature[3] != 'C') return false;
        return true;
    }
    /// @brief Check to make sure the header structure is valid
    bool is_valid () const
    {
        if (!check_signature ())
            return false;
        if (wkt.size () > 0xFFFF)
            return false;
        return true;
    }

    /// Signature characters
    char signature[5];
    /// Version information
    uint8_t major_version;
    /// Version information
    uint8_t minor_version;
    /// Well-known-text string
    std::string wkt;
    /// The number of extra fields in each point record
    uint8_t extra_fields;
    /// The total number of points in the file
    size_t total_points;
    /// A flag that indicates if the records are compressed or not
    uint8_t compressed;
};

/// Helper relational operator
/// @param a First header
/// @param b Second header
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

/// Helper relational operator
/// @param a First header
/// @param b Second header
inline bool operator!= (const header &a, const header &b)
{
    for (auto i : {0, 1, 2, 3})
        // cppcheck-suppress useStlAlgorithm
        if (a.signature[i] != b.signature[i]) return true;
    if (a.major_version != b.major_version) return true;
    if (a.minor_version != b.minor_version) return true;
    if (a.wkt != b.wkt) return true;
    if (a.extra_fields != b.extra_fields) return true;
    if (a.total_points != b.total_points) return true;
    if (a.compressed != b.compressed) return true;
    return false;
}

/// I/O function
/// @param s Output stream
/// @param h Header struct
inline void write_header (std::ostream &s, const header &h)
{
    if (!h.check_signature ())
        throw std::runtime_error ("Invalid spoc file format");
    s.write (reinterpret_cast<const char*>(h.signature), 4 * sizeof(char));
    s.write (reinterpret_cast<const char*>(&h.major_version), sizeof(uint8_t));
    s.write (reinterpret_cast<const char*>(&h.minor_version), sizeof(uint8_t));
    const uint16_t len = h.wkt.size ();
    s.write (reinterpret_cast<const char*>(&len), sizeof(uint16_t));
    s.write (reinterpret_cast<const char*>(&h.wkt[0]), h.wkt.size ());
    s.write (reinterpret_cast<const char*>(&h.extra_fields), sizeof(uint8_t));
    s.write (reinterpret_cast<const char*>(&h.total_points), sizeof(uint64_t));
    s.write (reinterpret_cast<const char*>(&h.compressed), sizeof(uint8_t));
    s.flush ();
}

/// I/O function
/// @param s Input stream
inline header read_header (std::istream &s)
{
    header h;
    s.read (reinterpret_cast<char*>(&h.signature), 4 * sizeof(char));
    if (!h.check_signature ())
        throw std::runtime_error ("Invalid spoc file format");
    s.read (reinterpret_cast<char*>(&h.major_version), sizeof(uint8_t));
    if (h.major_version != MAJOR_VERSION)
        throw std::runtime_error ("Incompatible major version number");
    s.read (reinterpret_cast<char*>(&h.minor_version), sizeof(uint8_t));
    // See the note in the `test_header.cpp` unit test. When the major
    // version gets bumped above 0, this behavior should change to
    // allow for reading headers with unequal minor versions.
    if (h.minor_version != MINOR_VERSION)
        throw std::runtime_error ("Incompatible minor version number");
    uint16_t len = 0;
    s.read (reinterpret_cast<char*>(&len), sizeof(uint16_t));
    h.wkt.resize (len);
    s.read (reinterpret_cast<char*>(&h.wkt[0]), len);
    s.read (reinterpret_cast<char*>(&h.extra_fields), sizeof(uint8_t));
    s.read (reinterpret_cast<char*>(&h.total_points), sizeof(uint64_t));
    s.read (reinterpret_cast<char*>(&h.compressed), sizeof(uint8_t));
    return h;
}

/// Helper I/O function
/// @param s Output stream
/// @param h Header struct
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
    s << "extra_fields " << static_cast<unsigned> (h.extra_fields) << std::endl;
    s << "total_points " << h.total_points << std::endl;
    s << "compressed " << (h.compressed ? "true" : "false")  << std::endl;
    return s;
}

} // namespace header

} // namespace spoc
