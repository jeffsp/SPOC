default: compile test

.PHONY: convert # Convert LAS files to SPC files
convert_text:
	./convert_text_all.sh ./datasets/las_files ./results/spc_file_format

convert_laz:
	./convert_laz_all.sh ./datasets/las_files ./results/spc_file_format

convert_spc:
	./convert_spc_all.sh ./datasets/las_files ./results/spc_file_format

cmake:
	mkdir -p build/debug
	mkdir -p build/release
	cd build/debug && cmake -DCMAKE_BUILD_TYPE=Debug ../..
	cd build/release && cmake -DCMAKE_BUILD_TYPE=Release ../..

.PHONY: compile # Compile all applications
compile:
	cd build/debug && make -j 24
	cd build/release && make -j 24

.PHONY: clean # Clean build objects
clean:
	rm -rf build

.PHONY: test # Run tests
test:
	head -1000001 ./results/spc_file_format/Wellington.txt \
	| ./build/debug/test_double
	./build/debug/test_compress
	./build/release/test_compress
	./build/debug/test_extent
	./build/release/test_extent
	./build/debug/test_octree
	./build/release/test_octree

.PHONY: memcheck # Run memcheck
memcheck:
	valgrind --leak-check=full --error-exitcode=1 --quiet ./build/debug/test_compress
	valgrind --leak-check=full --error-exitcode=1 --quiet ./build/release/test_compress
	valgrind --leak-check=full --error-exitcode=1 --quiet ./build/debug/test_extent
	valgrind --leak-check=full --error-exitcode=1 --quiet ./build/release/test_extent
	valgrind --leak-check=full --error-exitcode=1 --quiet ./build/debug/test_octree
	valgrind --leak-check=full --error-exitcode=1 --quiet ./build/release/test_octree

.PHONY: run # Run app
run:
	./build/debug/text2spc -v < ./results/spc_file_format/Juarez.txt > Juarez.spc
	./build/debug/spc2text -v < Juarez.spc > Juarez.txt
	./build/debug/text2spc -v < Juarez.txt > Juarez2.spc
	./build/debug/spc2text -v < Juarez2.spc > Juarez2.txt
	diff -q Juarez.txt Juarez2.txt

.PHONY: help # Generate list of targets with descriptions
help:
	@grep '^.PHONY: .* #' Makefile | sed 's/\.PHONY: \(.*\) # \(.*\)/\1	\2/' | expand -t20
