#!/usr/bin/bash

# Get filename parts
fn=$(basename $1)
dn=$(dirname $1)
sn=$(basename ${dn})
echo converting ${sn}...

# Convert to spoc
./build/release/text2spoc -v < $2/${sn}.txt > $2/${sn}.spoc
