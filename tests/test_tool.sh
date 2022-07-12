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

spoc_tool --help 2> /dev/null

# Get fields
spoc_tool --get-field=x test_data/eo/romeo007.spoc > ${TMPDIR}/x.txt
spoc_tool --get-field=y test_data/eo/romeo007.spoc > ${TMPDIR}/y.txt
spoc_tool --get-field=c test_data/eo/romeo007.spoc > ${TMPDIR}/c.txt
spoc_tool --get-field=i test_data/eo/romeo007.spoc > ${TMPDIR}/i.txt

# They should differ
! diff -q ${TMPDIR}/x.txt ${TMPDIR}/y.txt > /dev/null
! diff -q ${TMPDIR}/c.txt ${TMPDIR}/i.txt > /dev/null

# Set it unchanged
spoc_tool --set-field=i \
    --field-filename ${TMPDIR}/i.txt \
    < test_data/eo/romeo007.spoc \
    > ${TMPDIR}/romeo007.spoc

# The files should not differ
spoc_diff test_data/eo/romeo007.spoc ${TMPDIR}/romeo007.spoc

# Change a field
sed -i 's/1/2/g' ${TMPDIR}/c.txt

# Set it, using short option names
spoc_tool -s c \
    -f ${TMPDIR}/c.txt \
    < test_data/eo/romeo007.spoc \
    > ${TMPDIR}/romeo007.spoc

# The files should differ
! spoc_diff test_data/eo/romeo007.spoc ${TMPDIR}/romeo007.spoc
