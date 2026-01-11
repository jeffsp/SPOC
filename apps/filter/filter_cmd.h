#pragma once

#include "cmd.h"
#include <set>
#include <stdexcept>
#include <string>
#include <variant>
#include <unordered_set>

namespace spoc
{

namespace filter_cmd
{

struct args
{
    bool help = false;
    bool verbose = false;
    bool version = false;
    std::unordered_set<int> keep_classes;
    size_t random_seed = 0;
    std::unordered_set<int> remove_classes;
    bool unique_xyz = false;
    double subsample = 0.0;
    std::string remove_coords;
    std::string input_fn;
    std::string output_fn;
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
            {"keep-class", required_argument, 0, 'k'},
            {"random-seed", required_argument, 0, 'a'},
            {"remove-class", required_argument, 0, 'r'},
            {"unique-xyz", no_argument, 0, 'u'},
            {"subsample", required_argument, 0, 's'},
            {"remove-coords", required_argument, 0, 'c'},
            {0, 0, 0, 0}
        };

        int c = getopt_long(argc, argv, "hvek:a:r:us:c:", long_options, &option_index);
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
            case 'v': { args.verbose = true; break; }
            case 'e': { args.version = true; break; }
            case 'k': { args.keep_classes.insert (std::atoi (optarg)); break; }
            case 'a': { args.random_seed = std::atol (optarg); break; }
            case 'r': { args.remove_classes.insert (std::atoi (optarg)); break; }
            case 'u': { args.unique_xyz = true; break; }
            case 's': { args.subsample = std::atof (optarg); break; }
            case 'c': { args.remove_coords = std::string (optarg); break; }
        }
    }

    // Get optional input filename
    if (optind < argc)
        args.input_fn = argv[optind++];

    // Get optional output filename
    if (optind < argc) // cppcheck-suppress duplicateCondition
        args.output_fn = argv[optind++];

    // Check command line
    if (optind != argc)
        throw std::runtime_error ("Too many arguments on command line");

    return args;
}

} // namespace filter_cmd

} // namespace spoc
