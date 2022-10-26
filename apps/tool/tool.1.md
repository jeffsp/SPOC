% SPOC\_TOOL(1) SPOC User's Manual | Version 0.1
% spoc@spocfile.xyz
% December 25, 2021

# NAME

spoc\_tool - SPOC file tool

# USAGE

spoc\_tool [*options*] [*input_filename*] [*output_filename*]

# DESCRIPTION

Collection of tools to run on a SPOC file

# OPTIONS

\-\-help, -h
:   Get help

\-\-verbose, -v
:   Set verbose mode ON

\-\-version, -e
:   Print version information and exit

\-\-random-seed=*#*, -r *#*
:   Set the random seed equal to *#*

\-\-resolution=*#*
:   Set the resolution used for upsampling

\-\-field-filename=*FN*
:   Set the filename of the text file used with the *set-field* command.
    This file (or named pipe) will contain newline separated values for
    the field being set.

# COMMANDS

\-\-get-field=*F*, -g *F*
:   Get point field *F* and write it to stdout as text. *F* can be one of
    'x', 'y', 'z', 'c', 'p', 'i', 'r', 'g', 'b', or 'e#', where the '#'
    after the 'e' specifies the extra field number.

\-\-recenter-xy
:   Recenter the point cloud by subtracting the mean X and Y value from
    each point's X and Y value

\-\-recenter-xyz
:   Same as recenter-xy, but also recenter the Z value

\-\-resize-extra=*#*
:   Change the number of extra fields in each point record to *#*

\-\-set-field=*F*, -s *F*
:   Set point field *F* by reading it from a file specified by the
    --field-filename option. *F* can be one of 'x', 'y', 'z', 'c', 'p',
    'i', 'r', 'g', 'b', or 'e#', where the '#' after the 'e' specifies
    the extra field number.

\-\-subtract-min-xy
:   Subtract minimum X and Y values from all X and Y values, thereby
    translating the point cloud's minimum to 0.0, 0.0.

\-\-subtract-min-xyz
:   Same as subtract-min-xy, but also subtract min from the Z value

\-\-upsample-classifications=*FN*
:   Upsample the classifications field from a low resolution SPOC file
    into the high resolution SPOC file specified by *FN* and write the
    result to the output file. The SPOC file being upsampled must have
    first been subsampled from *FN* using the *spoc_filter* application.
    The resolution should also be set using the *resolution* option, and
    this resolution should be the same resolution used for subsampling.

# SEE ALSO

SPOC\_FILTER(1)
SPOC\_TRANSFORM(1)
SPOC\_INFO(1)
