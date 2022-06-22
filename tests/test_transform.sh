# Fail on error
set -e

# Create a tmp directory for intermediate files
TMPDIR=$(mktemp --tmpdir --directory spoc.XXXXXXXX)

# Create a cleanup function
function cleanup {
    rm -rf ${TMPDIR}
}

# Run cleanup on exit
trap cleanup EXIT

# Get help
./build/debug/spoc_transform -h &> /dev/null
./build/debug/spoc_transform --help &> /dev/null

# This should fail
! ./build/debug/spoc_transform --asdf &> /dev/null

# Quantize
./build/debug/spoc_transform --quantize-xyz=0.01 \
    test_data/lidar/juarez50.spoc \
    ${TMPDIR}/output.spoc
#./build/debug/spoc_info ${TMPDIR}/juarez50_quantized.spoc
./build/debug/spoc_transform --quantize-xyz=10 \
    < test_data/lidar/juarez50.spoc \
    > ${TMPDIR}/output.spoc
#./build/debug/spoc_info ${TMPDIR}/output.spoc

# Replace
#./build/debug/spoc_info test_data/lidar/juarez50.spoc
./build/debug/spoc_transform --replace=c,1,2 \
    < test_data/lidar/juarez50.spoc \
    > ${TMPDIR}/output.spoc
#./build/debug/spoc_info ${TMPDIR}/output.spoc

# This should fail
! ./build/debug/spoc_transform --replace=x,1,2 \
    < test_data/lidar/juarez50.spoc \
    > ${TMPDIR}/output.spoc &> /dev/null

# This should fail
! ./build/debug/spoc_transform --replace=q,1,2 \
    < test_data/lidar/juarez50.spoc \
    > ${TMPDIR}/output.spoc &> /dev/null
