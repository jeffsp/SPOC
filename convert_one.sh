#!/usr/bin/bash

# Get filename parts
fn=$(basename $1)
dn=$(dirname $1)
sn=$(basename ${dn})
echo converting ${sn}...

# Create a tmp directory for intermediate files
TMPDIR=$(mktemp --tmpdir --directory gpc.XXXXXXXX)

# Create a cleanup function
function cleanup {
    rm -rf ${TMPDIR}
}

# Run cleanup on exit
trap cleanup EXIT

# First ensure that the coordinate system is OGC WKT
#
# It's a shame that you have to convert the whole file just to change
# the coordinate system
las2las64 -set_ogc_wkt -i $1 -o ${TMPDIR}/tmp.las

# Save info to a text file
lasinfo64 -i ${TMPDIR}/tmp.las -stdout > ${TMPDIR}/info.txt

# Does it have a projection?
if grep -q PROJCS ${TMPDIR}/info.txt; then
    # Save the projection
    grep PROJCS ${TMPDIR}/info.txt > $2/${sn}.txt
else
    # If there is no projection, use a default (US National Mall)
    echo "    COMPD_CS["unknown",PROJCS["WGS 84 / UTM zone 18N",GEOGCS["WGS 84",DATUM["WGS_1984",SPHEROID["WGS 84",6378137,298.257223563,AUTHORITY["EPSG","7030"]],AUTHORITY["EPSG","6326"]],PRIMEM["Greenwich",0],UNIT["degree",0.0174532925199433],AUTHORITY["EPSG","4326"]],PROJECTION["Transverse_Mercator"],PARAMETER["latitude_of_origin",0],PARAMETER["central_meridian",-75],PARAMETER["scale_factor",0.9996],PARAMETER["false_easting",500000],PARAMETER["false_northing",0],UNIT["metre",1,AUTHORITY["EPSG","9001"]],AUTHORITY["EPSG","32618"]],VERT_CS["unknown",VERT_DATUM["World Geodetic System 1984",2002,AUTHORITY["EPSG","6326"]],UNIT["metre",1.0,AUTHORITY["EPSG","9001"]],AXIS["Up",UP]]]" > $2/${sn}.txt
fi

# Append the data to the file
las2txt64 -i ${TMPDIR}/tmp.las -parse xyziRGBc -sep tab -stdout \
    >> $2/${sn}.txt
