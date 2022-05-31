#!/usr/bin/bash

# Get filename parts
fn=$(basename $1)
dn=$(dirname $1)
sn=$(basename ${dn})
echo converting ${sn}...

# Create a tmp directory for intermediate files
TMPDIR=$(mktemp --tmpdir --directory spoc.XXXXXXXX)

# Create a cleanup function
function cleanup {
    rm -rf ${TMPDIR}
}

# Run cleanup on exit
trap cleanup EXIT

# First ensure that the coordinate system is OGC WKT
#
# It's a shame that you have to convert the whole file just to change
# the coordinate system
las2las -set_ogc_wkt -i $1 -o ${TMPDIR}/tmp.las

# Convert to spoc
spoc_las2spoc -v ${TMPDIR}/tmp.las $2/${sn}.spoc
spoc_compress -v $2/${sn}.spoc $2/${sn}.spoc.z
