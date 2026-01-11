#ifndef CMD_H
#define CMD_H

#include <getopt.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace spoc_viewer
{

namespace cmd
{

void print_help (std::ostream &os, const std::string &usage, const size_t noptions, option long_options[])
{
    // Print usage string
    os << "Usage:" << std::endl << '\t' << usage << std::endl << std::endl;

    // Print associated options, if any
    if (noptions == 0)
        return;

    os << "Options:" << std::endl;

    for (size_t i = 0; i + 1 < noptions; ++i)
    {
        os << "\t--" << long_options[i].name;
        if (long_options[i].has_arg)
            os << "=<ARG>";
        os << ", ";
        os << "-" << char (long_options[i].val);
        if (long_options[i].has_arg)
            os << " <ARG>";
        os << std::endl;
    }
}

struct args
{
    bool help = false;
    bool verbose = false;
    float resolution = -1.0;
    std::vector<std::string> palette_filenames;
    std::vector<std::string> spoc_filenames;
    std::vector<std::string> color_modes;
    bool box_mode = false;
    std::string camera_coordinates;
    std::string screenshot_filename;
    size_t point_size = 5;
    float opacity = 1.0;
};

args get_args (int argc, char **argv, const std::string &usage)
{
    args args;
    while (1)
    {
        int option_index = 0;
        static struct option long_options[] = {
            {"help", no_argument, 0,  'h' },
            {"verbose", no_argument, 0,  'v' },
            {"palette", required_argument, 0,  'p' },
            {"resolution", required_argument, 0,  'r' },
            {"color-mode", required_argument, 0,  'c' },
            {"box-mode", no_argument, 0,  'b' },
            {"camera-coordinates", required_argument, 0,  'a' },
            {"screenshot-filename", required_argument, 0,  's' },
            {"opacity", required_argument, 0,  'o' },
            {"point-size", required_argument, 0,  'i' },
            {0,      0,           0,  0 }
        };

        int c = getopt_long(argc, argv, "hvp:r:c:ba:s:o:i:", long_options, &option_index);
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
            case 'p': args.palette_filenames.push_back (optarg); break;
            case 'r': args.resolution = std::atof (optarg); break;
            case 'c': args.color_modes.push_back (optarg); break;
            case 'b': args.box_mode = true; break;
            case 'a': args.camera_coordinates = optarg; break;
            case 's': args.screenshot_filename = optarg; break;
            case 'o': args.opacity = std::atof (optarg); break;
            case 'i': args.point_size = std::atoi (optarg); break;
        }
    }

    // Get input filename
    if (optind == argc)
        throw std::runtime_error ("No SPOC filename was specified");

    // Get the filenames
    while (optind < argc)
        args.spoc_filenames.push_back (argv[optind++]);

    return args;
}

} // namespace cmd

} // namespace spoc_viewer

#endif // CMD_H
