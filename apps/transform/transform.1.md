% SPOC\_TRANSFORM(1) SPOC User's Manual | Version 0.1
% spoc@zetamon.xyz
% December 25, 2021

# NAME

spoc\_transform - SPOC file transform

# USAGE

spoc\_transform [*options*] [*input_filename*] [*output_filename*]

# DESCRIPTION

Collection of transformations to run on a SPOC file

# OPTIONS

\-\-help, -h
:   Get help

\-\-verbose, -v
:   Set verbose mode ON

\-\-version, -e
:   Print version information and exit

\-\-random-seed=*#*, -a *#*
:   Set the random seed. This seed will determine the random values
    generated for operations like adding noise.

# COMMANDS

\-\-add-x=*#*
:   Add an offset to the X location of all points

\-\-add-y=*#*
:   Add an offset to the Y location of all points

\-\-add-z=*#*
:   Add an offset to the Z location of all points

\-\-copy-field=*F1*,*F2*
:   Copy all values in field *F1* to the field *F2*. *F1* and *F2* may be
    one of 'c', 'p', 'i', 'r', 'g', 'b', or 'e#', where the '#' after
    the 'e' specifies the extra field number.

\-\-gaussian-noise=*#*
:   Add Gaussian noise to each X, Y, and Z value with a standard
    deviation of *#*

\-\-gaussian-noise-x=*#*
:   Add Gaussian noise to each X value with a standard deviation of *#*

\-\-gaussian-noise-y=*#*
:   Add Gaussian noise to each Y value with a standard deviation of *#*

\-\-gaussian-noise-z=*#*
:   Add Gaussian noise to each Z value with a standard deviation of *#*

\-\-quantize-xyz=*#*
:   Quantize X, Y, and Z values to the specified precision by truncating.
    For example, --quantize=0.1 will quantize X, Y, and Z values by
    truncating decimal digits past the tenths decimal place.

\-\-replace=*F*,*A*,*B*
:   Replace field *F* whose values are *A* to the value *B*. *F* can be
    one of 'x', 'y', 'z', 'c', 'p', 'i', 'r', 'g', 'b', or 'e#', where the
    '#' after the 'e' specifies the extra field number.

\-\-rotate-x=*#*
:   Rotate points about the X axis by *#* degrees.

\-\-rotate-y=*#*
:   Rotate points about the Y axis by *#* degrees.

\-\-rotate-z=*#*
:   Rotate points about the Z axis by *#* degrees.

\-\-scale-x=*#*
:   Scale the X value of all points by a constant factor

\-\-scale-y=*#*
:   Scale the Y value of all points by a constant factor

\-\-scale-z=*#*
:   Scale the Z value of all points by a constant factor

\-\-set=*F*,*#*
:   Set the field *F* to *#*. *F* can be one of 'x', 'y', 'z', 'c', 'p',
    'i', 'r', 'g', 'b', or 'e#', where the '#' after the 'e' specifies
    the extra field number.

\-\-uniform-noise=*#*
:   Add uniform noise to each X, Y, and Z value of magnitude +/-*#*

\-\-uniform-noise-x=*#*
:   Add uniform noise to each X value of magnitude +/-*#*

\-\-uniform-noise-y=*#*
:   Add uniform noise to each Y value of magnitude +/-*#*

\-\-uniform-noise-z=*#*
:   Add uniform noise to each Z value of magnitude +/-*#*

# SEE ALSO

SPOC\_INFO(1)
SPOC\_TOOL(1)
