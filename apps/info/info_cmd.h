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
            {"help", no_argument, 0,  'h' },
            {"verbose", no_argument, 0,  'v' },
            {"json", no_argument, 0,  'j' },
            {"header-info", no_argument, 0,  'e' },
            {"summary-info", no_argument, 0,  's' },
            {"classifications", no_argument, 0,  'l' },
            {"compact", no_argument, 0,  'c' },
            {0,      0,           0,  0 }
        };

        int c = getopt_long(argc, argv, "hvjeslc", long_options, &option_index);
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
            case 'j': args.json = !args.json; break;
            case 'e': args.header_info = !args.header_info; break;
            case 's': args.summary_info = !args.summary_info; break;
            case 'l': args.classifications = !args.classifications; break;
            case 'c': args.compact = !args.compact; break;
        }
    }

    while (optind < argc)
        args.fns.push_back (argv[optind++]);

    return args;
}

} // namespace cmd

} // namespace spoc
