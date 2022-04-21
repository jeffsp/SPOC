#include "cmd.h"
#include "test_utils.h"
#include <iostream>
#include <stdexcept>

void test_cmd ()
{
    static struct option long_options[] = {
        {"help", no_argument, 0,  'h' },
        {"verbose", no_argument, 0,  'v' },
        {"blah", no_argument, 0,  'b' },
        {0,      0,           0,  0 }
    };

    std::stringstream ss;
    const size_t noptions = sizeof (long_options) / sizeof (struct option);
    const std::string usage ("usage");
    spoc::cmd::print_help (ss, usage, noptions, long_options);
}

int main (int argc, char **argv)
{
    using namespace std;
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
