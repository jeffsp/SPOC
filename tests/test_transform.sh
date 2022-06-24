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
spoc_transform -h &> /dev/null
spoc_transform --help &> /dev/null

# This should fail
! spoc_transform --asdf &> /dev/null

# Quantize
spoc_transform --quantize-xyz=0.01 \
    test_data/lidar/juarez50.spoc \
    ${TMPDIR}/output.spoc
#spoc_info ${TMPDIR}/juarez50_quantized.spoc
spoc_transform --quantize-xyz=10 \
    < test_data/lidar/juarez50.spoc \
    > ${TMPDIR}/output.spoc
#spoc_info ${TMPDIR}/output.spoc

# Replace
#spoc_info test_data/lidar/juarez50.spoc
spoc_transform --replace=c,1,2 \
    < test_data/lidar/juarez50.spoc \
    > ${TMPDIR}/output.spoc
#spoc_info ${TMPDIR}/output.spoc

# This should fail
! spoc_transform --replace=x,1,2 \
    < test_data/lidar/juarez50.spoc \
    > ${TMPDIR}/output.spoc &> /dev/null

# This should fail
! spoc_transform --replace=q,1,2 \
    < test_data/lidar/juarez50.spoc \
    > ${TMPDIR}/output.spoc &> /dev/null

# Rotate
#spoc_info test_data/lidar/juarez50.spoc
spoc_transform --rotate-x=90 \
    < test_data/lidar/juarez50.spoc \
    > ${TMPDIR}/output.spoc
#spoc_info ${TMPDIR}/output.spoc

#spoc_info test_data/lidar/juarez50.spoc
spoc_transform --rotate-y=90 \
    < test_data/lidar/juarez50.spoc \
    > ${TMPDIR}/output.spoc
#spoc_info ${TMPDIR}/output.spoc

#spoc_info test_data/lidar/juarez50.spoc
spoc_transform --rotate-z=90 \
    < test_data/lidar/juarez50.spoc \
    > ${TMPDIR}/output.spoc
#spoc_info ${TMPDIR}/output.spoc
