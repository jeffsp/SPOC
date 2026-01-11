#pragma once

#include "cmd.h"
#include <stdexcept>
#include <string>

namespace spoc
{

namespace tile_cmd
{

struct args
{
    bool help = false;
    bool verbose = false;
    bool version = false;
    bool force = false;
    unsigned tiles = 2;
    unsigned digits = 3;
    double tile_size = -1;
    double tile_size_x = -1;
    double tile_size_y = -1;
    double target_tile_size = -1;
    std::string prefix;
    std::string fn;
};

inline args get_args (int argc, char **argv, const std::string &usage)
{
    args args;
    while (1)
    {
        int option_index = 0;
        static struct option long_options[] = {
            {"help", no_argument, 0, 'h'},
            {"verbose", no_argument, 0, 'v'},
            {"version", no_argument, 0, 'e'},
            {"force", no_argument, 0, 'f'},
            {"tiles", required_argument, 0, 't'},
            {"digits", required_argument, 0, 'd'},
            {"tile-size", required_argument, 0, 's'},
            {"tile-size-x", required_argument, 0, 'x'},
            {"tile-size-y", required_argument, 0, 'y'},
            {"target-tile-size", required_argument, 0, 'a'},
            {"prefix", required_argument, 0, 'p'},
            {0, 0, 0, 0}
        };

        int c = getopt_long(argc, argv, "hveft:d:s:x:y:p:a:", long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
            default:
            case 0:
            case 'h':
            {
                const size_t noptions = sizeof (long_options) / sizeof (struct option);
                spoc::cmd::print_help (std::clog, usage, noptions, long_options);
                if (c != 'h')
                    throw std::runtime_error ("Invalid option");
                args.help = true;
                return args;
            }
            case 'v': args.verbose = true; break;
            case 'e': args.version = true; break;
            case 'f': args.force = true; break;
            case 't': args.tiles = atoi (optarg); break;
            case 'd': args.digits = atoi (optarg); break;
            case 's': args.tile_size = atof (optarg); break;
            case 'x': args.tile_size_x = atof (optarg); break;
            case 'y': args.tile_size_y = atof (optarg); break;
            case 'p': args.prefix = std::string (optarg); break;
            case 'a': args.target_tile_size = atof (optarg); break;
        }
    }

    // Get input filename if specified
    if (optind != argc)
    {
        args.fn = argv[optind++];
        // Should have max of 1 input argument
        if (optind != argc)
            throw std::runtime_error ("Too many arguments on command line");
    }

    return args;
}

} // namespace tile_cmd

} // namespace spoc
