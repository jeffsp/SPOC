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

namespace detail
{

std::vector<std::string> split (const std::string &s, const std::string &delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::vector<std::string> res;

    while ((pos_end = s.find (delimiter, pos_start)) != std::string::npos) {
        res.push_back (s.substr (pos_start, pos_end - pos_start));
        pos_start = pos_end + delim_len;
    }

    res.push_back (s.substr (pos_start));
    return res;
}

} // namespace detail

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

// Filter based on coordinates less than or greater than some threshold
template<typename T>
inline T remove_coords (const T &f, const std::string &op)
{
    // Get an empty clone of the spoc file
    T g = f.clone_empty ();

    auto ops = detail::split(op, " ");

    if (ops.size() != 3)
        throw std::runtime_error("Did not find correct number of arguments for remove-coords option");

    // Generic value access function
    std::function<double(const spoc::point_record::point_record &p)> value;
    if (ops[0] == "x")
        value = [](const spoc::point_record::point_record &p) { return p.x; };
    else if (ops[0] == "y")
        value = [](const spoc::point_record::point_record &p) { return p.y; };
    else if (ops[0] == "z")
        value = [](const spoc::point_record::point_record &p) { return p.z; };
    else
        throw std::runtime_error("An invalid coordinate value was given");

    std::function<double(const double v1, const double v2)> compare;
    if (ops[1] == ">")
    {
        compare = [](const double v1, const double v2) {
            return v1 > v2;
        };
    }
    else if (ops[1] == "<")
    {
        compare = [](const double v1, const double v2) {
            return v1 < v2;
        };
    }
    else
        throw std::runtime_error("An invalid comparison operator was given");

    double compare_value = std::numeric_limits<double>::quiet_NaN();
    try
    {
        compare_value = std::stod(ops[2]);
    }
    catch (const std::exception &e)
    {
        std::string err_str = "Failed to convert comparison value to double for remove-coords option in spoc_filter";
        err_str = err_str + ". Did you format the option correctly?";
        throw std::runtime_error(err_str);
    }

    // Get a reference to the records
    const auto &prs = f.get_point_records ();

    // For each record
    for (const auto &p : prs)
        // Keep points that fail the comparison, i.e. remove points that pass
        // the comparison
        if (!compare(value(p), compare_value))
            g.push_back (p);

    return g;
}

} // namespace filter_app

} // namespace spoc
