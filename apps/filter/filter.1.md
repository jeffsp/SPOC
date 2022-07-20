% SPOC\_FILTER(1) SPOC User's Manual | Version 0.1
% spoc@zetamon.xyz
% December 25, 2021

# NAME

spoc\_filter - Filter out points from a SPOC file

# USAGE

spoc\_filter [*options*] [*input_filename*] [*output_filename*]

# DESCRIPTION

Remove point records from a SPOC file that satisfy certain properties

# OPTIONS

\-\-help, -h
:   Get help

\-\-verbose, -v
:   Set verbose mode ON

\-\-version, -e
:   Print version information and exit

\-\-keep-class=*#*, -k *#*
:   Only keep point records whose classification field is set to *#*.
    All other point records are removed. This option may be specified
    multiple times.

\-\-remove-class=*#*, -r *#*
:   Remove all point records whose classification field is set to *#*.
    All other points are kept. This option may be specified multiple
    times.

\-\-unique-xyz, -u
:   Remove duplicates with the same X, Y, Z values. This is useful for
    removing points whose X, Y, and Z values have been quantized with the
    spoc_transform application. This will prevent any two points from
    occupying the same exact location in space. The first occurrance of
    a point record with a non-unique X, Y, Z location is the one that
    gets saved. All subsequent occurances are removed.

\-\-subsample=*#*, -s *#*
:   Subsample a point cloud by selecting a single random point record
    from each voxel of size *#*. This is useful for decimating a point
    cloud while still preserving the local structure. The first
    occurrance of a point record within a voxel is the one that gets
    saved. All subsequent occurances are removed.

# SEE ALSO

SPOC\_TRANSFORM(1)
