#pragma once

#include <cmath>

namespace spoc
{

namespace affine
{

// Add offset
template<typename T>
void add_x (T &p, const double v)
{
#pragma omp parallel for
    for (size_t i = 0; i < p.size (); ++i)
        p[i].x += v;
}

// Add offset
template<typename T>
void add_y (T &p, const double v)
{
#pragma omp parallel for
    for (size_t i = 0; i < p.size (); ++i)
        p[i].y += v;
}

// Add offset
template<typename T>
void add_z (T &p, const double v)
{
#pragma omp parallel for
    for (size_t i = 0; i < p.size (); ++i)
        p[i].z += v;
}

// Add offset
template<typename T>
void add (T &p, const typename T::value_type &v)
{
#pragma omp parallel for
    for (size_t i = 0; i < p.size (); ++i)
    {
        p[i].x += v.x;
        p[i].y += v.y;
        p[i].z += v.z;
    }
}

// Rotate about the X, Y, or Z axis
template<typename T,typename XOP,typename YOP,typename ZOP>
void rotate (T &p, XOP xop, YOP yop, ZOP zop)
{
    // Process the points
#pragma omp parallel for
    for (size_t i = 0; i < p.size (); ++i)
    {
        const double x = p[i].x;
        const double y = p[i].y;
        const double z = p[i].z;

        // Rotate the point
        p[i].x = xop (x, y, z);
        p[i].y = yop (x, y, z);
        p[i].z = zop (x, y, z);
    }
}

template<typename T>
void rotate_x (T &p, const double degrees)
{
    // Convert degrees to radians
    const double r = degrees * M_PI / 180.0;
    auto xop = [&r] (const double x, const double y, const double z) -> double { return x; };
    auto yop = [&r] (const double x, const double y, const double z) -> double { return y * cos (r) - z * sin (r); };
    auto zop = [&r] (const double x, const double y, const double z) -> double { return y * sin (r) + z * cos (r); };
    rotate (p, xop, yop, zop);
}

template<typename T>
void rotate_y (T &p, const double degrees)
{
    // Convert degrees to radians
    const double r = degrees * M_PI / 180.0;
    auto xop = [&r] (const double x, const double y, const double z) -> double { return x * cos (r) - z * sin (r); };
    auto yop = [&r] (const double x, const double y, const double z) -> double { return y; };
    auto zop = [&r] (const double x, const double y, const double z) -> double { return x * sin (r) + z * cos (r); };
    rotate (p, xop, yop, zop);
}

template<typename T>
void rotate_z (T &p, const double degrees)
{
    // Convert degrees to radians
    const double r = degrees * M_PI / 180.0;
    auto xop = [&r] (const double x, const double y, const double z) -> double { return x * cos (r) - y * sin (r); };
    auto yop = [&r] (const double x, const double y, const double z) -> double { return x * sin (r) + y * cos (r); };
    auto zop = [&r] (const double x, const double y, const double z) -> double { return z; };
    rotate (p, xop, yop, zop);
}

// Scale
template<typename T>
void scale_x (T &p, const double v)
{
#pragma omp parallel for
    for (size_t i = 0; i < p.size (); ++i)
        p[i].x = p[i].x * v;
}

// Scale
template<typename T>
void scale_y (T &p, const double v)
{
#pragma omp parallel for
    for (size_t i = 0; i < p.size (); ++i)
        p[i].y = p[i].y * v;
}

// Scale
template<typename T>
void scale_z (T &p, const double v)
{
#pragma omp parallel for
    for (size_t i = 0; i < p.size (); ++i)
        p[i].z = p[i].z * v;
}

} // namespace affine

} // namespace spoc
