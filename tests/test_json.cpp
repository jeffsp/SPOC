#include "test_utils.h"
#include <iostream>
#include <any>
#include <memory>
#include <vector>
#include <stdexcept>

using namespace std;

// JSON Definition
using jvalue = std::any; // string, int, double, bool, jobject, jarray
using jobject = vector<pair<string,jvalue>>;
using jarray = vector<jvalue>;

inline std::ostream &operator<< (std::ostream &s, const jvalue &v);
inline std::ostream &operator<< (std::ostream &s, const jarray &ja)
{
    s << "[ ";
    auto i = ja.begin ();
    while (i != ja.end ())
    {
        s << *i++;
        if (i != ja.end ())
            s << ", ";
    }
    s << " ]";
    return s;
}

inline std::ostream &operator<< (std::ostream &s, const jobject &jo)
{
    s << "{" << endl;
    auto i = jo.begin ();
    while (i != jo.end ())
    {
        s << "'" << i->first << "'";
        s << " : ";
        s << i->second;
        ++i;
        if (i != jo.end ())
            s << ", ";
        s << endl;
    }
    s << "}";
    return s;
}

inline std::ostream &operator<< (std::ostream &s, const jvalue &v)
{
    if (v.type () == typeid (int))
        clog << any_cast<int> (v);
    else if (v.type () == typeid (float))
        clog << any_cast<float> (v);
    else if (v.type () == typeid (bool))
        clog << boolalpha << any_cast<bool> (v);
    else if (v.type () == typeid (double))
        clog << any_cast<double> (v);
    else if (v.type () == typeid (string))
        clog << "'" << any_cast<string> (v) << "'";
    else if (v.type () == typeid (const char *))
        clog << "'" << any_cast<const char *> (v) << "'";
    else if (v.type () == typeid (jarray))
        clog << any_cast<jarray> (v);
    else if (v.type () == typeid (jobject))
        clog << any_cast<jobject> (v);
    else
        clog << "unknown type: " << v.type().name();
    return s;
}

void test_json ()
{
    jobject j;
    j.push_back (make_pair (string ("xyz"), jvalue {12345}));
    j.push_back (make_pair ("hammy", jvalue { "asdf" }));
    j.push_back (make_pair ("blah", jvalue { 1.0f }));
    j.push_back (make_pair ("foo", jvalue { string ("bar") }));
    j.push_back (make_pair (string ("x"), jvalue { 2.3 }));
    j.push_back (make_pair ("y", jvalue { false }));

    jarray ja;
    ja.push_back (jvalue { 333 });
    ja.push_back (jvalue { "666" });
    ja.push_back (jvalue { 9.99 });
    j.push_back (make_pair ("369", jvalue { ja }));

    jobject jo;
    jo.push_back (make_pair ("superstring1", jvalue {-1}));
    jo.push_back (make_pair ("superstring2", jvalue {-2}));
    j.push_back (make_pair ("ss12", jvalue { jo }));
    clog << j << endl;

    verify (false);
}

int main (int argc, char **argv)
{
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
