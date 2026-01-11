#pragma once

#include <any>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <map>
#include <vector>

namespace spoc
{

namespace json
{

// JSON Definition
//
// json::value can be any of:
//      int,
//      char,
//      uint8_t,
//      uint16_t,
//      uint64_t,
//      float,
//      double,
//      bool,
//      string,
//      const char *,
//      json::object,
//      json::array
using value = std::any;

// json::object maps strings to values
using object = std::map<std::string,value>;

// json::array is a vector of json::values
using array = std::vector<value>;

inline void print_indents (std::ostream &s, const int indents)
{
    for (auto i = 0; i < indents; ++i)
        s << "    ";
}

inline void pretty_print (std::ostream &s, const json::value &v, const int indents);
inline void pretty_print (std::ostream &s, const json::array &a, const int indents)
{
    s << "[ ";
    auto i = a.begin ();
    while (i != a.end ())
    {
        pretty_print (s, *i++, indents);
        if (i != a.end ())
            s << ", ";
    }
    s << " ]";
}

inline void pretty_print (std::ostream &s, const json::object &o, const int indents)
{
    s << "{" << std::endl;
    auto i = o.begin ();
    while (i != o.end ())
    {
        print_indents (s, indents + 1);
        s << "'" << i->first << "'";
        s << " : ";
        pretty_print (s, i->second, indents + 1);
        ++i;
        if (i != o.end ())
            s << ",";
        s << std::endl;
    }
    print_indents (s, indents);
    s << "}";
}

inline void pretty_print (std::ostream &s, const json::value &v, const int indents)
{
    if (v.type () == typeid (int))
    {
        s << std::any_cast<int> (v);
    }
    else if (v.type () == typeid (char))
    {
        s << int (std::any_cast<char> (v));
    }
    else if (v.type () == typeid (uint8_t))
    {
        s << int (std::any_cast<uint8_t> (v));
    }
    else if (v.type () == typeid (uint16_t))
    {
        s << std::any_cast<uint16_t> (v);
    }
    else if (v.type () == typeid (uint64_t))
    {
        s << std::any_cast<uint64_t> (v);
    }
    else if (v.type () == typeid (float))
    {
        s << std::any_cast<float> (v);
    }
    else if (v.type () == typeid (bool))
    {
        s << std::boolalpha;
        s << std::any_cast<bool> (v);
    }
    else if (v.type () == typeid (double))
    {
        s << std::any_cast<double> (v);
    }
    else if (v.type () == typeid (std::string))
    {
        s << "'";
        s << std::any_cast<std::string> (v);
        s << "'";
    }
    else if (v.type () == typeid (const char *))
    {
        s << "'";
        s << std::any_cast<const char *> (v);
        s << "'";
    }
    else if (v.type () == typeid (json::array))
        pretty_print (s, std::any_cast<json::array> (v), indents);
    else if (v.type () == typeid (json::object))
        pretty_print (s, std::any_cast<json::object> (v), indents);
    else
        s << "unknown type: " << v.type().name();
}

inline std::ostream &operator<< (std::ostream &s, const json::value &v);
inline std::ostream &operator<< (std::ostream &s, const json::array &a)
{
    s << "[ ";
    auto i = a.begin ();
    while (i != a.end ())
    {
        s << *i++;
        if (i != a.end ())
            s << ", ";
    }
    s << " ]";
    return s;
}

inline std::ostream &operator<< (std::ostream &s, const json::object &o)
{
    s << "{ ";
    auto i = o.begin ();
    while (i != o.end ())
    {
        s << "'" << i->first << "'";
        s << " : ";
        s << i->second;
        ++i;
        if (i != o.end ())
            s << ", ";
    }
    s << " }";
    return s;
}

inline std::ostream &operator<< (std::ostream &s, const json::value &v)
{
    if (v.type () == typeid (int))
        s << std::any_cast<int> (v);
    else if (v.type () == typeid (char))
        s << int (std::any_cast<char> (v));
    else if (v.type () == typeid (uint8_t))
        s << int (std::any_cast<uint8_t> (v));
    else if (v.type () == typeid (uint16_t))
        s << std::any_cast<uint16_t> (v);
    else if (v.type () == typeid (uint64_t))
        s << std::any_cast<uint64_t> (v);
    else if (v.type () == typeid (float))
        s << std::any_cast<float> (v);
    else if (v.type () == typeid (bool))
        s << std::boolalpha << std::any_cast<bool> (v);
    else if (v.type () == typeid (double))
        s << std::any_cast<double> (v);
    else if (v.type () == typeid (std::string))
        s << "'" << std::any_cast<std::string> (v) << "'";
    else if (v.type () == typeid (const char *))
        s << "'" << std::any_cast<const char *> (v) << "'";
    else if (v.type () == typeid (json::array))
        s << std::any_cast<json::array> (v);
    else if (v.type () == typeid (json::object))
        s << std::any_cast<json::object> (v);
    else
        s << "unknown type: " << v.type().name();
    return s;
}

} // namespace json

}
