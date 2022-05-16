% SPOC_TILE(1) SPOC User's Manual | Version 0.1
% spoc@zetamon.xyz
% December 25, 2021

# NAME

spoc_tile - Split a spoc file into non-overlapping square tiles

# USAGE

spoc_tile [*options*] *filename.spoc*

# DESCRIPTION

Split a point cloud into square tiles.

The 'tiles' option specifies how many tiles should be used along the
point cloud's longest extent. For example, if the point cloud is 1200 X
900 meters, and the tiles option is set to 4, each tile will be 300 X
300 meters.

Optionally, you can specify the size of each tile using the 'tile-size'
option. For example, if the point cloud is 1200 X 800 meters, and the
tile-size option is set to 400 meters, six 400 X 400 tiles will be created.

The 'prefix' option determines the output filename. An empty prefix
uses the input file's basename.

Output file names consist of the prefix (or input file's basename if no
prefix is specified), followed by decimal digits that enumerate the tile
number.

The program will not overwrite existing files. If the filename being
written exists, the program will abort with an error. Use the 'f' option
to force overwrites.

# OPTIONS

--help, -h
:   Get help

--verbose, -v
:   Set verbose mode ON

--force, -f
:   Force overwrites of existing files.

--tiles=*#*, -t *#*
:   Number of tiles along the largest dimension

--digits=*#*, -d *#*
:   Number of digits to use when numbering the output filenames.

--tile-size=*#*, -s *#*
:   Length of one side of a tile

--prefix=*string*, -p *string*
:   The prefix to use for the output files

# SEE ALSO

SPOC_MERGE(1)
