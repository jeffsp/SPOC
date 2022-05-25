% SPOC_TRANSFORM(1) SPOC User's Manual | Version 0.1
% spoc@zetamon.xyz
% December 25, 2021

# NAME

spoc_transform - Transform point records in a spoc file

# USAGE

spoc_transform [*options*] [*input_filename*] [*output_filename*]

# DESCRIPTION

Transform point records in a spoc file.

# OPTIONS

--help, -h
:   Get help

--verbose, -v
:   Set verbose mode ON

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

# SEE ALSO

SPOC_FILTER(1)
