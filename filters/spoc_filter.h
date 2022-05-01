#pragma once

#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>

namespace spoc_filter
{

const uint8_t MAJOR_VERSION = 0;
const uint8_t MINOR_VERSION = 1;

struct point_record
{
    double x, y, z;
    uint16_t c, p, i, r, g, b;
    uint64_t extra[8];
};

inline void read_header (std::istream &s,
    char signature[5],
    uint8_t &major_version,
    uint8_t &minor_version,
    size_t &npoints)
{
    s.read (reinterpret_cast<char*>(&signature[0]), sizeof(char));
    s.read (reinterpret_cast<char*>(&signature[1]), sizeof(char));
    s.read (reinterpret_cast<char*>(&signature[2]), sizeof(char));
    s.read (reinterpret_cast<char*>(&signature[3]), sizeof(char));
    signature[4] = '\0'; // null terminate
    s.read (reinterpret_cast<char*>(&major_version), sizeof(uint8_t));
    s.read (reinterpret_cast<char*>(&minor_version), sizeof(uint8_t));
    s.read (reinterpret_cast<char*>(&npoints), sizeof(uint64_t));
}

point_record read_point (std::istream &is)
{
    point_record p;
    is.read (reinterpret_cast<char *> (&p.x), sizeof (double));
    is.read (reinterpret_cast<char *> (&p.y), sizeof (double));
    is.read (reinterpret_cast<char *> (&p.z), sizeof (double));
    is.read (reinterpret_cast<char *> (&p.c), sizeof (uint16_t));
    is.read (reinterpret_cast<char *> (&p.p), sizeof (uint16_t));
    is.read (reinterpret_cast<char *> (&p.i), sizeof (uint16_t));
    is.read (reinterpret_cast<char *> (&p.r), sizeof (uint16_t));
    is.read (reinterpret_cast<char *> (&p.g), sizeof (uint16_t));
    is.read (reinterpret_cast<char *> (&p.b), sizeof (uint16_t));
    for (size_t i = 0; i < sizeof (p.extra) / sizeof (p.extra[0]); ++i)
        is.read (reinterpret_cast<char *> (&p.extra[i]), sizeof (uint64_t));
    return p;
}

void write_point (std::ostream &os, const point_record &p)
{
    os.write (reinterpret_cast<const char *> (&p.x), sizeof (double));
    os.write (reinterpret_cast<const char *> (&p.y), sizeof (double));
    os.write (reinterpret_cast<const char *> (&p.z), sizeof (double));
    os.write (reinterpret_cast<const char *> (&p.c), sizeof (uint16_t));
    os.write (reinterpret_cast<const char *> (&p.p), sizeof (uint16_t));
    os.write (reinterpret_cast<const char *> (&p.i), sizeof (uint16_t));
    os.write (reinterpret_cast<const char *> (&p.r), sizeof (uint16_t));
    os.write (reinterpret_cast<const char *> (&p.g), sizeof (uint16_t));
    os.write (reinterpret_cast<const char *> (&p.b), sizeof (uint16_t));
    for (size_t i = 0; i < sizeof (p.extra) / sizeof (p.extra[0]); ++i)
        os.write (reinterpret_cast<const char *> (&p.extra[i]), sizeof (uint64_t));
    os.flush ();
}

template<typename T>
int filter_entry (int argc, char **argv, T process_function)
{
    using namespace std;

    try
    {
        // Open input pipe
        ifstream ifs (argv[1]);
        if (!ifs)
            return -1;

        // Open output pipe
        ofstream ofs (argv[2]);
        if (!ofs)
            return -1;

        // Get header information
        char signature[5];
        uint8_t major_version;
        uint8_t minor_version;
        size_t npoints;
        read_header (ifs, signature, major_version, minor_version, npoints);

        // Check the header
        if (string (signature) != string ("SPOC"))
            throw runtime_error ("The header signature is incorrect");

        if (major_version != MAJOR_VERSION)
            throw runtime_error ("The major version does not match");

        if (minor_version > MINOR_VERSION)
            throw runtime_error ("The minor version is newer than the filter version");

        // Process the points one by one
        for (size_t i = 0; i < npoints; ++i)
        {
            // Get a point
            point_record p = spoc_filter::read_point (ifs);

            // Run it through the filter
            const auto q = process_function (p);

            // Write it back out
            write_point (ofs, q);
        }

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}

} // namespace spoc_filter
