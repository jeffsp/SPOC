# Exit on error
set -e

# Run the filter
./build/debug/spoc_filter_noop \
    test_data/lidar/juarez50.spoc \
    /tmp/juarez50_transformed.spoc

# Return an error code if the files differ
./build/debug/spoc_diff test_data/lidar/juarez50.spoc /tmp/juarez50_transformed.spoc
