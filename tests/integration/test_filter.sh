# Fail on error
set -e

spoc_filter --help 2> /dev/null

spoc_filter --unique-xyz \
    test_data/lidar/juarez50.spoc \
    ${TMPDIR}/output.spoc

spoc_filter --subsample=1.1 \
    test_data/lidar/juarez50.spoc \
    ${TMPDIR}/output.spoc

spoc_filter --subsample=1.1 \
    --save-voxel-indexes \
    test_data/lidar/juarez50.spoc \
    ${TMPDIR}/output.spoc

# This should fail: can't save indexes if you aren't subsampling
! spoc_filter -i \
    test_data/lidar/juarez50.spoc \
    ${TMPDIR}/output.spoc &> /dev/null

# This should fail: can't save indexes when subsampling == 0.0
! spoc_filter --subsample=0.0 \
    --save-voxel-indexes \
    test_data/lidar/juarez50.spoc \
    ${TMPDIR}/output.spoc &> /dev/null

