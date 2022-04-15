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
    ja.push_back (ja);
    j["369"] = ja;

    json::object jo;
    jo["superstring1"] = -1;
    jo["superstring2"] = -2;
    ja.push_back (jo);
    j["xxx"] = ja;
    j["ss12"] = jo;

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
