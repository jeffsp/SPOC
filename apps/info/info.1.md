% SPOC_INFO(1) SPOC User's Manual | Version 0
% spoc@zetamon.xyz
% December 25, 2021

# NAME

spoc_info - Print information about a SPOC file to stdout

# USAGE

spoc_info [*options*] [*file*]

# DESCRIPTION

Print header and/or summary information about a SPOC file

Information about each field in a SPOC file is given in the output. For
example, in non-json compact mode the field's range and quartiles are
listed.

Below, the 'x' field is shown to have a range of 249.950 meters. The
minimum value is 356631.450, and each quartile's size is shown following
the minimum value, and the last value is the field's maximum,
356881.400.

    x       range=249.950, 356631.450 +87.350 +45.920 +39.090 +77.590 = 356881.400

Below, the 'i' (intensity) field is shown to have a range of 4087. The
minimum value is 1, and each quartile's size is shown following the
minimum value, and the last value is the field's maximum, 4088.

This notation allows you to get an rough estimate of the shape of the
field's distribution. For example, notice how the size of the last
quartile is very large relative to first three quartiles. This indicates
that the intensity distribution is skewed toward zero. That is, most of
the intensity values (75% of them) are below 88 (1+19+35+33), while the
highest 25% of the values are spread out over the range 89 to 4088.

    i       range=4087, 1 +19 +35 +33 +4000 = 4088

# OPTIONS

\-\-help, -h
:   Get help

\-\-verbose, -v
:   Set verbose mode ON

\-\-version, -e
:   Print version information and exit

\-\-json, -j
: Toggle json output switch

\-\-header-info, -a
: Toggle header information switch

\-\-summary-info, -s
: Toggle summary information switch

\-\-classifications, -l
: Toggle classification information switch

\-\-compact, -c
: Toggle compact output mode switch

# EXAMPLES

Compact, text mode output

    $ spoc_info < Austin.spoc
    major_version	0
    minor_version	1
    wkt	PROJCS["NAD83...]
    npoints	117456
    x	range=249.950, 356631.450 +87.350 +45.920 +39.090 +77.590 = 356881.400
    y	range=306.600, 3515084.930 +125.650 +52.810 +40.120 +88.020 = 3515391.530
    z	range=24.860, 1134.220 +2.290 +2.240 +3.290 +17.040 = 1159.080
    c	range=12, 1 +1 +0 +2 +9 = 13
    p	range=1, 152 +0 +1 +0 +0 = 153
    i	range=4087, 1 +19 +35 +33 +4000 = 4088

Compact, text mode output without header information

    $ spoc_info -h < Austin.spoc
    x	range=249.950, 356631.450 +87.350 +45.920 +39.090 +77.590 = 356881.400
    y	range=306.600, 3515084.930 +125.650 +52.810 +40.120 +88.020 = 3515391.530
    z	range=24.860, 1134.220 +2.290 +2.240 +3.290 +17.040 = 1159.080
    c	range=12, 1 +1 +0 +2 +9 = 13
    p	range=1, 152 +0 +1 +0 +0 = 153
    i	range=4087, 1 +19 +35 +33 +4000 = 4088

Non-compact, JSON output

    $ spoc_info -c -j < Austin.spoc
    {
        'header' : {
            'major_version' : 0,
            'minor_version' : 1,
            'npoints' : 117456,
            'wkt' : 'PROJCS["NAD83..."]]'
        },
        'summary' : {
            ...
            'c' : {
                'q0' : 1,
                'q1' : 2,
                'q2' : 2,
                'q3' : 4,
                'q4' : 13,
                'range' : 12
            },
            ...
            'x' : {
                'q0' : 356631.450000000011642,
                'q1' : 356718.800000000046566,
                'q2' : 356764.720000000030268,
                'q3' : 356803.810000000055879,
                'q4' : 356881.400000000023283,
                'range' : 249.950000000011642
            },
            'y' : {
                'q0' : 3515084.930000000167638,
                'q1' : 3515210.580000000074506,
                'q2' : 3515263.390000000130385,
                'q3' : 3515303.509999999776483,
                'q4' : 3515391.529999999795109,
                'range' : 306.599999999627471
            },
            'z' : {
                'q0' : 1134.220000000000027,
                'q1' : 1136.509999999999991,
                'q2' : 1138.750000000000000,
                'q3' : 1142.040000000000191,
                'q4' : 1159.080000000000155,
                'range' : 24.860000000000127
            }
        }
    }

# SEE ALSO

SPOC_SRS(1)
