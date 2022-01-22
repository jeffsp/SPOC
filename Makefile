default: compile run

cmake:
	@mkdir -p build/debug
	@mkdir -p build/release
	@cd build/debug && cmake -DCMAKE_BUILD_TYPE=Debug ../..
	@cd build/release && cmake -DCMAKE_BUILD_TYPE=Release ../..

.PHONY: convert # Convert LAS files to GPC files
convert:
	./convert_all.sh ./datasets/las_files ./results/gpc_file_format

.PHONY: compile # Compile all applications
compile:
	@cd build/debug && make -j 24
	@cd build/release && make -j 24

.PHONY: clean # Clean build objects
clean:
	@rm -rf build

.PHONY: test # Run tests
test:
	./build/debug/test1
	display tmp.ppm
	./build/release/test1
	display tmp.ppm

.PHONY: run # Run app
run:
	./build/debug/overstrike_encoder < hambone.ppm > hambone.hmy
	./build/debug/overstrike_decoder < hambone.hmy > hambone_decoded.ppm
	display hambone.ppm hambone_decoded.ppm

