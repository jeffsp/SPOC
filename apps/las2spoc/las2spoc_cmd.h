#pragma once

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
    std::string input_fn;
    std::string output_fn;
};

args get_args (int argc, char **argv, const std::string &usage)
{
    args args;
    while (1)
    {
        int option_index = 0;
        static struct option long_options[] = {
            {"help", no_argument, 0, 'h'},
            {"verbose", no_argument, 0, 'v'},
            {0, 0, 0, 0}
        };

        int c = getopt_long(argc, argv, "hv", long_options, &option_index);
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
        }
    }

    // Get remaining args
    if (optind == argc)
        throw std::runtime_error ("An input file was not specified");

    args.input_fn = argv[optind++];

    if (optind == argc)
        throw std::runtime_error ("An output file was not specified");

    args.output_fn = argv[optind++];

    if (optind != argc)
        throw std::runtime_error ("Too many command line arguments were specified");

    return args;
}

} // namespace cmd

} // namespace spoc
