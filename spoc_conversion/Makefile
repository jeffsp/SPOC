default: build test

################################################################
#
# Run all tests, checks, generate docs, generate man pages,
# ..., etc.
#
# This should always be run before pushing changes to the
# server.
#
################################################################
.PHONY: full_build # Build all targets, run tests, generate docs, ..., etc.
full_build: cppcheck clean build test man_pages memcheck

################################################################
#
# Run static code checks on C++ code
#
################################################################
.PHONY: cppcheck # Run cppcheck
cppcheck:
	@echo "Running cppcheck..."
	@cppcheck --std=c++17 --language=c++ --enable=all \
		-q --error-exitcode=255 \
		-I . -I spoc -I apps -I laslib/LASlib/inc \
		--inline-suppr \
		--suppress=missingIncludeSystem \
		--suppress=unusedFunction \
		--suppress=useStlAlgorithm \
		--suppress='*:laslib/LASlib/inc/*' \
		apps/*/*.cpp

################################################################
#
# Run cmake when CMakeLists.txt changes
#
################################################################
./build/debug/Makefile: CMakeLists.txt
	@echo "Running cmake..."
	@mkdir -p build/debug
	@mkdir -p build/release
	@cd build/debug && cmake -DCMAKE_BUILD_TYPE=Debug ../..
	@cd build/release && cmake -DCMAKE_BUILD_TYPE=Release ../..

.PHONY: laslib # Build LASlib library
laslib:
	$(MAKE) -j -C laslib/LASlib

################################################################
#
# Compile applications and tests
#
################################################################
.PHONY: build # Compile all applications and tests
build: laslib ./build/debug/Makefile
	cd build/debug && make -j 8
	cd build/release && make -j 8

################################################################
#
# Delete all build objects
#
################################################################
.PHONY: clean # Clean build objects
clean:
	@echo "Cleaning..."
	@rm -rf build
	@$(MAKE) -C laslib/LASlib clean

################################################################
#
# Tests
#
################################################################
.PHONY: unit_and_app_test
unit_and_app_test: BUILD=debug
unit_and_app_test:
	@parallel --jobs 24 --halt now,fail=1 "echo {} && {}" ::: build/$(BUILD)/test_*

.PHONY: integration_test
integration_test: BUILD=debug
integration_test:
	@parallel --jobs 24 --halt now,fail=1 \
		"echo $(BUILD): {} && PATH=./build/$(BUILD)/:$$PATH {}" ::: tests/integration/test_*.sh

.PHONY: test # Run tests
test:
	@echo "Testing..."
	@$(MAKE) --no-print-directory unit_and_app_test BUILD=debug
	@$(MAKE) --no-print-directory unit_and_app_test BUILD=release
	@$(MAKE) --no-print-directory integration_test BUILD=debug
	@$(MAKE) --no-print-directory integration_test BUILD=release

################################################################
#
# Run valgrind to check for memory leaks and other issues
#
################################################################
.PHONY: memcheck # Run memcheck
memcheck:
	@echo "Running memchecks..."
	@parallel --jobs 24 --halt now,fail=1 \
		"echo {} && valgrind --leak-check=full --error-exitcode=1 --quiet --suppressions=valgrind.suppressions {}" ::: build/debug/test_*
	@parallel --jobs 24 --halt now,fail=1 \
		"echo {} && valgrind --leak-check=full --error-exitcode=1 --quiet --suppressions=valgrind.suppressions {}" ::: build/release/test_*

################################################################
#
# Documentation
#
################################################################
.PHONY: man_pages # Generate man pages
man_pages:
	@mkdir -p build/man
	@find apps/*/*.md | xargs --verbose -P 8 -I {} bash -c 'pandoc -s -t man {} -o build/man/`basename {} .md`'

################################################################
#
# Install applications and man pages
#
# For this local installation to work, you must ensure that
# ~/.local/bin is in your path.
#
# To access the man pages, you also must ensure that
# ~/.local/share/man is in your manpath. You can check your
# manpath with the `manpath` command.
#
################################################################
.PHONY: install_local # Install applications and man pages to ~/.local directory
install_local: build man_pages
	@mkdir -p $$HOME/.local/bin
	@echo Installing applications to $$HOME/.local/bin
	@install ./build/release/las2spoc  $$HOME/.local/bin
	@install ./build/release/spoc2las  $$HOME/.local/bin
	@install ./build/release/spoc2text $$HOME/.local/bin
	@install ./build/release/text2spoc $$HOME/.local/bin
	@mkdir -p $$HOME/.local/share/man/man1
	@echo Copying man pages to $$HOME/.local/share/man/man1
	@cp ./build/man/*.1 $$HOME/.local/share/man/man1

.PHONY: help # Generate list of targets with descriptions
help:
	@grep '^.PHONY: .* #' Makefile | sed 's/\.PHONY: \(.*\) # \(.*\)/\1	\2/' | expand -t20
