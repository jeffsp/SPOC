#!/usr/bin/bash

# usage: convert_laz_one.sh <input_filename> <output_directory>

# Get filename parts
fn=$(basename $1)
dn=$(dirname $1)
sn=${fn%.*}
echo converting ${sn}...

# Convert to las
laszip $1 -o $2/${sn}.las

# Convert to laz
laszip $1 -o $2/${sn}.laz
