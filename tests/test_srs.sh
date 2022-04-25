#!/usr/bin/bash
spoc_srs --help 2> /dev/null

# Create a tmp directory for intermediate files
TMPDIR=$(mktemp --tmpdir --directory spoc.XXXXXXXX)

# Create a cleanup function
function cleanup {
    rm -rf ${TMPDIR}
}

# Run cleanup on exit
trap cleanup EXIT

spoc_srs ./test_data/lidar/juarez50.spoc > /dev/null
spoc_srs -s "invalid" ./test_data/lidar/juarez50.spoc ${TMPDIR}/juarez50_invalid_srs.spoc
