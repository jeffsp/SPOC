#include "test_utils.h"
#include <any>
#include <iostream>
#include <map>
#include <memory>
#include <vector>
#include <stdexcept>

namespace json
{

// JSON Definition
//
// json::value can be any of:
//      string,
//      const char *,
//      int,
//      float,
//      double,
//      bool,
//      json::object,
//      json::array
using value = std::any;

// json::object maps strings to values
using object = std::map<std::string,value>;

// json::array is a vector of json::values
using array = std::vector<value>;

} // namespace json

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
            s << ", ";
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
        std::clog << std::any_cast<int> (v);
    else if (v.type () == typeid (float))
        std::clog << std::any_cast<float> (v);
    else if (v.type () == typeid (bool))
        std::clog << std::boolalpha << std::any_cast<bool> (v);
    else if (v.type () == typeid (double))
        std::clog << std::any_cast<double> (v);
    else if (v.type () == typeid (std::string))
        std::clog << "'" << std::any_cast<std::string> (v) << "'";
    else if (v.type () == typeid (const char *))
        std::clog << "'" << std::any_cast<const char *> (v) << "'";
    else if (v.type () == typeid (json::array))
        std::clog << std::any_cast<json::array> (v);
    else if (v.type () == typeid (json::object))
        std::clog << std::any_cast<json::object> (v);
    else
        std::clog << "unknown type: " << v.type().name();
    return s;
}

void test_json ()
{
    json::object j;
    j[std::string ("xyz")] = 12345;
    j["hammy"] = "asdf";
    j["blah"] = 1.0f;
    j["foo"] = std::string ("bar");
    j[std::string ("x")] = 2.3;
    j["y"] = false;

    json::array ja;
    ja.push_back (333);
    ja.push_back ("666");
    ja[1] = "66666";
    ja[1] = 66666;
    ja[1] = 666.66;
    ja.push_back (9.99);
    ja.push_back (ja);
    j["369"] = ja;

    json::object jo;
    jo["superstring1"] = -1;
    jo["superstring2"] = -2;
    ja.push_back (jo);
    j["xxx"] = ja;
    j["ss12"] = jo;

    json::object header;
    header["major_version"] = 1;
    header["minor_version"] = 4;
    json::object summary;
    summary["x"] = 100.1;
    summary["y"] = 100.1;
    json::array a;
    a.push_back (1);
    a.push_back (2);
    a.push_back (3);
    summary["list"] = a;
    json::object jj;
    jj["header"] = header;
    jj["summary"] = summary;

    std::clog << "j " << std::endl;
    std::clog << j << std::endl;
    std::clog << "jj " << std::endl;
    std::clog << jj << std::endl;

    std::clog << "j " << std::endl;
    pretty_print (std::clog, j, 0);
    std::clog << std::endl;
    std::clog << "jj " << std::endl;
    pretty_print (std::clog, jj, 0);
    std::clog << std::endl;
    verify (false);
}

int main (int argc, char **argv)
{
    using namespace std;
    try
    {
        test_json ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
