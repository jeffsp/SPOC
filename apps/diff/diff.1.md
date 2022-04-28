% SPOC_DIFF(1) SPOC User's Manual | Version 0
% spoc@zetamon.xyz
% December 25, 2021

# NAME

spoc_diff - Compare two spoc files

# USAGE

spoc_diff [*options*] [*file1*] [*file2*]

# DESCRIPTION

Compare two spoc files and return an error code if they are different,
otherwise return a success code (0);

# OPTIONS

--help, -h
:   Get help

--verbose, -v
:   Set verbose mode ON

--header, -e
: Only compare the headers

--data, -d
: Only compare the data fields

--field=*F*, -f *F*
: Only compare field F, where F is one of 'x', 'y', 'z', 'c', 'p', 'i',
    'r', 'g', 'b', '0', '1', ..., '7'.

--reverse, -r
: Reverse the meaning of the comparison by returning an error if they
are equal and success if they are not equal

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

SPOC_INFO(1)
