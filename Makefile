default: compile test

cmake:
	@mkdir -p build/debug
	@mkdir -p build/release
	@cd build/debug && cmake -DCMAKE_BUILD_TYPE=Debug ../..
	@cd build/release && cmake -DCMAKE_BUILD_TYPE=Release ../..

.PHONY: convert # Convert LAS files to SPC files
convert:
	./convert_all.sh ./datasets/las_files ./results/spc_file_format

.PHONY: compile # Compile all applications
compile:
	@cd build/debug && make -j 24
	@cd build/release && make -j 24

.PHONY: clean # Clean build objects
clean:
	@rm -rf build

.PHONY: run # Run app
run:
	./build/debug/text2spc -v < ./results/spc_file_format/Raqqah.txt > Raqqah.spc
	./build/debug/spc2text -v < Raqqah.spc > Raqqah.txt

.PHONY: test # Run tests
test:
	./build/debug/test_octree
	./build/debug/test_zlib
