#include <cstdint>
#include <fstream>
#include <iostream>

const uint8_t MAJOR_VERSION = 0;
const uint8_t MINOR_VERSION = 1;

struct point_record
{
    double x, y, z;
    uint16_t c, p, i, r, g, b;
    uint64_t extra[8];
};

using namespace std;

inline void read_binary (std::istream &s,
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

point_record read_point (ifstream &ifs)
{
    point_record p;
    ifs.read (reinterpret_cast<char *> (&p.x), sizeof (double));
    ifs.read (reinterpret_cast<char *> (&p.y), sizeof (double));
    ifs.read (reinterpret_cast<char *> (&p.z), sizeof (double));
    ifs.read (reinterpret_cast<char *> (&p.c), sizeof (uint16_t));
    ifs.read (reinterpret_cast<char *> (&p.p), sizeof (uint16_t));
    ifs.read (reinterpret_cast<char *> (&p.i), sizeof (uint16_t));
    ifs.read (reinterpret_cast<char *> (&p.r), sizeof (uint16_t));
    ifs.read (reinterpret_cast<char *> (&p.g), sizeof (uint16_t));
    ifs.read (reinterpret_cast<char *> (&p.b), sizeof (uint16_t));
    for (size_t i = 0; i < sizeof (p.extra) / sizeof (p.extra[0]); ++i)
        ifs.read (reinterpret_cast<char *> (&p.extra[i]), sizeof (uint64_t));
    return p;
}

void write_point (ofstream &ofs, const point_record &p)
{
    ofs.write (reinterpret_cast<const char *> (&p.x), sizeof (double));
    ofs.write (reinterpret_cast<const char *> (&p.y), sizeof (double));
    ofs.write (reinterpret_cast<const char *> (&p.z), sizeof (double));
    ofs.write (reinterpret_cast<const char *> (&p.c), sizeof (uint16_t));
    ofs.write (reinterpret_cast<const char *> (&p.p), sizeof (uint16_t));
    ofs.write (reinterpret_cast<const char *> (&p.i), sizeof (uint16_t));
    ofs.write (reinterpret_cast<const char *> (&p.r), sizeof (uint16_t));
    ofs.write (reinterpret_cast<const char *> (&p.g), sizeof (uint16_t));
    ofs.write (reinterpret_cast<const char *> (&p.b), sizeof (uint16_t));
    for (size_t i = 0; i < sizeof (p.extra) / sizeof (p.extra[0]); ++i)
        ofs.write (reinterpret_cast<const char *> (&p.extra[i]), sizeof (uint64_t));
    ofs.flush ();
}

void process (point_record &p)
{
    /*
    clog << std::fixed;
    clog.precision (16);
    clog << p.x << ", " << p.y << ", " << p.z << endl;
    */
}

int main (int argc, char **argv)
{
    ifstream ifs (argv[1]);
    if (!ifs)
        return -1;
    ofstream ofs (argv[2]);
    if (!ofs)
        return -1;

    // Get header information
    char signature[5];
    uint8_t major_version;
    uint8_t minor_version;
    size_t npoints;

    read_binary (ifs, signature, major_version, minor_version, npoints);

    clog << "Signature '" << signature << "'" << endl;
    clog << "Major version " << static_cast<int> (major_version) << endl;
    clog << "Minor version " << static_cast<int> (minor_version) << endl;
    clog << "Total points " << npoints << endl;

    if (string (signature) != string ("SPOC"))
    {
        clog << "The header signature is incorrect";
        return -1;
    }

    if (major_version != MAJOR_VERSION)
    {
        clog << "The major version does not match";
        return -1;
    }

    if (minor_version > MINOR_VERSION)
    {
        clog << "The minor version is newer than the filter version";
        return -1;
    }

    for (size_t i = 0; i < npoints; ++i)
    {
        point_record p = read_point (ifs);
        process (p);
        write_point (ofs, p);
    }

    return 0;
}
