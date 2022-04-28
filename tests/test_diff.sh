#!/usr/bin/bash

# Fail on error
set -e

spoc_compare --help 2> /dev/null
spoc_compare --verbose ./test_data/lidar/juarez50.spoc ./test_data/lidar/juarez50.spoc > /dev/null
