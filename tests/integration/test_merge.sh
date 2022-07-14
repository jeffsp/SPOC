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

spoc_merge --help 2> /dev/null
spoc_tile -p ${TMPDIR}/spoc_tile ./test_data/lidar/juarez50.spoc
spoc_merge ${TMPDIR}/spoc_tile*.spoc > ${TMPDIR}/spoc_merged.spoc
spoc_info -s ./test_data/lidar/juarez50.spoc > ${TMPDIR}/spoc_merge_header1.txt
spoc_info -s ${TMPDIR}/spoc_merged.spoc > ${TMPDIR}/spoc_merge_header2.txt
diff \
    ${TMPDIR}/spoc_merge_header1.txt \
    ${TMPDIR}/spoc_merge_header2.txt
