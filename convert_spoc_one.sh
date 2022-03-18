#!/usr/bin/bash

# Get filename parts
fn=$(basename $1)
dn=$(dirname $1)
sn=$(basename ${dn})
echo converting ${sn}...

# Create a tmp directory for intermediate files
TMPDIR=$(mktemp --tmpdir --directory spc.XXXXXXXX)

# Create a cleanup function
function cleanup {
    rm -rf ${TMPDIR}
}

# Run cleanup on exit
trap cleanup EXIT

# Convert to spc
./build/release/text2spc -v < $2/${sn}.txt > $2/${sn}.spc

# Zip it up
zip $2/${sn}.zip $2/${sn}.spc
