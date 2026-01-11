#pragma once

#include "cmd.h"
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

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
    std::vector<spoc::transform_cmd::command> commands;
    size_t random_seed = 0;
    std::string input_fn;
    std::string output_fn;
};

enum command_values
{
    ADD_X = 1000,
    ADD_Y, // = 1001
    ADD_Z, // = 1002, ..., etc.
    COPY_FIELD,
    GAUSSIAN_NOISE,
    GAUSSIAN_NOISE_X,
    GAUSSIAN_NOISE_Y,
    GAUSSIAN_NOISE_Z,
    QUANTIZE_XYZ,
    REPLACE,
    REPLACE_NOT,
    ROTATE_X,
    ROTATE_Y,
    ROTATE_Z,
    SCALE_X,
    SCALE_Y,
    SCALE_Z,
    SET,
    UNIFORM_NOISE,
    UNIFORM_NOISE_X,
    UNIFORM_NOISE_Y,
    UNIFORM_NOISE_Z,
};

command get_command (const std::string &name, const char *s)
{
    // std::string() expects a null-terminated string
    if (s != nullptr)
        return command {name, std::string (s)};
    else
        return command {name, std::string ()};
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
            {"copy-field", required_argument, 0, COPY_FIELD},
            {"gaussian-noise", required_argument, 0, GAUSSIAN_NOISE},
            {"gaussian-noise-x", required_argument, 0, GAUSSIAN_NOISE_X},
            {"gaussian-noise-y", required_argument, 0, GAUSSIAN_NOISE_Y},
            {"gaussian-noise-z", required_argument, 0, GAUSSIAN_NOISE_Z},
            {"quantize-xyz", required_argument, 0, QUANTIZE_XYZ},
            {"random-seed", required_argument, 0, 'a'},
            {"replace", required_argument, 0, REPLACE},
            {"replace-not", required_argument, 0, REPLACE_NOT},
            {"rotate-x", required_argument, 0, ROTATE_X},
            {"rotate-y", required_argument, 0, ROTATE_Y},
            {"rotate-z", required_argument, 0, ROTATE_Z},
            {"scale-x", required_argument, 0, SCALE_X},
            {"scale-y", required_argument, 0, SCALE_Y},
            {"scale-z", required_argument, 0, SCALE_Z},
            {"set", required_argument, 0, SET},
            {"uniform-noise", required_argument, 0, UNIFORM_NOISE},
            {"uniform-noise-x", required_argument, 0, UNIFORM_NOISE_X},
            {"uniform-noise-y", required_argument, 0, UNIFORM_NOISE_Y},
            {"uniform-noise-z", required_argument, 0, UNIFORM_NOISE_Z},
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
                args.commands.push_back (get_command ("add-x", optarg));
                break;
            }
            case ADD_Y:
            {
                args.commands.push_back (get_command ("add-y", optarg));
                break;
            }
            case ADD_Z:
            {
                args.commands.push_back (get_command ("add-z", optarg));
                break;
            }
            case COPY_FIELD:
            {
                args.commands.push_back (get_command ("copy-field", optarg));
                break;
            }
            case GAUSSIAN_NOISE:
            {
                args.commands.push_back (get_command ("gaussian-noise", optarg));
                break;
            }
            case GAUSSIAN_NOISE_X:
            {
                args.commands.push_back (get_command ("gaussian-noise-x", optarg));
                break;
            }
            case GAUSSIAN_NOISE_Y:
            {
                args.commands.push_back (get_command ("gaussian-noise-y", optarg));
                break;
            }
            case GAUSSIAN_NOISE_Z:
            {
                args.commands.push_back (get_command ("gaussian-noise-z", optarg));
                break;
            }
            case QUANTIZE_XYZ:
            {
                args.commands.push_back (get_command ("quantize-xyz", optarg));
                break;
            }
            case 'a':
            {
                args.random_seed = std::atol (optarg);
                break;
            }
            case REPLACE:
            {
                args.commands.push_back (get_command ("replace", optarg));
                break;
            }
            case REPLACE_NOT:
            {
                args.commands.push_back (get_command ("replace-not", optarg));
                break;
            }
            case ROTATE_X:
            {
                args.commands.push_back (get_command ("rotate-x", optarg));
                break;
            }
            case ROTATE_Y:
            {
                args.commands.push_back (get_command ("rotate-y", optarg));
                break;
            }
            case ROTATE_Z:
            {
                args.commands.push_back (get_command ("rotate-z", optarg));
                break;
            }
            case SCALE_X:
            {
                args.commands.push_back (get_command ("scale-x", optarg));
                break;
            }
            case SCALE_Y:
            {
                args.commands.push_back (get_command ("scale-y", optarg));
                break;
            }
            case SCALE_Z:
            {
                args.commands.push_back (get_command ("scale-z", optarg));
                break;
            }
            case SET:
            {
                args.commands.push_back (get_command ("set", optarg));
                break;
            }
            case UNIFORM_NOISE:
            {
                args.commands.push_back (get_command ("uniform-noise", optarg));
                break;
            }
            case UNIFORM_NOISE_X:
            {
                args.commands.push_back (get_command ("uniform-noise-x", optarg));
                break;
            }
            case UNIFORM_NOISE_Y:
            {
                args.commands.push_back (get_command ("uniform-noise-y", optarg));
                break;
            }
            case UNIFORM_NOISE_Z:
            {
                args.commands.push_back (get_command ("uniform-noise-z", optarg));
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
