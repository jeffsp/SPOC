% SPOC_TOOL(1) SPOC User's Manual | Version 0.1
% spoc@zetamon.xyz
% December 25, 2021

# NAME

spoc_tool - SPOC file tool

# USAGE

spoc_tool [*options*] [*input_filename*] [*output_filename*]

# DESCRIPTION

Collection of tools to run on a SPOC file

# OPTIONS

--help, -h
:   Get help

--verbose, -v
:   Set verbose mode ON

# COMMANDS

--set=*F*,*#*
:   Set the field *F* to *#*

--replace=*F*,*A*,*B*
:   Replace field *F* whose values are *A* to the value *B*

--recenter-xy
:   Recenter the point cloud by subtracting the mean X and Y value from
    each point's X and Y value

--recenter-xyz
:   Same as recenter-xy, but also recenter the Z value

--subtract-min-xy
:   Subtract minimum X and Y values from all X and Y values, thereby
    translating the point cloud's minimum to 0.0, 0.0.

--subtract-min-xyz
:   Same as subtract-min-xy, but also subtract min from the Z value

--quantize-xyz=*#*
:   Quantize X, Y, and Z values to the specified precision by truncating.
    For example, --quantize=0.1 will quantize X, Y, and Z values by
    truncating decimal digits past the tenths decimal place.

# SEE ALSO

SPOC_INFO(1)
