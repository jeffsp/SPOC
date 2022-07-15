#include "asprs.h"
#include "test_utils.h"

using namespace std;
using namespace spoc::asprs;

void test_classes ()
{
    const auto c = get_asprs_class_map ();
    verify (c.at (static_cast<int> (point_class::building)) == string ("building"));
}

int main (int argc, char **argv)
{
    try
    {
        test_classes ();

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
