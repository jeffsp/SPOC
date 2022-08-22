#pragma once

#include "spoc/app_utils.h"
#include "spoc/spoc.h"
#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <limits>
#include <random>
#include <vector>
#include <unordered_map>

namespace spoc
{

namespace filter_app
{

template<typename T,typename U>
inline T keep_classes (const T &f, const U &c)
{
    // Get an empty clone of the spoc file
    T g = f.clone_empty ();

    // Get a reference to the records
    const auto &prs = f.get_point_records ();

    // For each record
    for (const auto &p : prs)
        // If it's in the set, keep it
        if (c.find (p.c) != c.end ())
            g.push_back (p);

    return g;
}

template<typename T,typename U>
inline T remove_classes (const T &f, const U &c)
{
    // Get an empty clone of the spoc file
    T g = f.clone_empty ();

    // Get a reference to the records
    const auto &prs = f.get_point_records ();

    // For each record
    for (const auto &p : prs)
        // If it's not in the set, keep it
        if (c.find (p.c) == c.end ())
            g.push_back (p);

    return g;
}

template<typename T>
inline T unique_xyz (const T &f, const size_t random_seed)
{
    // Get an empty clone of the spoc file
    T g = f.clone_empty ();

    // Get a reference to the records
    const auto &prs = f.get_point_records ();

    // Get the indexes into f of the unique locations
    const auto indexes = spoc::subsampling::get_unique_xyz_indexes (prs, random_seed);

    // Add them
    for (auto i : indexes)
        g.push_back (prs[i]); // cppcheck-suppress useStlAlgorithm

    return g;
}

template<typename T>
inline T subsample (const T &f, const double res, const size_t random_seed)
{
    // Get an empty clone of the spoc file
    T g = f.clone_empty ();

    // Get a reference to the records
    const auto &prs = f.get_point_records ();

    // Get the indexes into f
    const auto indexes = spoc::subsampling::get_subsample_indexes (prs, res, random_seed);

    // Add them
    for (auto i : indexes)
        g.push_back (prs[i]); // cppcheck-suppress useStlAlgorithm

    return g;
}

} // namespace filter_app

} // namespace spoc
