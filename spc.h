#pragma once

#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace spc
{

struct point_record
{
    double x;
    double y;
    double z;
    unsigned c; // classification
    unsigned p; // point source ID
    unsigned i; // intensity
    unsigned r;
    unsigned g;
    unsigned b;
};

struct spc_file
{
    char signature[4];
    std::vector<double> x;
    std::vector<double> y;
    std::vector<double> z;
    std::vector<uint16_t> c;
    std::vector<uint16_t> p;
    std::vector<uint16_t> i;
    std::vector<uint16_t> r;
    std::vector<uint16_t> g;
    std::vector<uint16_t> b;
    std::vector<std::vector<uint64_t>> extra;
    std::string wkt;

    spc_file ()
    {
        signature[0] = 'S'; // Simple
        signature[1] = 'P'; // Point
        signature[2] = 'C'; // Cloud
        signature[3] = '\0';
    }
    bool check () const
    {
        // Check signature
        if (signature[0] != 'S') return false;
        if (signature[1] != 'P') return false;
        if (signature[2] != 'C') return false;
        if (signature[3] != '\0') return false;

        // Check data
        const size_t npoints = x.size ();
        if (y.size () != npoints) return false;
        if (z.size () != npoints) return false;
        if (c.size () != npoints) return false;
        if (p.size () != npoints) return false;
        if (i.size () != npoints) return false;
        if (r.size () != npoints) return false;
        if (g.size () != npoints) return false;
        if (b.size () != npoints) return false;
        for (auto &e : extra)
            if (e.size () != npoints) return false;
        return true;
    }
};

inline void write_spc_file (std::ostream &s, const spc_file &f)
{
    // Make sure 'f' is valid
    if (!f.check ())
        throw std::runtime_error ("Invalid spc file format");

    // Signature
    s.write (&f.signature[0], 4);

    // Number of points
    const uint64_t n = f.x.size ();
    s.write (reinterpret_cast<const char*>(&n), sizeof(uint64_t));

    // Data
    s.write (reinterpret_cast<const char*>(&f.x[0]), n * sizeof (double));
    s.write (reinterpret_cast<const char*>(&f.y[0]), n * sizeof (double));
    s.write (reinterpret_cast<const char*>(&f.z[0]), n * sizeof (double));
    s.write (reinterpret_cast<const char*>(&f.c[0]), n * sizeof (uint16_t));
    s.write (reinterpret_cast<const char*>(&f.p[0]), n * sizeof (uint16_t));
    s.write (reinterpret_cast<const char*>(&f.i[0]), n * sizeof (uint16_t));
    s.write (reinterpret_cast<const char*>(&f.r[0]), n * sizeof (uint16_t));
    s.write (reinterpret_cast<const char*>(&f.g[0]), n * sizeof (uint16_t));
    s.write (reinterpret_cast<const char*>(&f.b[0]), n * sizeof (uint16_t));

    // Extra fields
    const uint64_t sz = f.extra.size ();
    s.write (reinterpret_cast<const char*>(&sz), sizeof(uint64_t));
    for (size_t i = 0; i < sz; ++i)
        s.write (reinterpret_cast<const char*>(&f.extra[i]), n * sizeof(uint64_t));

    // WKT
    const uint64_t len = f.wkt.size ();
    s.write (reinterpret_cast<const char*>(&len), sizeof(uint64_t));
    s.write (reinterpret_cast<const char*>(&f.wkt[0]), f.wkt.size ());
}

inline void write_spc_file (std::ostream &s,
    const std::vector<point_record> &point_records,
    const std::string &wkt)
{
    // Stuff the records into a spc_file struct
    spc_file f;
    const size_t sz = point_records.size ();
    f.x.resize (sz);
    f.y.resize (sz);
    f.z.resize (sz);
    f.c.resize (sz);
    f.p.resize (sz);
    f.i.resize (sz);
    f.r.resize (sz);
    f.g.resize (sz);
    f.b.resize (sz);
    for (size_t i = 0; i < sz; ++i)
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
    }
    f.wkt = wkt;

    write_spc_file (s, f);
}

inline void read_spc_file (std::istream &s, spc_file &f)
{
    // Read into tmp
    spc_file tmp_f;

    // Read signature
    s.read (&tmp_f.signature[0], 4);

    // Read number of points
    uint64_t n = 0;
    s.read (reinterpret_cast<char*>(&n), sizeof(uint64_t));

    // Resize vectors
    tmp_f.x.resize (n);
    tmp_f.y.resize (n);
    tmp_f.z.resize (n);
    tmp_f.c.resize (n);
    tmp_f.p.resize (n);
    tmp_f.i.resize (n);
    tmp_f.r.resize (n);
    tmp_f.g.resize (n);
    tmp_f.b.resize (n);

    // Read data
    s.read (reinterpret_cast<char*>(&tmp_f.x[0]), n * sizeof (double));
    s.read (reinterpret_cast<char*>(&tmp_f.y[0]), n * sizeof (double));
    s.read (reinterpret_cast<char*>(&tmp_f.z[0]), n * sizeof (double));
    s.read (reinterpret_cast<char*>(&tmp_f.c[0]), n * sizeof (uint16_t));
    s.read (reinterpret_cast<char*>(&tmp_f.p[0]), n * sizeof (uint16_t));
    s.read (reinterpret_cast<char*>(&tmp_f.i[0]), n * sizeof (uint16_t));
    s.read (reinterpret_cast<char*>(&tmp_f.r[0]), n * sizeof (uint16_t));
    s.read (reinterpret_cast<char*>(&tmp_f.g[0]), n * sizeof (uint16_t));
    s.read (reinterpret_cast<char*>(&tmp_f.b[0]), n * sizeof (uint16_t));

    // Extra fields
    uint64_t sz = 0;
    s.read (reinterpret_cast<char*>(&sz), sizeof(uint64_t));
    for (size_t i = 0; i < sz; ++i)
        s.read (reinterpret_cast<char*>(&tmp_f.extra[i]), n * sizeof(uint64_t));

    // WKT
    uint64_t len = 0;
    s.read (reinterpret_cast<char*>(&len), sizeof(uint64_t));
    tmp_f.wkt.resize (len);
    s.read (reinterpret_cast<char*>(&tmp_f.wkt[0]), len);

    // Make sure 'f' is valid
    if (!f.check ())
        throw std::runtime_error ("Invalid spc file format");

    // Commit
    f = tmp_f;
}

inline void read_spc_file (std::istream &s,
    std::vector<point_record> &point_records,
    std::string &wkt)
{
    // Read into spc_file struct
    spc_file f;
    read_spc_file (s, f);

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
    }

    // Commit
    point_records = tmp_point_records;
    wkt = f.wkt;
}

} // namespace spc
