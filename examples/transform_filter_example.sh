# Exit on error
set -e

# Compile the filter
filter_source=./filters/spoc_noop_transform_filter.cpp
filter_executable=/tmp/spoc_noop_transform_filter
c++ -I ./filters -o ${filter_executable} ${filter_source}

# Create the input and output pipes
input=/tmp/spoc_transform_filter_input
output=/tmp/spoc_transform_filter_output
rm -f ${input}
rm -f ${output}
mkfifo ${input}
mkfifo ${output}

# Run the transform application in the background
./build/debug/spoc_transform -v -i ${input} -o ${output} \
    test_data/lidar/juarez50.spoc \
    /tmp/juarez50_transformed.spoc \
    &

# Run the transform filter in the foreground
${filter_executable} ${input} ${output}

# Wait for the background process to finish
wait

# Return an error code if the files differ
./build/debug/spoc_diff test_data/lidar/juarez50.spoc /tmp/juarez50_transformed.spoc

# Run checksums
md5sum test_data/lidar/juarez50.spoc /tmp/juarez50_transformed.spoc
