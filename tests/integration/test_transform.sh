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

# Add
#spoc_info test_data/lidar/juarez50.spoc
spoc_transform --add-x=100 \
    test_data/lidar/juarez50.spoc \
    ${TMPDIR}/output.spoc
#spoc_info ${TMPDIR}/output.spoc
spoc_transform --add-y=100 \
    < test_data/lidar/juarez50.spoc \
    > ${TMPDIR}/output.spoc
#spoc_info ${TMPDIR}/output.spoc
spoc_transform --add-z=100 \
    < test_data/lidar/juarez50.spoc \
    > ${TMPDIR}/output.spoc
#spoc_info ${TMPDIR}/output.spoc

# Gaussian noise
spoc_transform --gaussian-noise=0.01 \
    test_data/lidar/juarez50.spoc \
    ${TMPDIR}/output.spoc
spoc_transform --gaussian-noise-x=0.01 \
    test_data/lidar/juarez50.spoc \
    ${TMPDIR}/output.spoc
spoc_transform --gaussian-noise-y=0.01 \
    test_data/lidar/juarez50.spoc \
    ${TMPDIR}/output.spoc
spoc_transform --gaussian-noise-z=0.01 \
    test_data/lidar/juarez50.spoc \
    ${TMPDIR}/output.spoc

# Quantize
spoc_transform --quantize-xyz=0.01 \
    test_data/lidar/juarez50.spoc \
    ${TMPDIR}/output.spoc
#spoc_info ${TMPDIR}/output.spoc
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

# Replace NOT
#spoc_info test_data/lidar/juarez50.spoc
spoc_transform --replace-not=c,2,4,6,1 \
    < test_data/lidar/juarez50.spoc \
    > ${TMPDIR}/output.spoc
#spoc_info ${TMPDIR}/output.spoc

# This should fail
! spoc_transform --replace-not=c,2 \
    < test_data/lidar/juarez50.spoc \
    > ${TMPDIR}/output.spoc &> /dev/null

# This should fail
! spoc_transform --replace-not=x,1,2 \
    < test_data/lidar/juarez50.spoc \
    > ${TMPDIR}/output.spoc &> /dev/null

# This should fail
! spoc_transform --replace-not=q,1,2 \
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

# Scale
#spoc_info test_data/lidar/juarez50.spoc
spoc_transform --scale-x=10 \
    test_data/lidar/juarez50.spoc \
    ${TMPDIR}/output.spoc
#spoc_info ${TMPDIR}/output.spoc
spoc_transform --scale-y=10 \
    < test_data/lidar/juarez50.spoc \
    > ${TMPDIR}/output.spoc
#spoc_info ${TMPDIR}/output.spoc
spoc_transform --scale-z=10 \
    < test_data/lidar/juarez50.spoc \
    > ${TMPDIR}/output.spoc
#spoc_info ${TMPDIR}/output.spoc

# Uniform noise
spoc_transform --uniform-noise=0.01 \
    test_data/lidar/juarez50.spoc \
    ${TMPDIR}/output.spoc
spoc_transform --uniform-noise-x=0.01 \
    test_data/lidar/juarez50.spoc \
    ${TMPDIR}/output.spoc
spoc_transform --uniform-noise-y=0.01 \
    test_data/lidar/juarez50.spoc \
    ${TMPDIR}/output.spoc
spoc_transform --uniform-noise-z=0.01 \
    test_data/lidar/juarez50.spoc \
    ${TMPDIR}/output.spoc

# Multiple
#spoc_info test_data/lidar/juarez50.spoc
spoc_transform \
    --add-x=100 \
    --quantize-xyz=0.01 \
    --rotate-z=12.3 \
    test_data/lidar/juarez50.spoc \
    ${TMPDIR}/output.spoc
#spoc_info ${TMPDIR}/output.spoc
