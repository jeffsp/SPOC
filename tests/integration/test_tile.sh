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

spoc_tile --help 2> /dev/null
spoc_tile -p ${TMPDIR}/spoc_tile ./test_data/lidar/juarez50.spoc
spoc_tile -p ${TMPDIR}/spoc_tile ./test_data/lidar/juarez50.zpoc

# Should fail, needs the --force option
! spoc_tile -p ${TMPDIR}/spoc_tile ./test_data/lidar/juarez50.spoc 2> /dev/null

# Should fail, needs the --force option
! spoc_tile --prefix=${TMPDIR}/spoc_tile ./test_data/lidar/juarez50.zpoc 2> /dev/null

# Force overwrite
spoc_tile --force -p ${TMPDIR}/spoc_tile ./test_data/lidar/juarez50.spoc 2> /dev/null
spoc_tile --force --prefix=${TMPDIR}/spoc_tile ./test_data/lidar/juarez50.zpoc 2> /dev/null
