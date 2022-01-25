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

.PHONY: run # Run app
run:
	./build/release/text2gpc -v -s -g 100 < ./results/gpc_file_format/Raqqah.txt > Raqqah.gpc
	./build/release/gpc2text -v < Raqqah.gpc > Raqqah.txt
