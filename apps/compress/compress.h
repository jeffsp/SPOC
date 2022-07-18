#pragma once

#include "app_utils.h"
#include "spoc.h"

namespace spoc
{

namespace compress_app
{

inline void change_precision (spoc::io::spoc_file &f, const unsigned precision)
{
    REQUIRE (precision != 0);
    REQUIRE (precision < 53);
    uint64_t mask = 0xFFFFFFFFFFFFFFFF << (52 - precision);
    std::clog << "MASK = ";
    std::clog << std::hex << std::uppercase
        << mask
        << std::nouppercase << std::dec
        << std::endl;

    for (size_t i = 0; i < f.get_point_records ().size (); ++i)
    {
        // Mask off bits
        double x = f[i].x;
        double y = f[i].y;
        double z = f[i].z;
        uint64_t *px = reinterpret_cast<uint64_t *> (&x);
        uint64_t *py = reinterpret_cast<uint64_t *> (&y);
        uint64_t *pz = reinterpret_cast<uint64_t *> (&z);
        *px = (*px & mask);
        *py = (*py & mask);
        *pz = (*pz & mask);
        // Set them
        f[i].x = x;
        f[i].y = y;
        f[i].z = z;
    }
}

} // namespace compress_app

} // namespace spoc
