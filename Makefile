default: build test

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

.PHONY: prep_merge # Prepare for merging
prep_merge: cppcheck clean cmake build test memcheck coverage

.PHONY: cppcheck # Run cppcheck
cppcheck:
	@echo "Running cppcheck..."
	@cppcheck --std=c++14 --language=c++ --enable=all \
		-q --error-exitcode=255 \
		-I include -I apps -I laslib/LASlib/inc \
		--inline-suppr \
		--suppress=missingIncludeSystem \
		--suppress='*:laslib/LASlib/inc/*' \
		apps/*.cpp

.PHONY: cmake # Use cmake to generate Makefiles
cmake:
	@echo "Running cmake..."
	@mkdir -p build/debug
	@mkdir -p build/release
	@mkdir -p build/coverage
	@cd build/debug && cmake -DCMAKE_BUILD_TYPE=Debug ../..
	@cd build/release && cmake -DCMAKE_BUILD_TYPE=Release ../..
	@cd build/coverage && cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_COVERAGE=ON ../..

.PHONY: laslib # Build LASlib library
laslib:
	$(MAKE) -j -C laslib/LASlib

.PHONY: build # Compile all applications and tests
build: laslib
	cd build/debug && make -j 24
	cd build/release && make -j 24
	cd build/coverage && make -j 24

.PHONY: clean # Clean build objects
clean:
	@echo "Cleaning..."
	@rm -rf build

.PHONY: unit_test
unit_test: BUILD=debug
unit_test:
	@echo ===== $(BUILD) =====
	@parallel --jobs 24 --halt now,fail=1 "echo {} && {}" ::: build/$(BUILD)/test_*

.PHONY: test # Run tests
test:
	@echo "Testing..."
	@$(MAKE) --no-print-directory unit_test BUILD=debug
	@$(MAKE) --no-print-directory unit_test BUILD=release

.PHONY: memcheck # Run memcheck
memcheck:
	@echo "Running memchecks..."
	@parallel --jobs 24 --halt now,fail=1 \
		"echo {} && valgrind --leak-check=full --error-exitcode=1 --quiet {}" ::: build/debug/test_*
	@parallel --jobs 24 --halt now,fail=1 \
		"echo {} && valgrind --leak-check=full --error-exitcode=1 --quiet {}" ::: build/release/test_*

.PHONY: coverage # Generate a coverage report
coverage: build
	@$(MAKE) --no-print-directory unit_test BUILD=coverage
	@mkdir -p ./code_analysis
	@echo "Running gcovr..."
	@cd build/coverage/ && gcovr -r ../.. \
		--filter=../../include . | tee ../../code_analysis/coverage.txt
	@grep TOTAL code_analysis/coverage.txt

.PHONY: help # Generate list of targets with descriptions
help:
	@grep '^.PHONY: .* #' Makefile | sed 's/\.PHONY: \(.*\) # \(.*\)/\1	\2/' | expand -t20
