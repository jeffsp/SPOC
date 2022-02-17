#pragma once

#include <iostream>

namespace spc
{

// A point in 3D space
template<typename T>
struct point { T x, y, z; };

// Point operators

inline bool operator== (const point<double> &a, const point<double> &b)
{
    if (a.x != b.x) return false;
    if (a.y != b.y) return false;
    if (a.z != b.z) return false;
    return true;
}

inline bool operator!= (const point<double> &a, const point<double> &b)
{
    return !(a == b);
}

inline std::ostream &operator<< (std::ostream &s, const point<double> &p)
{
    s << ' ' << p.x;
    s << ' ' << p.y;
    s << ' ' << p.z;
    return s;
}

}
