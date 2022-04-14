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
using json = jobject;

void test_json ()
{
    json j;
    j.push_back (make_pair ("xyz", jvalue {12345}));
    j.push_back (make_pair ("hammy", jvalue { "asdf" }));
    j.push_back (make_pair ("blah", jvalue { 1.0f }));
    j.push_back (make_pair ("x", jvalue { 2.3 }));
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
    clog << "{" << endl;
    for (auto i : j)
    {
        clog << i.first << '\t';
        if (i.second.type () == typeid (int))
            clog << any_cast<int> (i.second );
        else if (i.second.type () == typeid (float))
            clog << any_cast<float> (i.second );
        else if (i.second.type () == typeid (bool))
            clog << boolalpha << any_cast<bool> (i.second );
        else if (i.second.type () == typeid (double))
            clog << any_cast<double> (i.second );
        else if (i.second.type () == typeid (string))
            clog << any_cast<string> (i.second );
        else if (i.second.type () == typeid (const char *))
            clog << any_cast<const char *> (i.second );
        else if (i.second.type () == typeid (jarray))
            clog << "jarray";
        else if (i.second.type () == typeid (jobject))
            clog << "jobject";
        else
            clog << "unknown type: " << i.second.type().name();
        clog << endl;
    }
    clog << "}" << endl;

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
