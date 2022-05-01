#include "spoc_filter.h"

spoc_filter::point_record process (const spoc_filter::point_record &p)
{
    // Copy p...
    auto q (p);

    // ... and change it in some way...
    return q;
}

int main (int argc, char **argv)
{
    return filter_entry (argc, argv, process);
}
