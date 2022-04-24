% SPOC_SRS(1) SPOC User's Manual | Version 0
% spoc@zetamon.xyz
% December 25, 2021

# NAME

spoc_srs - Get or set the spatial reference system information in a SPOC
file.

# USAGE

spoc_srs [*options*] [*file*]

# DESCRIPTION

Get or set the spatial reference system information in a SPOC file.

The spatial reference system information in a spoc file is stored in a
string in Open Geospatial Consortium (OGC) well-known text (WKT) format.

This is the same format used by the ASPRS LAS 1.4 standard.

# OPTIONS

--help, -h
:   Get help

--verbose, -v
:   Set verbose mode ON

--srs=*string*, -s *string*
:   Set the SRS string

# EXAMPLES

Get the SRS

    $ spoc_srs < Austin.spoc
    $ spoc_srs aoi1.spoc aoi2.spoc aoi3.spoc

Set the SRS

    $ spoc_srs -s $(cat ogr_wkt.txt) < Austin.spoc > Austin2.spoc

# SEE ALSO

SPOC_SRS(1)
