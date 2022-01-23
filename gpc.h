#pragma once

#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

namespace gpc
{

struct point_record
{
    double x;
    double y;
    double z;
    uint64_t c;
    uint64_t i;
    uint64_t r;
    uint64_t g;
    uint64_t b;
};

struct gpc_file
{
    char signature[4];
    std::vector<double> x;
    std::vector<double> y;
    std::vector<double> z;
    std::vector<uint64_t> c;
    std::vector<uint64_t> i;
    std::vector<uint64_t> r;
    std::vector<uint64_t> g;
    std::vector<uint64_t> b;
    std::vector<std::vector<uint64_t>> extra;
    std::string wkt;

    gpc_file ()
    {
        signature[0] = 'G'; // Geospatial
        signature[1] = 'P'; // Point
        signature[2] = 'C'; // Cloud
        signature[3] = '\0';
    }
    bool check () const
    {
        if (signature[0] != 'G') return false;
        if (signature[1] != 'P') return false;
        if (signature[2] != 'C') return false;
        if (signature[3] != '\0') return false;
        const size_t npoints = x.size ();
        if (y.size () != npoints) return false;
        if (z.size () != npoints) return false;
        if (c.size () != npoints) return false;
        if (i.size () != npoints) return false;
        if (r.size () != npoints) return false;
        if (g.size () != npoints) return false;
        if (b.size () != npoints) return false;
        for (auto &e : extra)
            if (e.size () != npoints) return false;
        return true;
    }
};

std::ostream& write_gpc_file (std::ostream &s, const gpc_file &f)
{
    // Make sure 'f' is valid
    if (!f.check ())
        throw std::runtime_error ("Invalid gpc file format");

    // Signature
    s.write (&f.signature[0], 4);

    // Number of points
    const uint64_t sz = f.x.size ();
    s.write (reinterpret_cast<const char*>(&sz), sizeof(uint64_t));

    // Data
    s.write (reinterpret_cast<const char*>(&f.x[0]), sz * sizeof (double));
    s.write (reinterpret_cast<const char*>(&f.y[0]), sz * sizeof (double));
    s.write (reinterpret_cast<const char*>(&f.z[0]), sz * sizeof (double));
    s.write (reinterpret_cast<const char*>(&f.c[0]), sz * sizeof (uint64_t));
    s.write (reinterpret_cast<const char*>(&f.i[0]), sz * sizeof (uint64_t));
    s.write (reinterpret_cast<const char*>(&f.r[0]), sz * sizeof (uint64_t));
    s.write (reinterpret_cast<const char*>(&f.g[0]), sz * sizeof (uint64_t));
    s.write (reinterpret_cast<const char*>(&f.b[0]), sz * sizeof (uint64_t));

    // Extra fields
    const uint64_t n = f.extra.size ();
    s.write (reinterpret_cast<const char*>(&n), sizeof(uint64_t));
    for (size_t i = 0; i < n; ++i)
        s.write (reinterpret_cast<const char*>(&f.extra[i]), sz * sizeof(uint64_t));

    // WKT
    s.write (reinterpret_cast<const char*>(&f.wkt[0]), f.wkt.size ());
    return s;
}

std::ostream& write_gpc_file (std::ostream &s,
    const std::vector<point_record> &point_records,
    const std::string &wkt)
{
    // Stuff the records into a gpc_file struct
    gpc_file f;
    const size_t sz = point_records.size ();
    f.x.resize (sz);
    f.y.resize (sz);
    f.z.resize (sz);
    f.c.resize (sz);
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
        f.i[i] = point_records[i].i;
        f.r[i] = point_records[i].r;
        f.g[i] = point_records[i].g;
        f.b[i] = point_records[i].b;
    }
    return write_gpc_file (s, f);
}

std::istream& read_gpc_file (std::istream &s, gpc_file &f)
{
    return s;
}

} // namespace gpc
