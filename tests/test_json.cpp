#include "json.h"
#include "test_utils.h"
#include <any>
#include <iostream>
#include <map>
#include <vector>
#include <stdexcept>

void test_json ()
{
    using namespace spoc;
    using namespace json;
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

    std::stringstream ss;
    ss << "j " << std::endl;
    ss << j << std::endl;
    ss << "jj " << std::endl;
    ss << jj << std::endl;

    ss << "j " << std::endl;
    pretty_print (ss, j, 0);
    ss << std::endl;
    ss << "jj " << std::endl;
    pretty_print (ss, jj, 0);
    ss << std::endl;
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
