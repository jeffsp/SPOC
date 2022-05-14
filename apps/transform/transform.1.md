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

--field=*F*, -f *F*
:   Set the field to transform. May be one of 'x', 'y', 'z', 'c', 'p',
    'i', 'r', 'g', 'b', '0', '1', ..., '7'. The default is 'c'.

--set=*#* -s *#*
    Set the selected field's values to *#*.

--replace=*A*,*B* -r *A*,*B*
    Replace the selected field whose value is *A* to the value *B*. You
    can specify this option multiple times. They will be applied in the
    order they occur on the command line.

# SEE ALSO

SPOC_FILTER(1)
