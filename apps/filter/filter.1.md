% SPOC\_FILTER(1) SPOC User's Manual | Version 0.1
% spoc@spocfile.xyz
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

\-\-random-seed=*#*, -a *#*
:   Set the random seed. This seed will determine the order of random
    operations like subsampling and filtering out non-unique values.

\-\-remove-class=*#*, -r *#*
:   Remove all point records whose classification field is set to *#*.
    All other points are kept. This option may be specified multiple
    times.

\-\-subsample=*#*, -s *#*
:   Subsample a point cloud by selecting a single random point record
    from each voxel of size *#*. This is useful for decimating a point
    cloud while still preserving the local structure. If the random seed
    is set to '0', then the first occurrance of a point record within a
    voxel is the one that gets saved. All subsequent occurances are
    removed. If the random-seed is set to a non-zero value, then the
    point that gets saved will be selected randomly from each voxel.

\-\-unique-xyz, -u
:   Remove duplicates with the same X, Y, Z values. This is useful for
    removing points whose X, Y, and Z values have been quantized with the
    spoc_transform application. This will prevent any two points from
    occupying the same exact location in space. If the random seed is
    set to '0', then the first occurrance of a point record with a
    non-unique X, Y, Z location is the one that gets saved. All
    subsequent occurances are removed. If the random-seed is set to a
    non-zero value, then the point with the non-unique value will be
    selected randomly.

\-\-remove-coords, -c
:   Remove points based on their coordinate values. To use this option
    specify a coordinate value, a comparison operator, and a float value
    separated by a space. Enclose these values in quotation marks. For
    example -c "x > 10" will remove all points with an x value greater
    than 10. Available coordinate values are x, y, and z. Available
    comparison operators are >, and <.

# SEE ALSO

SPOC\_TOOL(1)
SPOC\_TRANSFORM(1)
