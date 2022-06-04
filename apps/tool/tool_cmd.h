#pragma once

#include <set>
#include <stdexcept>
#include <string>
#include <variant>
#include "cmd.h"

namespace spoc
{

namespace cmd
{

struct set_command { char f; double v; };
struct replace_command { char f; int v1; int v2; };
struct recenter_xy_command { };
struct recenter_xyz_command { };
struct subtract_min_xy_command { };
struct subtract_min_xyz_command { };

using command = std::variant<
    set_command,
    replace_command,
    recenter_xy_command,
    recenter_xyz_command,
    subtract_min_xy_command,
    subtract_min_xyz_command
    >;

struct args
{
    bool help = false;
    bool verbose = false;
    std::vector<command> commands;
    std::string input_fn;
    std::string output_fn;
};

enum command_values
{
    SET = 1000,
    REPLACE, // = 1001
    RECENTER_XY, // = 1002
    RECENTER_XYZ, // ...
    SUBTRACT_MIN_XY,
    SUBTRACT_MIN_XYZ,
};

const std::set<char> field_chars {'x', 'y', 'z', 'c', 'p', 'i', 'r', 'g', 'b'};

bool check_field (const char f)
{
    if (field_chars.find (f) == field_chars.end ())
        return false;
    return true;
}

char consume_field (std::string &s)
{
    // Get the field
    const char c = s[0];
    // Check it
    if (!check_field (c))
        throw std::runtime_error (std::string ("Invalid field name: ") + s);
    // Make sure the next char is a ','
    if (!check_field (c))
        throw std::runtime_error (std::string ("Invalid field specification: ") + s);
    s.erase (0, 2);
    return c;
}

int consume_int (std::string &s)
{
    size_t sz = 0;
    int v = 0.0;
    try { v = std::stoi (s, &sz); }
    catch (const std::invalid_argument &e) {
        throw std::runtime_error (std::string ("Could not parse field string: ") + s);
    }
    s.erase (0, sz + 1);
    return v;
}

double consume_double (std::string &s)
{
    size_t sz = 0;
    double v = 0.0;
    try { v = std::stod (s, &sz); }
    catch (const std::invalid_argument &e) {
        throw std::runtime_error (std::string ("Could not parse field string: ") + s);
    }
    s.erase (0, sz + 1);
    return v;
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
            {"set", required_argument, 0, SET},
            {"replace", required_argument, 0, REPLACE},
            {"recenter-xy", no_argument, 0, RECENTER_XY},
            {"recenter-xyz", no_argument, 0, RECENTER_XYZ},
            {"subtract-min-xy", no_argument, 0, SUBTRACT_MIN_XY},
            {"subtract-min-xyz", no_argument, 0, SUBTRACT_MIN_XYZ},
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
            case 'v':
            {
                args.verbose = true;
                break;
            }
            case SET:
            {
                std::string s (optarg);
                set_command cmd;
                cmd.f = consume_field (s);
                cmd.v = consume_double (s);
                args.commands.push_back (cmd);
                break;
            }
            case REPLACE:
            {
                std::string s (optarg);
                replace_command cmd;
                cmd.f = consume_field (s);
                cmd.v1 = consume_int (s);
                cmd.v2 = consume_int (s);
                args.commands.push_back (cmd);
                break;
            }
            case RECENTER_XY:
            {
                args.commands.push_back (recenter_xy_command ());
                break;
            }
            case RECENTER_XYZ:
            {
                args.commands.push_back (recenter_xyz_command ());
                break;
            }
            case SUBTRACT_MIN_XY:
            {
                args.commands.push_back (subtract_min_xy_command ());
                break;
            }
            case SUBTRACT_MIN_XYZ:
            {
                args.commands.push_back (subtract_min_xyz_command ());
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

} // namespace cmd

} // namespace spoc
