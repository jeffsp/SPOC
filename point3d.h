#pragma once

namespace spc
{

// A point in 3D space
template<typename T>
struct point { T x, y, z; };

// Point operators

bool operator== (const point<double> &a, const point<double> &b)
{
    if (a.x != b.x) return false;
    if (a.y != b.y) return false;
    if (a.z != b.z) return false;
    return true;
}

bool operator!= (const point<double> &a, const point<double> &b)
{
    return !(a == b);
}

std::ostream &operator<< (std::ostream &s, const point<double> &p)
{
    s << ' ' << p.x;
    s << ' ' << p.y;
    s << ' ' << p.z;
    return s;
}

// Point functions

// All dimensions <=
bool all_le (const point<double> &a, const point<double> &b)
{
    if (a.x > b.x) return false;
    if (a.y > b.y) return false;
    if (a.z > b.z) return false;
    return true;
}

}
