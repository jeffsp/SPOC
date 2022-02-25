#pragma once

#include <iostream>

namespace spc
{

// A point in 3D space
template<typename T>
struct point
{
    T x = 0;
    T y = 0;
    T z = 0;
};

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

inline point<double> operator-= (point<double> &a, const point<double> &b)
{
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;
    return a;
}

inline point<double> operator- (const point<double> &a, const point<double> &b)
{
    auto c (a);
    return c -= b;
}

inline point<double> operator+= (point<double> &a, const point<double> &b)
{
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
    return a;
}

inline point<double> operator+ (const point<double> &a, const point<double> &b)
{
    auto c (a);
    return c += b;
}

inline std::ostream &operator<< (std::ostream &s, const point<double> &p)
{
    s << ' ' << p.x;
    s << ' ' << p.y;
    s << ' ' << p.z;
    return s;
}

inline bool operator< (const point<double> &a, const point<double> &b)
{
    if (a.x < b.x) return true;
    if (a.x > b.x) return false;
    if (a.y < b.y) return true;
    if (a.y > b.y) return false;
    if (a.z < b.z) return true;
    if (a.z > b.z) return false;
    return false;
}

}
