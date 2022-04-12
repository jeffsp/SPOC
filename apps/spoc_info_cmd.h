#pragma once

#include <getopt.h>
#include <stdexcept>
#include <string>
#include "cmd.h"

namespace spoc
{

namespace cmd
{

struct args
{
    bool help = false;
    bool verbose = false;
    bool json = true;
    bool header_info = true;
    bool summary_info = true;
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
            {0,      0,           0,  0 }
        };

        int c = getopt_long(argc, argv, "hvjes", long_options, &option_index);
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
        }
    }

    if (optind != argc)
        throw std::runtime_error ("Too many command line arguments were specified");

    return args;
}

} // namespace cmd

} // namespace spoc