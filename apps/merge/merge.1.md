% SPOC_MERGE(1) SPOC User's Manual | Version 0
% spoc@zetamon.xyz
% December 25, 2021

# NAME

spoc_merge - Combine two or more SPOC files into a single file

# USAGE

spoc_merge [*options*] *file1* *file2* [*file3* ...]

# DESCRIPTION

Combine all the point records in two or more SPOC files into a single
SPOC file.

Input files are specified on the command line, and the merged file is
written to stdout.

# OPTIONS

--help, -h
:   Get help

--verbose, -v
:   Set verbose mode ON

--quiet, -q
:   Don't produce warnings when common mistakes are detected

--point-id=#, -p #
:   Set all point IDs in the merged file to #. By default, the point IDs
    are numbered according to the 0-based index of the input file's
    occurance on the command line.

# SEE ALSO

SPOC_TILE(1)
