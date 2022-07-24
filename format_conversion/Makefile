default: help

.PHONY: convert_laz # Convert LAS files to LAZ files
convert_laz:
	@./convert_laz_all.sh \
		$$(realpath ./datasets) \
		$$(realpath ./results)

.PHONY: convert_spoc # Convert LAS files to SPOC files
convert_spoc:
	@PATH=$$(pwd)/../build/release:$$PATH && \
		./convert_spoc_all.sh \
		$$(realpath ./datasets) \
		$$(realpath ./results)

.PHONY: compare # Compare LAZ and SPOC file sizes
compare:
	@./compare_all.sh \
		$$(realpath ./datasets) \
		$$(realpath ./results)

.PHONY: help # Generate list of targets with descriptions
help:
	@grep '^.PHONY: .* #' Makefile | sed 's/\.PHONY: \(.*\) # \(.*\)/\1	\2/' | expand -t20
