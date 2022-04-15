#include "test_utils.h"
#include <any>
#include <iostream>
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

// json::object is a vector of string,json::value pairs
//
// vectors are used because we want to preserve the order
using object = std::vector<std::pair<std::string,value>>;

// json::array is a vector of json::values
//
// vectors are used because we want to preserve the order
using array = std::vector<value>;

struct json
{
    json ()
    {
    }
    ::json::object root;
    /*
    ::json::object &operator[] (const std::string &key)
    {
    }
    */
};

} // namespace json

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
    s << "{" << std::endl;
    auto i = o.begin ();
    while (i != o.end ())
    {
        s << "'" << i->first << "'";
        s << " : ";
        s << i->second;
        ++i;
        if (i != o.end ())
            s << ", ";
        s << std::endl;
    }
    s << "}";
    return s;
}

inline std::ostream &operator<< (std::ostream &s, const json::value &v)
{
    if (v.type () == typeid (int))
        std::clog << std::any_cast<int> (v);
    else if (v.type () == typeid (float))
        std::clog << std::any_cast<float> (v);
    else if (v.type () == typeid (bool))
        std::clog << std::boolalpha << any_cast<bool> (v);
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
    j.push_back (make_pair (std::string ("xyz"), json::value {12345}));
    j.push_back (make_pair ("hammy", json::value { "asdf" }));
    j.push_back (make_pair ("blah", json::value { 1.0f }));
    j.push_back (make_pair ("foo", json::value { std::string ("bar") }));
    j.push_back (make_pair (std::string ("x"), json::value { 2.3 }));
    j.push_back (make_pair ("y", json::value { false }));

    json::array ja;
    ja.push_back (json::value { 333 });
    ja.push_back (json::value { "666" });
    ja[1] = "66666";
    ja[1] = 66666;
    ja[1] = 666.66;
    ja.push_back (json::value { 9.99 });
    ja.push_back (json::value { ja });
    ja.push_back (json::value { ja });
    j.push_back (make_pair ("369", json::value { ja }));

    json::object jo;
    jo.push_back (make_pair ("superstring1", json::value {-1}));
    jo.push_back (make_pair ("superstring2", json::value {-2}));
    jo[0].second = -3;
    ja.push_back (json::value { jo });
    j.push_back (make_pair ("xxx", json::value { ja }));
    j.push_back (make_pair ("ss12", json::value { jo }));
    std::clog << j << std::endl;

    json::json jj;
    //jj.add ("xyz", 12345);
    //j["header"]["major_version"] = 4;
    //j["summary"]["x"]["range"] = 100.1;
    //j["summary"]["x"]["min"][0] = 100.1;
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
