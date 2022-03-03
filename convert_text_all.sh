#!/usr/bin/bash

# Convert LAS files to text files
parallel -k ./convert_text_one.sh {} $2 ::: \
$(find $1 -name "*.las" -or -name "*.laz")
