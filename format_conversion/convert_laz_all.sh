#!/usr/bin/bash

# usage: convert_laz_one.sh <input_directory> <output_directory>

# Convert to laz file
SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
repodir=$(dirname $SCRIPTPATH)

parallel -k ${repodir}/format_conversion/convert_laz_one.sh {} $2 ::: \
$(find $1 -name "*.las" -or -name "*.laz")
