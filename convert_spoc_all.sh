#!/usr/bin/bash

# Convert to spoc file
parallel -k ./convert_spoc_one.sh {} $2 ::: \
$(find $1 -name "*.las" -or -name "*.laz")
