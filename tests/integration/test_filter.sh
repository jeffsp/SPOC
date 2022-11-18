# Fail on error
set -e

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
