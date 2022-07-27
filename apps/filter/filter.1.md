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

\-\-save-voxel-indexes, -i
:   Save voxel indexes into extra fields e0, e1, and e2. This will allow
    you to upsample the resulting point cloud with the *spoc_tool*
    application after you have made modifications to is. For example, if
    you assign classifications to the downsampled point cloud, you may
    want to upsample the point cloud in order to propogate the
    classifications up into the original high resolution point cloud.

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

# SEE ALSO

SPOC\_TOOL(1)
SPOC\_TRANSFORM(1)
