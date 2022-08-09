#!/usr/bin/bash

#usage: convert_spoc_all.sh <input_directory> <output_directory>

SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
repodir=$(dirname $SCRIPTPATH)
fn=$(basename $0)

# Convert to spoc file
parallel -k --joblog /tmp/${fn}.log ${repodir}/format_conversion/convert_spoc_one.sh {} $2 ::: \
$(find $1 -name "*.las" -or -name "*.laz")
