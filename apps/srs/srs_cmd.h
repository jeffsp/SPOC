#pragma once

#include "spoc/cmd.h"
#include <stdexcept>
#include <string>
#include <vector>

namespace spoc
{

namespace srs_cmd
{

struct args
{
    bool help = false;
    bool verbose = false;
    bool version = false;
    std::string srs;
    bool set_srs = false;
    std::vector<std::string> fns;
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
            {"srs", required_argument, 0, 's'},
            {0, 0, 0, 0}
        };

        int c = getopt_long(argc, argv, "hves:", long_options, &option_index);
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
            case 's':
            {
                args.srs = std::string (optarg);
                args.set_srs = true;
                break;
            }
        }
    }

    while (optind < argc)
        args.fns.push_back (argv[optind++]);

    return args;
}

} // namespace srs_cmd

} // namespace spoc
