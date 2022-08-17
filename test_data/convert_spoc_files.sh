# If the spoc file format changes, you will need to convert the test LAS
# files used for integration testing to SPOC files. To do this, use the
# following command from the test_data directory:
#
#     $ bash ./convert_spoc_files.sh

# Convert the test files from LAS to SPOC
../build/debug/spoc_las2spoc lidar/juarez50.las lidar/juarez50.spoc
../build/debug/spoc_las2spoc lidar/rome012.las lidar/rome012.spoc
../build/debug/spoc_las2spoc eo/romeo007.las eo/romeo007.spoc

# Compress
../build/debug/spoc_compress lidar/juarez50.spoc lidar/juarez50.zpoc

# Set the SRS for Rome
../build/debug/spoc_srs -v --srs='PROJCS["ETRS89 / ETRS89 / UTM 33N",GEOGCS["ETRS89",DATUM["European_Terrestrial_Reference_System_1989",SPHEROID["GRS 1980",6378137,298.257222101,AUTHORITY["EPSG","7019"]],AUTHORITY["EPSG","6258"]],PRIMEM["Greenwich",0,AUTHORITY["EPSG","8901"]],UNIT["degree",0.01745329251994328,AUTHORITY["EPSG","9122"]],AUTHORITY["EPSG","4258"]],PROJECTION["Transverse_Mercator"],PARAMETER["latitude_of_origin",0],PARAMETER["central_meridian",15],PARAMETER["scale_factor",0.9996],PARAMETER["false_easting",500000],PARAMETER["false_northing",0],UNIT["metre",1,AUTHORITY["EPSG","9001"]],AXIS["Easting",EAST],AXIS["Northing",NORTH],AUTHORITY["EPSG","25833"]]' lidar/rome012.spoc lidar/rome012.spoc
../build/debug/spoc_srs -v --srs='PROJCS["ETRS89 / ETRS89 / UTM 33N",GEOGCS["ETRS89",DATUM["European_Terrestrial_Reference_System_1989",SPHEROID["GRS 1980",6378137,298.257222101,AUTHORITY["EPSG","7019"]],AUTHORITY["EPSG","6258"]],PRIMEM["Greenwich",0,AUTHORITY["EPSG","8901"]],UNIT["degree",0.01745329251994328,AUTHORITY["EPSG","9122"]],AUTHORITY["EPSG","4258"]],PROJECTION["Transverse_Mercator"],PARAMETER["latitude_of_origin",0],PARAMETER["central_meridian",15],PARAMETER["scale_factor",0.9996],PARAMETER["false_easting",500000],PARAMETER["false_northing",0],UNIT["metre",1,AUTHORITY["EPSG","9001"]],AXIS["Easting",EAST],AXIS["Northing",NORTH],AUTHORITY["EPSG","25833"]]' eo/romeo007.spoc eo/romeo007.spoc
