# Introduction

This repository contains library code and applications for reading,
writing, and processing Simple POint Cloud, or SPOC, format files.

SPOC format files are used to store 3D geospatial data.

This format was developed in response to the need to manipulate and
store point cloud data for the purposes of point cloud analysis, in
particular with respect to machine learning algorithm design and
development.

Support for the following features were prioritized when developing this
format:

* 64-bit floating point coordinates for supporting datasets with high
  dynamic ranges

* Streaming file I/O for supporting command chaining using named and
  anonymous pipes

* Variable length inline point record extra fields for assigning
  arbitrary point attributes

The following should also be noted with regards to library support for
the SPOC file format

* Spatial reference system strings are stored in the header and
  maintained during file I/O; however, these strings are never parsed or
  used to process point record data by this library

* Raster products, like GeoTIFF, Esri Grid, or PCI Gemomatics are not
  supported by this library

* Vector products, like ESRI shapefiles, GeoJSON, Google Keyhole Markup
  (KML), or OpenStreetMap data are not supported by this library

* SPOC format files are very similar to ASPRS LAS format files, with the

  notable exceptions listed above: LAS files only support 32-bit
  floating point coordinates, do not easily support streaming I/O, and
  do not support inline variable length records


# Troubleshooting

## Invalid spoc file format

When running the integration tests, if you receive an error "invalid
spoc file format", it means that git's Large File Storage (LFS)
support is not properly installed.

To install it:

1. Install **git-lfs** with your system's package manager. For example,
    `sudo dnf install git-lfs`
1. Install support for your specific project by first changing
   directories to the git project root, and then running `git lfs install`
1. Fetching all LFS files with `git lfs fetch --all`
1. Restoring LFS files with `git lfs checkout`


# support

[https://gitlab.com/jeffsp/spoc_viewer](https://gitlab.com/jeffsp/spoc_viewer)

[https://gitlab.com/jeffsp/spoc_conversion](https://gitlab.com/jeffsp/spoc_conversion)


# file format

a spoc file consists of a **header** followed by **point records**.

the **header** contains the following information:

* a spoc file identifier consisting of the four characters: 'spoc'
* an 8-bit unsigned integer specifying the spoc file format major version
* an 8-bit unsigned integer specifying the spoc file format minor version
* a 16-bit unsigned integer specifying the length of the ogc wkt string
* an arbitrary length ogc wkt string which is used to store spatial
  reference data
* an 8-bit unsigned integer specifying the number of 64-bit extra fields
  associated with each point record
* a 64-bit unsigned integer specifying the number of points records
  contained in the file
* a 8-bit unsigned integer flag indicating whether or not the contents
  are compressed

| data type     | contents          | notes |
| ---           | ---               | ---   |
| uint8[0..3]   | 'spoc'            | 4 byte file identifier |
| uint8         | major version     | file format information |
| uint8         | minor version     | file format information |
| uint16        | ogc wkt length    | number of bytes in the next field |
| uint8[0..n-1] | ogc wkt           | arbitrary, possibly zero-length string containing ogc wkt format spatial reference data |
| uint8         | extra fields      | number of 64-bit unsigned extra fields in each record |
| uint64        | total points      | total point records in the spoc file |
| uint8         | compression flag  | indicates if the file contents are compressed |

each **point record** in a spoc file contains the following information:

* a 64-bit floating point x coordinate
* a 64-bit floating point y coordinate
* a 64-bit floating point z coordinate
* a 32-bit unsigned integer classification
* a 32-bit unsigned integer point id
* a 16-bit unsigned integer intensity/nir value
* a 16-bit unsigned integer red channel value
* a 16-bit unsigned integer green channel value
* a 16-bit unsigned integer blue channel value
* zero or more 64-bit unsigned integer extra fields

| data type      | contents          | notes |
| ---            | ---               | ---   |
| double         | x coordinate      | units are specified in the ogc wkt string |
| double         | y coordinate      | units are specified in the ogc wkt string |
| double         | z coordinate      | units are specified in the ogc wkt string |
| uint32         | classification    | classification as defined by the asprs |
| uint32         | point indentifier | a point id, which can indicate the collection source or any other appropriate value |
| uint16         | intensity/nir     | an intensity or nir value |
| uint16         | red               | the point's red channel value |
| uint16         | green             | the point's green channel value |
| uint16         | blue              | the point's blue channel value |
| uint64[0..n-1] | extra fields      | zero or more extra fields, as indicated in the spoc file header |

# applications

applications provided by this repository adhere to these design
principles:

* applications are command line oriented and are designed to run on a
  linux, macos, or windows subsystem for linux terminal
* where possible, always preserve point record ordering
* implicit streaming support by reading/writing records to a stream
  where possible. this will improve performance by allowing piped commands
  to operate in parallel. see examples below.
* avoid dependencies. current dependencies are:
  * zlib for compression
  * openmp for parallelization

the library api follows these principles:

* stl conformance where appropriate
* functional programming style. use oop sparingly and with strong
  justification.
* where possible, point records are stored in a vector, and functions
  manipulate point record vector indexes, rather than manipulating the
  point records themselves
* most functions provide the strong exception safety guarantee by using
  call-by-const-reference or call-by-value and returning either indexes
  into the input data or copies of the input data.
* linear complexity algorithms
  * exception: quantiles in `spoc_info` require point record sorting
  * exception: nearest neighbor algorithms are linear in the number of neighbors, not
      number of points
* this library currently makes assumptions about the byte ordering, and
  it is therefore not portable to other architectures

# project roadmap

this roadmap lists the target features and functionality and their
current development status.

## framework

- [ ] create build and deploy containers
- [x] add i/o benchmarks
- [x] add point record get/set benchmarks
- [ ] add compression benchmarks
- [x] update warning for ogc wkt to be more explicit
- [x] add doxygen support
- [x] add design by contract functionality
- [x] generate documentation
  - [ ] command line application manual
  - [x] sdk
- [x] reorganize namespaces and headers
  - [x] separate application headers from api headers
  - [x] create separate application namespace
- [x] reorganize tests
  - [x] separate app tests from unit tests
- [x] create header only library with examples
- [x] read/write uncompressed
- [x] add nextra field in header
- [x] allow arbitrary number of extra fields
- [x] rename 'extra\_size' -> 'extra\_fields'
- [x] create interface to spoc\_file, so header file size and point
      vector size do not get out of line. same for 'extra_size'
- [x] gitlab runner
- [x] ci/cd
- [x] version number in header
- [x] memcheck with valgrind
- [x] test data files
- [x] unit test suite
- [x] application test suite
  - [x] separate from unit test suite
  - [x] include in coverage and memcheck
- [x] integration test suite
  - [x] shell commands that manipulate path variable
- [x] man page generation
- [x] coverage
- [x] cppcheck
- [x] read/write spoc files
- [x] read/write las files
- [x] add --version option to all apps

## python interface

- [x] python extension reader/writer using pybind11
- [x] numpy/pandas helpers in python extension
- [x] simple python examples
- [ ] simple spoc file viewer using plotly

## applications

- [x] speed up i/o by either increasing the read/write buffer sizes or
      by manually buffering reads and writes.

- [x] spoc info
  - [x] header/summary
  - [x] unit/integration tests
  - [x] by default, don't show quantiles to avoid sorting
  - [x] flag to turn on quantile summary, just show min/max instead
  - [x] read compressed files
  - [ ] add metrics
    - [x] points/m^2 (extent)
    - [x] points/m^2 (grid cell)
    - [x] points/m^3 (voxel)
    - [ ] point spacing estimate

- [x] spoc spoc2text/text2spoc
  - [x] unit/integration tests
  - [x] read compressed files

- [x] spoc compress/decompress

- [x] spoc spoc2las/las2spoc
  - [x] unit/integration tests
  - [x] support streaming for large files
  - [x] read compressed files

- [x] spoc srs: get/set srs
  - [x] unit/integration tests

- [x] spoc diff: diff two point clouds, return error if different
  - [x] header only
  - [x] data only
  - [x] field f only - x, y, z, c, p, i, r, g, b, 0-7
  - [x] not - inverse results
  - [x] unit/integration tests
  - [x] read compressed files

- [x] spoc tile: tile into regular non-overlapping tiles.
  - [x] number of tiles on largest size
  - [x] size of tile on one side
  - [x] unit/integration tests
  - [x] tile along a single axis in order to get transect slices
  - [x] read/write compressed files

- [x] spoc merge: combine several point clouds into one
  - [x] set point id
  - [x] warn if the srs info strings differ
  - [x] warn if the area of the merged file is too big
  - [x] quiet (don't warn)
  - [x] unit/integration tests
  - [x] read/write compressed files

- [x] spoc transform: transform each point record into a different point
      record. these are all capable of streaming. the output has the
      same number of points as the input. the ordering of the points
      does not change.
  - [x] only allow one command at a time, multiple commands can be
        executed using pipes
  - [x] allow string field specifications for extra[0..n]
  - [x] set fields: set f #
  - [x] replace fields: replace f # #
  - [x] quantize: round x, y, z to nearest nth decimal place
  - [x] rotate by n degrees about x/y/z axis: rotatex/y/z #
  - [x] add offset to x,y,z: addx/y/z #
  - [x] scale by x,y,z: scale, scalex/y/z
  - [x] set random seed
  - [x] add gaussian noise to xyz
    - [x] sigma=#
    - [x] sigma-xyz=#,#,#
  - [x] add uniform noise to xyz
    - [x] size=#
    - [x] size-xyz=#,#,#
  - [x] copy field --src=f --dest=f
  - [ ] add --precision-x, --precision-y, --precision-z arguments to
        compress in order to improve compression

- [x] spoc tool: common operations
  - [x] get/set field f as text
    - [x] check to make sure text file has the correct number of points
          when setting a field
  - [x] recenter points about mean
  - [x] subtract minimum x, y, and z from all points: subtract-min
  - [x] resize extra
  - [x] upsample classifications
    - [x] resolution
  - [x] refactor: this application does not need to stream
  - [x] read/write compressed files

- [x] spoc filter: remove points with certain properties
  - [x] keep classes
  - [x] remove classes
  - [x] unique: remove duplicates with same x, y, z values
  - [x] subsample: remove duplicates with same voxel indexes
    - [x] random-seed=*#*
    - [x] save-voxel-indexes in e0,e1,e2
  - [x] unit/app tests
  - [x] read/write compressed files

## script examples

examples of how to use the command line applications

- [ ] machine learning
  - [ ] rotate by 5 degrees off nadir, then project to xy plane
  - [ ] slice a 1 meter transect, rotate by 90 degrees, project to get
        a transect
  - [ ] regularization, add noise
  - [ ] tile
- [ ] subsample/upsample
- [ ] transform with pipes
- [x] stream averaging

## c++ programming examples

examples of how to use the c++ api with compiled examples

- [x] noop with streams
- [x] very large spoc file with streaming
- [ ] nearest neighbor processing
- [ ] dsm creation
  - [ ] get voxel indexes
  - [ ] project to xy plane
  - [ ] write raw raster to disk
  - [ ] convert raw to png using imagemagick
- [ ] subsample with random indexes, assign class, undo subsampling

# proposed functionality

* spoctree: break into files arranged as an octree, access/create spoc files
  * given a bunch of spoc files, create a spoctree file. each entry in the spoctree file should contain:
    * filename
    * extent
    * md5sum
  * spoctree is immutable (i.e. can't add to or delete from spoctree)
  * octree cells divide until they reach a minimum size, based upon point record size (e.g. 1mb per cell)
  * generate a spoc file from a spoctree given an extent
    * api call
    * application interface
    * x,y extent
    * x,y,z extent
  * unit/integration tests

# a final note about library design

this library adheres to design by contract which is implemented using
macros and are enabled during debug compilation. in the release version
of the build, the macros have no effect (i.e., they are no-ops).

preconditions are specified typically upon function entry with the macro
**require()**.

postconditions are specified before function return with the macro
**ensure()**.

invariants are specified throughout the code with the **assert()** macro.

note that design by contract macros are used during development and aid
in code **design**. they are not used for debugging or for error
checking.

as noted above, side effects are typically reduced by adhering to a
function style of programming. also, the **strong** exception safety
guarantee is often provided by passing parameters that are const
reference and by using copy semantics when performance allows it.

# references

https://www.boost.org/community/exception\_safety.html

https://en.cppreference.com/w/cpp/language/copy\_elision

https://stackoverflow.com/questions/21471836/what-can-c-offer-as-far-as-functional-programming

https://stackoverflow.com/questions/21605579/how-true-is-want-speed-pass-by-value
(note that david abraham's original article, "want speed? pass by
value", at the time of this writing, does not appear to have a
persistent url, and cpp-next.com is defunct.

https://en.wikipedia.org/wiki/lambda\_lifting
