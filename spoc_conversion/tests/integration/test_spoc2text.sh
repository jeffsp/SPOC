# Fail on error
set -e

./build/debug/spoc2text --help 2> /dev/null

# Create a tmp directory for intermediate files
TMPDIR=$(mktemp --tmpdir --directory spoc.XXXXXXXX)

# Create a cleanup function
function cleanup {
    rm -rf ${TMPDIR}
}

# Run cleanup on exit
trap cleanup EXIT

./build/debug/spoc2text < ../test_data/lidar/juarez50.spoc > ${TMPDIR}/juarez50.txt
./build/debug/spoc2text < ../test_data/lidar/juarez50.zpoc > ${TMPDIR}/juarez50.txt
