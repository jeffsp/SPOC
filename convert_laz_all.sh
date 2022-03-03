#!/usr/bin/bash

# Convert to laz file
parallel -k ./convert_laz_one.sh {} $2 ::: \
$(find $1 -name "*.las" -or -name "*.laz")
