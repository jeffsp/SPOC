# Fail on error
set -e

spoc_info --help 2> /dev/null
spoc_info ./test_data/lidar/juarez50.spoc > /dev/null
spoc_info ./test_data/lidar/juarez50.spoz > /dev/null
