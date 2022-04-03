default: compile test

.PHONY: convert_laz # Convert LAS files to LAZ files
convert_laz:
	./convert_laz_all.sh ./datasets/las_files ./results/spoc_file_format

.PHONY: convert_spoc # Convert LAS files to SPOC files
convert_spoc:
	./convert_spoc_all.sh ./datasets/las_files ./results/spoc_file_format

.PHONY: compare # Compare LAZ and SPOC file sizes
compare:
	./compare_all.sh ./datasets/las_files ./results/spoc_file_format

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
	./build/debug/test_compress
	./build/release/test_compress
	./build/debug/test_extent
	./build/release/test_extent
	./build/debug/test_spoc
	./build/release/test_spoc

.PHONY: memcheck # Run memcheck
memcheck:
	valgrind --leak-check=full --error-exitcode=1 --quiet ./build/debug/test_compress
	valgrind --leak-check=full --error-exitcode=1 --quiet ./build/release/test_compress
	valgrind --leak-check=full --error-exitcode=1 --quiet ./build/debug/test_extent
	valgrind --leak-check=full --error-exitcode=1 --quiet ./build/release/test_extent
	valgrind --leak-check=full --error-exitcode=1 --quiet ./build/debug/test_spoc
	valgrind --leak-check=full --error-exitcode=1 --quiet ./build/release/test_spoc

.PHONY: laslib # Build LASlib library
laslib:
	$(MAKE) -j -C laslib/LASlib

.PHONY: help # Generate list of targets with descriptions
help:
	@grep '^.PHONY: .* #' Makefile | sed 's/\.PHONY: \(.*\) # \(.*\)/\1	\2/' | expand -t20
