% SPOC_COMPRESS(1) SPOC User's Manual | Version 0.1
% spoc@zetamon.xyz
% December 25, 2021

# NAME

spoc_compress - Compress a spoc file

# USAGE

spoc_compress [*options*] [*input_filename*] [*output_filename*]

# DESCRIPTION

Compress a spoc file.

# OPTIONS

\-\-help, -h
:   Get help

\-\-verbose, -v
:   Set verbose mode ON

\-\-version, -e
:   Print version information and exit

\-\-precision=*#*, -p *#*
:   The number of precision bits in significand of the IEEE 754 double
    precision floating point formatted X, Y and Z values. This is an
    integer value ranging from 1 to 51. Smaller values will improve
    compression but will reduce precision.

# SEE ALSO

SPOC_DECOMPRESS(1)
