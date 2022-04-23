#pragma once

#include "json.h"
#include "spoc.h"
#include <iostream>
#include <map>
#include <sstream>
#include <unordered_map>

namespace spoc
{

namespace info
{

template<typename T>
std::map<std::string,size_t> get_class_map (const T &c)
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
        a[std::to_string (i.first)] = i.second;

    return a;
}

template<typename T,typename U>
inline spoc::json::object get_summary_object (const U &x)
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

    // Copy
    auto y (x);

    // Get Quantiles
    const size_t n = y.size ();
    std::sort (begin (y), end (y));
    s["range"] = y.back() - y.front();
    s["q0"] = y.front();
    s["q1"] = y[n / 4];
    s["q2"] = y[n / 2];
    s["q3"] = y[3 * n / 4];
    s["q4"] = y.back();
    return s;
}

template<typename T,typename U>
inline std::string get_summary_string (const std::string &label, const U &x, const bool compact)
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

    // Copy
    auto y (x);

    // Get Quantiles
    const size_t n = y.size ();
    std::sort (begin (y), end (y));
    if (compact)
    {
        s << label;
        // Range
        s << "range=" << y.back() - y.front();
        // q0
        s << ", " << y.front();
        // Step to q1
        s << " +" << y[n / 4] - y.front ();
        // Step to q2
        s << " +" << y[n / 2] - y[n / 4];
        // Step to q3
        s << " +" << y[3 * n / 4] - y[n / 2];
        // Step to q4
        s << " +" << y.back () - y[3 * n / 4];
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
    return s.str ();
}

// Process a spoc file and write to stdout
void process (std::ostream &os,
    const spoc::spoc_file &f,
    const bool json,
    const bool header_info,
    const bool summary_info,
    const bool classifications,
    const bool compact)
{
    using namespace std;
    using namespace spoc;

    os.precision (15);
    os << fixed;

    if (json)
    {
        json::object j;

        if (header_info)
        {
            json::object h;
            h["major_version"] = f.get_major_version ();
            h["minor_version"] = f.get_minor_version ();
            h["wkt"] = f.get_wkt ();
            h["npoints"] = f.get_npoints ();
            j["header"] = h;
        }

        if (summary_info)
        {
            json::object s;

            s["x"] = get_summary_object<double> (f.get_x ());
            s["y"] = get_summary_object<double> (f.get_y ());
            s["z"] = get_summary_object<double> (f.get_z ());
            s["c"] = get_summary_object<uint16_t> (f.get_c ());
            s["p"] = get_summary_object<uint16_t> (f.get_p ());
            s["i"] = get_summary_object<uint16_t> (f.get_i ());
            s["r"] = get_summary_object<uint16_t> (f.get_r ());
            s["g"] = get_summary_object<uint16_t> (f.get_g ());
            s["b"] = get_summary_object<uint16_t> (f.get_b ());
            json::array a;
            for (size_t k = 0; k < f.get_extra ().size (); ++k)
                a.push_back (get_summary_object<uint64_t> (f.get_extra ()[k]));
            s["extra"] = a;
            j["summary"] = s;
        }

        if (classifications)
        {
            json::object c;

            const auto cls_map = get_class_map (f.get_c ());

            for (auto i : cls_map)
                c[i.first] = i.second;

            j["classifications"] = c;
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
            os << "major_version\t" << int (f.get_major_version ()) << endl;
            os << "minor_version\t" << int (f.get_minor_version ()) << endl;
            os << "wkt\t" << f.get_wkt () << endl;
            os << "npoints\t" << f.get_npoints () << endl;
        }

        if (summary_info)
        {
            os << get_summary_string<double> ("x\t", f.get_x (), compact);
            os << get_summary_string<double> ("y\t", f.get_y (), compact);
            os << get_summary_string<double> ("z\t", f.get_z (), compact);
            os << get_summary_string<uint16_t> ("c\t", f.get_c (), compact);
            os << get_summary_string<uint16_t> ("p\t", f.get_p (), compact);
            os << get_summary_string<uint16_t> ("i\t", f.get_i (), compact);
            os << get_summary_string<uint16_t> ("r\t", f.get_r (), compact);
            os << get_summary_string<uint16_t> ("g\t", f.get_g (), compact);
            os << get_summary_string<uint16_t> ("b\t", f.get_b (), compact);
            for (size_t k = 0; k < f.get_extra ().size (); ++k)
            {
                stringstream s;
                s.precision (3);
                s << fixed;
                s << "extra " << k << "\t";
                os << get_summary_string<uint16_t> (s.str (), f.get_extra ()[k], compact);
            }
        }

        if (classifications)
        {
            const auto cls_map = get_class_map (f.get_c ());

            for (auto i : cls_map)
                os << "cls_" << i.first << "\t" << i.second << std::endl;
        }
    }
}

} // namespace info

} // namespace spoc