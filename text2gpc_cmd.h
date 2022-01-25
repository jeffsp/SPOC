#pragma once

#include <getopt.h>
#include <stdexcept>
#include <string>
#include "cmd.h"

namespace gpc
{

namespace cmd
{

struct args
{
    bool help = false;
    bool verbose = false;
    bool sort = false;
    double sort_grid_resolution = 10.0;
};

args get_args (int argc, char **argv, const std::string &usage)
{
    args args;
    while (1)
    {
        int option_index = 0;
        static struct option long_options[] = {
            {"help", no_argument, 0,  'h' },
            {"verbose", no_argument, 0,  'v' },
            {"sort", required_argument, 0,  's' },
            {"sort_grid_resolution", required_argument, 0,  'g' },
            {0,      0,           0,  0 }
        };

        int c = getopt_long(argc, argv, "hvsg:", long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
            default:
            case 0:
            case 'h':
            {
                const size_t noptions = sizeof (long_options) / sizeof (struct option);
                print_help (std::clog, usage, noptions, long_options);
                if (c != 'h')
                    throw std::runtime_error ("Invalid option");
                args.help = true;
                return args;
            }
            case 'v': args.verbose = true; break;
            case 's': args.sort = true; break;
            case 'g': args.sort_grid_resolution = atof (optarg); break;
        }
    }

    // Check command line
    if (optind != argc)
        throw std::runtime_error ("Too many command line arguments were specified");

    return args;
}

} // namespace cmd

} // namespace gpc
