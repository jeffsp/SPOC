default: build

# Run cmake when CMakeLists.txt changes
./build/debug/Makefile: CMakeLists.txt
	@echo "Running cmake..."
	@mkdir -p build/debug
	@mkdir -p build/release
	@cd build/debug && cmake -DCMAKE_BUILD_TYPE=Debug ../..
	@cd build/release && cmake -DCMAKE_BUILD_TYPE=Release ../..

.PHONY: build # Build application
build: ./build/debug/Makefile
	cd build/debug && make -j 24
	cd build/release && make -j 24

.PHONY: clean # Clean build objects
clean:
	rm -rf build

.PHONY: test # Run tests
test:
	./build/debug/test_spoc_viewer
	./build/release/test_spoc_viewer

.PHONY: memcheck # Run memcheck
memcheck:
	valgrind --leak-check=full --error-exitcode=1 --quiet ./build/debug/test_spoc_viewer
	valgrind --leak-check=full --error-exitcode=1 --quiet ./build/release/test_spoc_viewer

################################################################
#
# Documentation
#
################################################################
.PHONY: man_pages # Generate man pages
man_pages:
	@mkdir -p build/man
	@pandoc -s -t man ./spoc_viewer.1.md -o build/man/spoc_viewer.1

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
	@install ./build/release/spoc_viewer $$HOME/.local/bin
	@mkdir -p $$HOME/.local/share/man/man1
	@echo Copying man pages to $$HOME/.local/share/man/man1
	@cp ./build/man/spoc_*.1 $$HOME/.local/share/man/man1


.PHONY: help # Generate list of targets with descriptions
help:
	@grep '^.PHONY: .* #' Makefile | sed 's/\.PHONY: \(.*\) # \(.*\)/\1	\2/' | expand -t20
