default: compile test

.PHONY: convert_laz # Convert LAS files to LAZ files
convert_laz:
	@cd ./scripts && \
		./convert_laz_all.sh \
		$$(realpath ../datasets/las_files) \
		$$(realpath ../results/spoc_file_format)

.PHONY: convert_spoc # Convert LAS files to SPOC files
convert_spoc:
	@PATH=$$(pwd)/build/debug:$$PATH && \
		cd ./scripts && \
		./convert_spoc_all.sh \
		$$(realpath ../datasets/las_files) \
		$$(realpath ../results/spoc_file_format)

.PHONY: compare # Compare LAZ and SPOC file sizes
compare:
	@cd ./scripts && \
		./compare_all.sh \
		$$(realpath ../datasets/las_files) \
		$$(realpath ../results/spoc_file_format)

.PHONY: cmake # Use cmake to generate Makefiles
cmake:
	mkdir -p build/debug
	mkdir -p build/release
	cd build/debug && cmake -DCMAKE_BUILD_TYPE=Debug ../..
	cd build/release && cmake -DCMAKE_BUILD_TYPE=Release ../..

.PHONY: laslib # Build LASlib library
laslib:
	$(MAKE) -j -C laslib/LASlib

.PHONY: compile # Compile all applications and tests
compile: laslib
	cd build/debug && make -j 24
	cd build/release && make -j 24

.PHONY: clean # Clean build objects
clean:
	rm -rf build

.PHONY: test # Run tests
test:
	./build/debug/test_spoc
	./build/release/test_spoc
	./build/debug/test_compress
	./build/release/test_compress
	./build/debug/test_extent
	./build/release/test_extent

.PHONY: memcheck # Run memcheck
memcheck:
	valgrind --leak-check=full --error-exitcode=1 --quiet ./build/debug/test_compress
	valgrind --leak-check=full --error-exitcode=1 --quiet ./build/release/test_compress
	valgrind --leak-check=full --error-exitcode=1 --quiet ./build/debug/test_extent
	valgrind --leak-check=full --error-exitcode=1 --quiet ./build/release/test_extent
	valgrind --leak-check=full --error-exitcode=1 --quiet ./build/debug/test_spoc
	valgrind --leak-check=full --error-exitcode=1 --quiet ./build/release/test_spoc

.PHONY: cppcheck # Run cppcheck
cppcheck:
	@cppcheck --enable=all -q --error-exitcode=255 \
		-I include -I apps -I laslib/LASlib/inc \
		--inline-suppr \
		--suppress=missingIncludeSystem \
		--suppress=*:laslib/LASlib/inc/* \
		apps/*.cpp

.PHONY: help # Generate list of targets with descriptions
help:
	@grep '^.PHONY: .* #' Makefile | sed 's/\.PHONY: \(.*\) # \(.*\)/\1	\2/' | expand -t20
