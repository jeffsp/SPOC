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

# Run the filter
./build/debug/spoc_filter_noop \
    test_data/lidar/juarez50.spoc \
    ${TMPDIR}/juarez50_transformed.spoc

# Return an error code if the files differ
./build/debug/spoc_diff test_data/lidar/juarez50.spoc \
    ${TMPDIR}/juarez50_transformed.spoc
