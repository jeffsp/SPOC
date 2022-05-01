# Exit on error
set -e

# Set input and output pipe names
input=/tmp/spoc_transform_filter_input
output=/tmp/spoc_transform_filter_output

echo Input pipe = ${input}
echo Output pipe = ${output}

filter_source=./filters/spoc_transform_filter.cpp
filter_executable=/tmp/spoc_transform_filter
echo Compiling transform filter...
c++ -I ./filters -o ${filter_executable} ${filter_source}

echo Creating pipes...
rm -f ${input}
rm -f ${output}
mkfifo ${input}
mkfifo ${output}

echo Running transform application in the background...
./build/debug/spoc_transform -v -i ${input} -o ${output} \
    test_data/lidar/juarez50.spoc \
    /tmp/juarez50_transformed.spoc \
    &

echo Running the transform filter...
${filter_executable} ${input} ${output}

# Wait for the background process to finish
wait

echo The file checksums should match
md5sum test_data/lidar/juarez50.spoc /tmp/juarez50_transformed.spoc
