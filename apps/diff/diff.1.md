% SPOC_DIFF(1) SPOC User's Manual | Version 0.1
% spoc@spocfile.xyz
% December 25, 2021

# NAME

spoc_diff - Compare two spoc files

# USAGE

spoc_diff [*options*] [*file1*] [*file2*]

# DESCRIPTION

Compare two spoc files and return an error code if they are different,
otherwise return a success code (0);

# OPTIONS

\-\-help, -h
:   Get help

\-\-verbose, -v
:   Set verbose mode ON

\-\-version, -e
:   Print version information and exit

\-\-header-only, -a
: Only compare the headers

\-\-data-only, -d
: Only compare the data fields

\-\-field=*F*, -f *F*
: Only compare field F, where F is one of 'x', 'y', 'z', 'c', 'p', 'i',
    'r', 'g', 'b', '0', '1', ..., '7'.

\-\-reverse, -r
: Reverse the meaning of the comparison by returning an error if they
are equal and success if they are not equal

# SEE ALSO

SPOC_DIFF(1)
