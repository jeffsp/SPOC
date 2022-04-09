#!/usr/bin/bash

# Compare
parallel -k ./compare_one.sh {} $2 ::: \
$(find $1 -name "*.las" -or -name "*.laz")
