#include "cmd.h"
#include "test_utils.h"
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace spoc::cmd;

void test_cmd ()
{
    static struct option long_options[] = {
        {"help", no_argument, 0,  'h' },
        {"verbose", no_argument, 0,  'v' },
        {"tiles", required_argument, 0,  't' },
        {0,      0,           0,  0 }
    };

    stringstream ss;
    const size_t noptions = sizeof (long_options) / sizeof (struct option);
    const string usage ("usage");
    print_help (ss, usage, noptions, long_options);
    print_help (ss, usage, 0, long_options);
}

int main (int argc, char **argv)
{
    try
    {
        test_cmd ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
