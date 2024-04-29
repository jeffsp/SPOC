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

spoc_filter --help 2> /dev/null

spoc_filter --unique-xyz \
    test_data/lidar/juarez50.spoc \
    ${TMPDIR}/output.spoc

spoc_filter --subsample=1.1 \
    test_data/lidar/juarez50.spoc \
    ${TMPDIR}/output.spoc

spoc_filter --remove-coords "x > 100" \
    test_data/lidar/juarez50.spoc \
    ${TMPDIR}/output.spoc
