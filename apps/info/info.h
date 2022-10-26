#pragma once

#include "spoc/spoc.h"
#include <iostream>
#include <map>
#include <sstream>
#include <unordered_map>

namespace spoc
{

namespace info_app
{

template<typename T>
inline std::map<std::string,size_t> get_class_count_map (const T &c)
{
    std::unordered_map<int,size_t> m;

    // Count each classification
    //
    // Use an unordered map for speed
    for (auto i : c)
        ++m[i];

    // Put them in a map so that they are sorted
    std::map<std::string,size_t> a;

    for (auto i : m)
    {
        std::stringstream s;
        s << std::setfill('0') << std::setw(3) << std::to_string (i.first);
        a[s.str ()] = i.second;
    }

    return a;
}

template<typename T>
inline std::map<std::string,double> get_metric_values (const T &p)
{
    using namespace spoc::extent;
    using namespace spoc::voxel;
    const auto e = get_extent (p);
    // Get the area
    const auto area = (e.maxp.x - e.minp.x) * (e.maxp.y - e.minp.y);
    const double resolution = 1.0;
    const auto vi = get_voxel_indexes (p, e, resolution);
    // Get the total number of unique voxel indexes
    const auto vis = get_voxel_index_set (vi);
    // Convert voxel indexes to grid indexes
    auto gi (vi);
    for (auto &i : gi)
        i.k = 0;
    // Get the total number of unique grid indexes
    const auto gis = get_voxel_index_set (gi);
    std::map<std::string,double> v;
    v["extent_point_density"] = p.size () / area;
    v["grid_point_density"] = static_cast<double> (p.size ()) / gis.size ();
    v["voxel_point_density"] = static_cast<double> (p.size ()) / vis.size ();
    return v;
}

inline std::map<std::string,std::string> get_metric_units ()
{
    return std::map<std::string,std::string> {
        { "extent_point_density", "pts/unit^2" },
        { "grid_point_density", "pts/unit^2" },
        { "voxel_point_density", "pts/unit^3" },
    };
}

template<typename T,typename U>
inline spoc::json::object get_summary_object (const U &x, const bool quartiles)
{
    // Return value
    spoc::json::object s;

    // Handle the empty case
    if (x.empty ())
    {
        s["size"] = 0;
        s["min"] = 0;
        s["max"] = 0;
        return s;
    }

    if (quartiles)
    {
        // Copy
        auto y (x);

        // Get Quartiles
        const size_t n = y.size ();
        std::sort (begin (y), end (y));
        s["range"] = y.back() - y.front();
        s["q0"] = y.front();
        s["q1"] = y[n / 4];
        s["q2"] = y[n / 2];
        s["q3"] = y[3 * n / 4];
        s["q4"] = y.back();
    }
    else
    {
        // Get min and max
        const auto min = *std::min_element (begin (x), end (x));
        const auto max = *std::max_element (begin (x), end (x));
        s["range"] = max - min;
        s["min"] = min;
        s["max"] = max;
    }
    return s;
}

template<typename T,typename U>
inline std::string get_summary_string (const std::string &label,
    const U &x,
    const bool compact,
    const bool quartiles)
{
    // Return value
    std::stringstream s;
    s.precision (3);
    s << std::fixed;

    // Handle the empty case
    if (x.empty ())
    {
        if (compact)
        {
            return std::string ();
        }
        else
        {
            s << label << "0" << std::endl;
            s << "size\t0" << std::endl;
            s << "min\t0" << std::endl;
            s << "max\t0" << std::endl;
            return s.str ();
        }
    }

    const size_t n = x.size ();

    if (quartiles)
    {
        // Copy
        auto y (x);

        // Get Quartiles
        std::sort (begin (y), end (y));
        if (compact)
        {
            s << label;
            // Range
            s << "range=" << y.back() - y.front();
            // q0
            s << ", " << y.front();
            // Step to q1
            s << " + " << y[n / 4] - y.front ();
            // Step to q2
            s << " + " << y[n / 2] - y[n / 4];
            // Step to q3
            s << " + " << y[3 * n / 4] - y[n / 2];
            // Step to q4
            s << " + " << y.back () - y[3 * n / 4];
            // Q4
            s << " = " << y.back() << std::endl;
        }
        else
        {
            s << label << n << std::endl;
            s << "range\t" << y.back() - y.front() << std::endl;
            s << "q0\t" << y.front() << std::endl;
            s << "q1\t" << y[n / 4] << std::endl;
            s << "q2\t" << y[n / 2] << std::endl;
            s << "q3\t" << y[3 * n / 4] << std::endl;
            s << "q4\t" << y.back() << std::endl;
        }
    }
    else
    {
        // Get min and max
        const auto min = *std::min_element (begin (x), end (x));
        const auto max = *std::max_element (begin (x), end (x));
        if (compact)
        {
            s << label;
            s << "range=" << max - min;
            s << ", min=" << min;
            s << ", max=" << max << std::endl;
        }
        else
        {
            s << label << n << std::endl;
            s << "range\t" << max - min << std::endl;
            s << "min\t" << min << std::endl;
            s << "max\t" << max << std::endl;
        }
    }

    return s.str ();
}

// Process a spoc file and write to 'os'
inline void process (std::ostream &os,
    const spoc::file::spoc_file &f,
    const bool json,
    const bool header_info,
    const bool summary_info,
    const bool classification_info,
    const bool metric_info,
    const bool compact,
    const bool quartiles)
{
    using namespace std;
    using namespace spoc;
    using namespace spoc::io;

    os.precision (15);
    os << fixed;

    if (json)
    {
        json::object j;

        if (header_info)
        {
            json::object h;
            h["major_version"] = f.get_header ().major_version;
            h["minor_version"] = f.get_header ().minor_version;
            h["wkt"] = f.get_header ().wkt;
            h["total_points"] = f.get_header ().total_points;
            h["compressed"] = f.get_header ().compressed;
            j["header"] = h;
        }

        if (summary_info)
        {
            json::object s;

            s["x"] = get_summary_object<double> (get_x (f.get_point_records ()), quartiles);
            s["y"] = get_summary_object<double> (get_y (f.get_point_records ()), quartiles);
            s["z"] = get_summary_object<double> (get_z (f.get_point_records ()), quartiles);
            s["c"] = get_summary_object<uint32_t> (get_c (f.get_point_records ()), quartiles);
            s["p"] = get_summary_object<uint32_t> (get_p (f.get_point_records ()), quartiles);
            s["i"] = get_summary_object<uint16_t> (get_i (f.get_point_records ()), quartiles);
            s["r"] = get_summary_object<uint16_t> (get_r (f.get_point_records ()), quartiles);
            s["g"] = get_summary_object<uint16_t> (get_g (f.get_point_records ()), quartiles);
            s["b"] = get_summary_object<uint16_t> (get_b (f.get_point_records ()), quartiles);
            json::array a;
            for (size_t k = 0; k < get_extra_fields_size (f.get_point_records ()); ++k)
                a.push_back (get_summary_object<uint64_t> (get_extra (k, f.get_point_records ()), quartiles));
            s["extra"] = a;
            j["summary"] = s;
        }

        if (classification_info)
        {
            json::object c;

            const auto class_count_map = get_class_count_map (get_c (f.get_point_records ()));

            for (auto i : class_count_map)
                c[i.first] = i.second;

            j["classifications"] = c;
        }

        if (metric_info)
        {
            const auto metric_value_map = get_metric_values (f.get_point_records ());

            json::object c;

            for (auto i : metric_value_map)
                c[i.first] = i.second;

            j["metrics"] = c;
        }

        if (compact)
            spoc::json::operator<<(os, j);
        else
            json::pretty_print (os, j, 0);
    }
    else
    {
        if (header_info)
        {
            os << "major_version\t" << int (f.get_header ().major_version) << endl;
            os << "minor_version\t" << int (f.get_header ().minor_version) << endl;
            os << "ogc_wkt\t'" << f.get_header ().wkt << "'" << endl;
            os << "total_points\t" << f.get_header ().total_points << endl;
            os << "compressed\t" << (f.get_header ().compressed ? "true" : "false") << endl;
        }

        if (summary_info)
        {
            os << get_summary_string<double> ("x\t", get_x (f.get_point_records ()), compact, quartiles);
            os << get_summary_string<double> ("y\t", get_y (f.get_point_records ()), compact, quartiles);
            os << get_summary_string<double> ("z\t", get_z (f.get_point_records ()), compact, quartiles);
            os << get_summary_string<uint32_t> ("c\t", get_c (f.get_point_records ()), compact, quartiles);
            os << get_summary_string<uint32_t> ("p\t", get_p (f.get_point_records ()), compact, quartiles);
            os << get_summary_string<uint16_t> ("i\t", get_i (f.get_point_records ()), compact, quartiles);
            os << get_summary_string<uint16_t> ("r\t", get_r (f.get_point_records ()), compact, quartiles);
            os << get_summary_string<uint16_t> ("g\t", get_g (f.get_point_records ()), compact, quartiles);
            os << get_summary_string<uint16_t> ("b\t", get_b (f.get_point_records ()), compact, quartiles);
            for (size_t k = 0; k < get_extra_fields_size (f.get_point_records ()); ++k)
            {
                stringstream s;
                s.precision (3);
                s << fixed;
                s << "extra_" << k << "\t";
                os << get_summary_string<uint64_t> (s.str (), get_extra (k, f.get_point_records ()), compact, quartiles);
            }
        }

        if (classification_info)
        {
            const auto class_count_map = get_class_count_map (get_c (f.get_point_records ()));
            const auto class_map = spoc::asprs::get_asprs_class_map ();

            for (auto i : class_count_map)
            {
                const auto j = std::stoi (i.first);
                const auto l = class_map.find (j) == class_map.end ()
                    ? "undefined"
                    : class_map.at (j);
                os << "cls_" << i.first
                    << "\t"
                    << i.second
                    << "\t"
                    << l
                    << std::endl;
            }
        }

        if (metric_info)
        {
            const auto metric_value_map = get_metric_values (f.get_point_records ());
            const auto metric_units_map = get_metric_units ();

            for (auto i : metric_value_map)
            {
                os << i.first
                    << "\t" << i.second
                    << "\t" << metric_units_map.at (i.first)
                    << std::endl;
            }
        }
    }
}

} // namespace info_app

} // namespace spoc
