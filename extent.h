#pragma once

#include "point3d.h"

namespace spc
{

template<typename T>
std::pair<point<T>, point<T>> get_extent (const std::vector<point<T>> &points)
{
    point<T> minp { std::numeric_limits<T>::max (),
        std::numeric_limits<T>::max (),
        std::numeric_limits<T>::max ()};
    point<T> maxp { std::numeric_limits<T>::lowest (),
        std::numeric_limits<T>::lowest (),
        std::numeric_limits<T>::lowest ()};
    for (const auto &p : points)
    {
        minp.x = std::min (p.x, minp.x);
        minp.y = std::min (p.y, minp.y);
        minp.z = std::min (p.z, minp.z);
        maxp.x = std::max (p.x, maxp.x);
        maxp.y = std::max (p.y, maxp.y);
        maxp.z = std::max (p.z, maxp.z);
    }
    return {minp, maxp};
}

void change_extent (const point<double> &p,
    point<double> &minp,
    const point<double> &midp,
    point<double> &maxp)
{
    if (p.x < midp.x) maxp.x = midp.x; else minp.x = midp.x;
    if (p.y < midp.y) maxp.y = midp.y; else minp.y = midp.y;
    if (p.z < midp.z) maxp.z = midp.z; else minp.z = midp.z;
}

}
