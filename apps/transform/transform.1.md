% SPOC\_TRANSFORM(1) SPOC User's Manual | Version 0.1
% spoc@zetamon.xyz
% December 25, 2021

# NAME

spoc\_transform - SPOC file tranform

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

# COMMANDS

\-\-quantize-xyz=*#*
:   Quantize X, Y, and Z values to the specified precision by truncating.
    For example, --quantize=0.1 will quantize X, Y, and Z values by
    truncating decimal digits past the tenths decimal place.

\-\-replace=*F*,*A*,*B*
:   Replace field *F* whose values are *A* to the value *B*. *F* can be
    one of 'x', 'y', 'z', 'c', 'p', 'i', 'r', 'g', 'b', or 'e#', where the
    '#' after the 'e' specifies the extra field number.

\-\-set=*F*,*#*
:   Set the field *F* to *#*. *F* can be one of 'x', 'y', 'z', 'c', 'p',
    'i', 'r', 'g', 'b', or 'e#', where the '#' after the 'e' specifies
    the extra field number.

# SEE ALSO

SPOC\_INFO(1)
SPOC\_TOOL(1)
