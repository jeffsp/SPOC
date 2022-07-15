#pragma once

#include "cmd.h"
#include <stdexcept>
#include <string>
#include <vector>

namespace spoc
{

namespace info_cmd
{

struct args
{
    bool help = false;
    bool verbose = false;
    bool version = false;
    bool json = false;
    bool header_info = true;
    bool summary_info = true;
    bool compact = true;
    bool classifications = true;
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
            {"json", no_argument, 0, 'j'},
            {"header-info", no_argument, 0, 'a'},
            {"summary-info", no_argument, 0, 's'},
            {"classifications", no_argument, 0, 'l'},
            {"compact", no_argument, 0, 'c'},
            {0, 0, 0, 0}
        };

        int c = getopt_long(argc, argv, "hvejaslc", long_options, &option_index);
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
            case 'j': args.json = !args.json; break;
            case 'a': args.header_info = !args.header_info; break;
            case 's': args.summary_info = !args.summary_info; break;
            case 'l': args.classifications = !args.classifications; break;
            case 'c': args.compact = !args.compact; break;
        }
    }

    while (optind < argc)
        args.fns.push_back (argv[optind++]);

    return args;
}

} // namespace info_cmd

} // namespace spoc
