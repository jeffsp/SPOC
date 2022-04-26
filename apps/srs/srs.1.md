% SPOC_SRS(1) SPOC User's Manual | Version 0
% spoc@zetamon.xyz
% December 25, 2021

# NAME

spoc_srs - Get or set the spatial reference system information in a SPOC
file.

# USAGE

spoc_srs [*options*] [*input1*] [*input2*] [*...*]
spoc_srs [*options*] -s '<OGC WKT string>' *input* *output*

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

    $ ls omaha?.spoc
    omaha1.spoc  omaha2.spoc  omaha3.spoc
    $ spoc_srs < omaha1.spoc
    PROJCS["WGS 84 / WGS 84 / UTM 18N",...
    $ spoc_srs omaha*.spoc
    PROJCS["WGS 84 / WGS 84 / UTM 18N",...
    PROJCS["WGS 84 / WGS 84 / UTM 18N",...
    PROJCS["WGS 84 / WGS 84 / UTM 18N",...

Set the SRS

    $ spoc_srs -s "invalid" omaha3.spoc omaha3_no_srs.spoc
    $ spoc_srs omaha3_no_srs.spoc
    invalid
    $ spoc_srs omaha1.spoc > srs.txt
    $ spoc_srs -s "$(cat srs.txt)" omaha3_no_srs.spoc omaha3_with_srs.spoc
    $ spoc_srs omaha3_with_srs.spoc
    PROJCS["WGS 84 / WGS 84 / UTM 18N",...

Equivalently

    $ spoc_srs -s "invalid" < omaha3.spoc > omaha3_no_srs.spoc
    $ spoc_srs omaha3_no_srs.spoc
    invalid
    $ spoc_srs omaha1.spoc > srs.txt
    $ spoc_srs -s "$(cat srs.txt)" < omaha3_no_srs.spoc > omaha3_with_srs.spoc
    $ spoc_srs omaha3_with_srs.spoc
    PROJCS["WGS 84 / WGS 84 / UTM 18N",...

# SEE ALSO

SPOC_INFO(1)
