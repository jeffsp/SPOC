#!/usr/bin/bash

# Convert to spc file
parallel -k ./convert_spc_one.sh {} $2 ::: \
$(find $1 -name "*.las" -or -name "*.laz")
