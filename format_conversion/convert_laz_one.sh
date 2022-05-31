#!/usr/bin/bash

# Get filename parts
fn=$(basename $1)
dn=$(dirname $1)
sn=$(basename ${dn})
echo converting ${sn}...

# Convert to las
laszip $1 -o $2/${sn}.las

# Convert to laz
laszip $1 -o $2/${sn}.laz
