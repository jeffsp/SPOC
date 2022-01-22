#!/usr/bin/bash

# Get filename parts
fn=$(basename $1)
dn=$(dirname $1)
sn=$(basename ${dn})
echo converting ${sn}...

# Create a tmp directory for intermediate files
TMPDIR=$(mktemp --tmpdir --directory gpc.XXXXXXXX)

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
las2las64 -set_ogc_wkt -i $1 -o ${TMPDIR}/tmp.las

# Extract the coordinate system and save it to a file
lasinfo64 -i ${TMPDIR}/tmp.las -stdout \
    | grep PROJCS \
    > $2/${sn}.txt

# Append the data to the file
las2txt64 -i ${TMPDIR}/tmp.las -parse xyziRGBc -sep tab -stdout \
    >> $2/${sn}.txt
