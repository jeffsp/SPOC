# Spocpy Makefile

SHELL=/bin/bash

##############################################################################
.PHONY: default
default: build

.PHONY: build # Build all modules
build:
	@python setup.py build

##############################################################################
.PHONY: install # Install the extension into the venv
install: build
	pip install .

##############################################################################
.PHONY: test # Run all tests
test:
	@cd tests && python -m unittest -v *.py

##############################################################################
.PHONY: examples # Run all examples
examples:
	@cd examples && parallel python {} ::: *.py

##############################################################################
.PHONY: everything # Clean, build, install, test, run examples
everything: clean build install test examples

##############################################################################
.PHONY: clean # Remove all intermediate files
clean:
	rm -rf build *.so spocpy.egg-info __pycache__ */__pycache__

##############################################################################
.PHONY: help # Generate list of targets with descriptions
help:
	@grep '^.PHONY: .* #' Makefile | sed 's/\.PHONY: \(.*\) # \(.*\)/\1	\2/' | expand -t20

# vim: set spell tw=78:
