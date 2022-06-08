#pragma once

#include <stdexcept>
#include <string>
#include <vector>
#include "cmd.h"

namespace spoc
{

namespace cmd
{

struct args
{
    bool help = false;
    bool verbose = false;
    bool version = false;
    bool header_only = false;
    bool data_only = false;
    std::vector<int> fields;
    bool reverse = false;
    std::string fn1;
    std::string fn2;
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
            {"header-only", no_argument, 0, 'a'},
            {"data-only", no_argument, 0, 'd'},
            {"field", required_argument, 0, 'f'},
            {"reverse", no_argument, 0, 'r'},
            {0, 0, 0, 0}
        };

        int c = getopt_long(argc, argv, "hveadf:r", long_options, &option_index);
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
            case 'e': args.version = true; break;
            case 'a': args.header_only = true; break;
            case 'd': args.data_only = true; break;
            case 'f':
            {
                std::string s = std::string (optarg);
                for (size_t i = 0; i < s.size (); ++i)
                    args.fields.push_back (s[i]);
                break;
            }
            case 'r': args.reverse = true; break;
        }
    }

    // Get input filenames
    if (optind == argc)
        throw std::runtime_error ("No input filename 1 was specified");

    args.fn1 = argv[optind++];

    if (optind == argc)
        throw std::runtime_error ("No input filename 2 was specified");

    args.fn2 = argv[optind++];

    // Check command line
    if (optind != argc)
        throw std::runtime_error ("Too many arguments on command line");

    return args;
}

} // namespace cmd

} // namespace spoc
