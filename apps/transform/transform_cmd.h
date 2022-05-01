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
    char field_name = 'c';
    bool set_flag = false;
    double set_value = 0.0;
    std::vector<std::pair<unsigned,unsigned>> replace_pairs;
    std::string input_fn;
    std::string output_fn;
    std::string input_pipe_name;
    std::string output_pipe_name;
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
            {"field-name", required_argument, 0, 'f'},
            {"set", required_argument, 0, 's'},
            {"replace", required_argument, 0, 'r'},
            {"input-pipe-name", required_argument, 0, 'i'},
            {"output-pipe-name", required_argument, 0, 'o'},
            {0, 0, 0, 0}
        };

        int c = getopt_long(argc, argv, "hvf:s:r:i:o:", long_options, &option_index);
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
            case 'f': args.field_name = optarg[0]; break;
            case 's':
            {
                args.set_flag = true;
                args.set_value = std::atof (optarg);
                break;
            }
            case 'r':
            {
                // Put argument in a string for processing
                std::string s (optarg);
                size_t sz = 0;
                // Get first value
                const unsigned i1 = std::stoi (s, &sz);
                std::clog << "i1 = " << i1 << std::endl;
                s.erase (0, sz + 1);
                // Get second value
                const unsigned i2 = std::stoi (s);
                std::clog << "i2 = " << i2 << std::endl;
                // Save it
                args.replace_pairs.push_back (std::make_pair (i1, i2));
                break;
            }
            case 'i': args.input_pipe_name = std::string (optarg); break;
            case 'o': args.output_pipe_name = std::string (optarg); break;
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

} // namespace cmd

} // namespace spoc
