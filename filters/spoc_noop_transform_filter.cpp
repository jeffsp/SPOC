#include "spoc_filter.h"

spoc_filter::point_record process (const spoc_filter::point_record &p)
{
    // Copy the point
    auto q (p);

    // This No-op filter just copies the input to the output.
    //
    // Typically, you would change 'q' here in some way before
    // returning it.
    return q;
}

int main (int argc, char **argv)
{
    return filter_entry (argc, argv, process);
}
