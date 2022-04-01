#!/usr/bin/bash

# Get filename parts
fn=$(basename $1)
dn=$(dirname $1)
sn=$(basename ${dn})
echo converting ${sn}...

# Convert to las
laszip64 $1 -o $2/${sn}.las

# Convert to laz
laszip64 $1 -o $2/${sn}.laz
