# Fail on error
set -e

spoc_diff --help 2> /dev/null
# Same file
spoc_diff ./test_data/lidar/juarez50.spoc ./test_data/lidar/juarez50.spoc > /dev/null
# Same data, but header compression flag is different
! spoc_diff ./test_data/lidar/juarez50.spoc ./test_data/lidar/juarez50.zpoc > /dev/null
spoc_diff -e ./test_data/lidar/juarez50.spoc ./test_data/lidar/juarez50.spoc > /dev/null
spoc_diff -d ./test_data/lidar/juarez50.spoc ./test_data/lidar/juarez50.spoc > /dev/null
spoc_diff -f x ./test_data/lidar/juarez50.spoc ./test_data/lidar/juarez50.spoc > /dev/null
spoc_diff -f xyzrgbpci01234567 ./test_data/lidar/juarez50.spoc ./test_data/lidar/juarez50.spoc > /dev/null
spoc_diff -f x --field=yz --field=rgbpci01234567 ./test_data/lidar/juarez50.spoc ./test_data/lidar/juarez50.spoc > /dev/null
spoc_diff -f xyz --field=012345678 ./test_data/lidar/juarez50.spoc ./test_data/lidar/juarez50.spoc 2> /dev/null
# Invalid field specifier
! spoc_diff -f q ./test_data/lidar/juarez50.spoc ./test_data/lidar/juarez50.spoc 2> /dev/null
