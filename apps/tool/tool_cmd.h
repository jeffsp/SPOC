#pragma once

#include "cmd.h"
#include <set>
#include <stdexcept>
#include <string>
#include <variant>

namespace spoc
{

namespace tool_cmd
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
    double resolution = 0.0;
    spoc::tool_cmd::command command;
    std::string field_fn;
    std::string input_fn;
    std::string output_fn;
};

enum command_values
{
    RECENTER_XY = 1000,
    RECENTER_XYZ,
    RESIZE_EXTRA,
    SUBTRACT_MIN_XY,
    SUBTRACT_MIN_XYZ,
    UPSAMPLE_CLASSIFICATIONS,
    RESOLUTION,
};

args set_command (const args &args, const std::string &name, const char *s)
{
    std::string params;
    if (s != nullptr)
        params = std::string (s);
    if (!args.command.name.empty ())
        throw std::runtime_error ("You can only specify one command at a time");
    spoc::tool_cmd::args new_args (args);
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
            {"get-field", required_argument, 0, 'g'},
            {"recenter-xy", no_argument, 0, RECENTER_XY},
            {"recenter-xyz", no_argument, 0, RECENTER_XYZ},
            {"resize-extra", required_argument, 0, RESIZE_EXTRA},
            {"set-field", required_argument, 0, 's'},
            {"field-filename", required_argument, 0, 'f'},
            {"subtract-min-xy", no_argument, 0, SUBTRACT_MIN_XY},
            {"subtract-min-xyz", no_argument, 0, SUBTRACT_MIN_XYZ},
            {"upsample-classifications", required_argument, 0, UPSAMPLE_CLASSIFICATIONS},
            {"resolution", required_argument, 0, RESOLUTION},
            {0, 0, 0, 0}
        };

        int c = getopt_long(argc, argv, "hveg:s:f:", long_options, &option_index);
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
            case 'g':
            {
                args = set_command (args, "get-field", optarg);
                break;
            }
            case RECENTER_XY:
            {
                args = set_command (args, "recenter-xy", optarg);
                break;
            }
            case RECENTER_XYZ:
            {
                args = set_command (args, "recenter-xyz", optarg);
                break;
            }
            case RESIZE_EXTRA:
            {
                args = set_command (args, "resize-extra", optarg);
                break;
            }
            case 's':
            {
                args = set_command (args, "set-field", optarg);
                break;
            }
            case 'f':
            {
                args.field_fn = optarg;
                break;
            }
            case SUBTRACT_MIN_XY:
            {
                args = set_command (args, "subtract-min-xy", optarg);
                break;
            }
            case SUBTRACT_MIN_XYZ:
            {
                args = set_command (args, "subtract-min-xyz", optarg);
                break;
            }
            case UPSAMPLE_CLASSIFICATIONS:
            {
                args = set_command (args, "upsample-classifications", optarg);
                break;
            }
            case RESOLUTION:
            {
                args.resolution = atof (optarg);
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

} // namespace tool_cmd

} // namespace spoc
