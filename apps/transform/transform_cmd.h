#pragma once

#include <set>
#include <stdexcept>
#include <string>
#include <variant>
#include "cmd.h"

namespace spoc
{

namespace transform_cmd
{

struct command
{
    std::string name;
    std::string params;
};

struct args
{
    bool help = false;
    bool verbose = false;
    bool version = false;
    spoc::transform_cmd::command command;
    std::string input_fn;
    std::string output_fn;
};

enum command_values
{
    ADD_X = 1000,
    ADD_Y, // = 1001
    ADD_Z, // = 1002, ..., etc.
    QUANTIZE_XYZ,
    REPLACE,
    ROTATE_X,
    ROTATE_Y,
    ROTATE_Z,
    SCALE_X,
    SCALE_Y,
    SCALE_Z,
    SET,
};

args set_command (const args &args, const std::string &name, const char *s)
{
    std::string params;
    if (s != nullptr)
        params = std::string (s);
    if (!args.command.name.empty ())
        throw std::runtime_error ("You can only specify one command at a time");
    spoc::transform_cmd::args new_args (args);
    new_args.command.name = name;
    new_args.command.params = params;
    return new_args;
}

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
            {"add-x", required_argument, 0, ADD_X},
            {"add-y", required_argument, 0, ADD_Y},
            {"add-z", required_argument, 0, ADD_Z},
            {"quantize-xyz", required_argument, 0, QUANTIZE_XYZ},
            {"replace", required_argument, 0, REPLACE},
            {"rotate-x", required_argument, 0, ROTATE_X},
            {"rotate-y", required_argument, 0, ROTATE_Y},
            {"rotate-z", required_argument, 0, ROTATE_Z},
            {"scale-x", required_argument, 0, SCALE_X},
            {"scale-y", required_argument, 0, SCALE_Y},
            {"scale-z", required_argument, 0, SCALE_Z},
            {"set", required_argument, 0, SET},
            {0, 0, 0, 0}
        };

        int c = getopt_long(argc, argv, "hve", long_options, &option_index);
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
            case ADD_X:
            {
                args = set_command (args, "add-x", optarg);
                break;
            }
            case ADD_Y:
            {
                args = set_command (args, "add-y", optarg);
                break;
            }
            case ADD_Z:
            {
                args = set_command (args, "add-z", optarg);
                break;
            }
            case QUANTIZE_XYZ:
            {
                args = set_command (args, "quantize-xyz", optarg);
                break;
            }
            case REPLACE:
            {
                args = set_command (args, "replace", optarg);
                break;
            }
            case ROTATE_X:
            {
                args = set_command (args, "rotate-x", optarg);
                break;
            }
            case ROTATE_Y:
            {
                args = set_command (args, "rotate-y", optarg);
                break;
            }
            case ROTATE_Z:
            {
                args = set_command (args, "rotate-z", optarg);
                break;
            }
            case SCALE_X:
            {
                args = set_command (args, "scale-x", optarg);
                break;
            }
            case SCALE_Y:
            {
                args = set_command (args, "scale-y", optarg);
                break;
            }
            case SCALE_Z:
            {
                args = set_command (args, "scale-z", optarg);
                break;
            }
            case SET:
            {
                args = set_command (args, "set", optarg);
                break;
            }
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

} // namespace transform_cmd

} // namespace spoc
