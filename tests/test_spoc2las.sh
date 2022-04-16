las2las64 -set_ogc_wkt -i results/spoc_file_format/Juarez.las -o Juarez_with_ogc_wkt1.las
./build/debug/spoc_las2spoc Juarez_with_ogc_wkt1.las Juarez_with_ogc_wkt.spoc
./build/debug/spoc_spoc2las Juarez_with_ogc_wkt.spoc Juarez_with_ogc_wkt2.las
lasinfo64 Juarez_with_ogc_wkt1.las &> Juarez_info1.txt
lasinfo64 Juarez_with_ogc_wkt2.las &> Juarez_info2.txt
