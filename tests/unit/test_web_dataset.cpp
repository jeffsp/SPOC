#include "test_utils.h"
#include "web_dataset.h"
#include <print>
#include <stdexcept>

using namespace std;
using namespace spoc::test_utils;
using namespace spoc::web_dataset;

void test_wds0 ()
{
    // I/O buffer
    stringstream ss;

    // Data files
    string json {R"({"foo": "bar", "baz": 123.456})"};
    vector<char> image (512, 0xFF);

    vector<WebDSFile> files
    {
        {"0001.json", vector<char> (json.begin (), json.end ())},
        {"0001.jpg", image},
    };

    // Write it out
    {
        WebDSWriter writer (ss);
        for (const auto &f : files)
            writer.write (f.name, f.data);
    }

    // Data gets flushed when the writer goes out of scope
    VERIFY (ss.str().size() > 0);

    // Read it back
    WebDSReader reader (ss);

    size_t n = 0;
    while (auto file = reader.next())
    {
        //println("[{}]-> {}, {} bytes", n, file->name, file->data.size());
        VERIFY (file->name == files[n].name);
        VERIFY (file->data == files[n].data);
        ++n;
    }
}

int main ()
{
    try
    {
        test_wds0 ();

        return 0;
    }
    catch (const exception &e)
    {
        println (stderr, "Exception: {}", e.what());
        return -1;
    }
}
