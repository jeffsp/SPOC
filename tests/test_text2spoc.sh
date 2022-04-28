#!/usr/bin/bash

# Fail on error
set -e

spoc_text2spoc --help 2> /dev/null

# Create a tmp directory for intermediate files
TMPDIR=$(mktemp --tmpdir --directory spoc.XXXXXXXX)

# Create a cleanup function
function cleanup {
    rm -rf ${TMPDIR}
}

# Run cleanup on exit
trap cleanup EXIT

spoc_spoc2text < ./test_data/lidar/juarez50.spoc > ${TMPDIR}/juarez50.txt
spoc_text2spoc < ${TMPDIR}/juarez50.txt > ${TMPDIR}/juarez50.spoc
