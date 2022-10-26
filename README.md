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


# Support

[https://gitlab.com/jeffsp/spoc_viewer](https://gitlab.com/jeffsp/spoc_viewer)

[https://gitlab.com/jeffsp/spoc_conversion](https://gitlab.com/jeffsp/spoc_conversion)


# File format

A SPOC file consists of a **HEADER** followed by **POINT RECORDS**.

The **HEADER** contains the following information:

* A SPOC file identifier consisting of the four characters: 'SPOC'
* An 8-bit unsigned integer specifying the SPOC file format major version
* An 8-bit unsigned integer specifying the SPOC file format minor version
* A 16-bit unsigned integer specifying the length of the OGC WKT string
* An arbitrary length OGC WKT string which is used to store spatial
  reference data
* An 8-bit unsigned integer specifying the number of 64-bit extra fields
  associated with each point record
* A 64-bit unsigned integer specifying the number of points records
  contained in the file
* A 8-bit unsigned integer flag indicating whether or not the contents
  are compressed

| Data type     | Contents          | Notes |
| ---           | ---               | ---   |
| uint8[0..3]   | 'SPOC'            | 4 byte file identifier |
| uint8         | Major version     | File format information |
| uint8         | Minor version     | File format information |
| uint16        | OGC WKT length    | Number of bytes in the next field |
| uint8[0..N-1] | OGC WKT           | Arbitrary, possibly zero-length string containing OGC WKT format spatial reference data |
| uint8         | Extra fields      | Number of 64-bit unsigned extra fields in each record |
| uint64        | Total points      | Total point records in the SPOC file |
| uint8         | Compression flag  | Indicates if the file contents are compressed |

Each **POINT RECORD** in a SPOC file contains the following information:

* A 64-bit floating point X coordinate
* A 64-bit floating point Y coordinate
* A 64-bit floating point Z coordinate
* A 32-bit unsigned integer classification
* A 32-bit unsigned integer point ID
* A 16-bit unsigned integer intensity/NIR value
* A 16-bit unsigned integer red channel value
* A 16-bit unsigned integer green channel value
* A 16-bit unsigned integer blue channel value
* Zero or more 64-bit unsigned integer extra fields

| Data type      | Contents          | Notes |
| ---            | ---               | ---   |
| double         | X coordinate      | Units are specified in the OGC WKT string |
| double         | Y coordinate      | Units are specified in the OGC WKT string |
| double         | Z coordinate      | Units are specified in the OGC WKT string |
| uint32         | Classification    | Classification as defined by the ASPRS |
| uint32         | Point indentifier | A point ID, which can indicate the collection source or any other appropriate value |
| uint16         | Intensity/NIR     | An intensity or NIR value |
| uint16         | Red               | The point's red channel value |
| uint16         | Green             | The point's green channel value |
| uint16         | Blue              | The point's blue channel value |
| uint64[0..N-1] | Extra fields      | Zero or more extra fields, as indicated in the SPOC file header |

# Applications

Applications provided by this repository adhere to these design
principles:

* Applications are command line oriented and are designed to run on a
  Linux, MacOS, or Windows Subsystem for Linux terminal
* Where possible, always preserve point record ordering
* Implicit streaming support by reading/writing records to a stream
  where possible. This will improve performance by allowing piped commands
  to operate in parallel. See examples below.
* Avoid dependencies. Current dependencies are:
  * zlib for compression
  * OpenMP for parallelization

The library API follows these principles:

* STL conformance where appropriate
* Functional programming style. Use OOP sparingly and with strong
  justification.
* Where possible, point records are stored in a vector, and functions
  manipulate point record vector indexes, rather than manipulating the
  point records themselves
* Most functions provide the strong exception safety guarantee by using
  call-by-const-reference or call-by-value and returning either indexes
  into the input data or copies of the input data.
* Linear complexity algorithms
  * Exception: Quantiles in `spoc_info` require point record sorting
  * Exception: Nearest neighbor algorithms are linear in the number of neighbors, not
      number of points
* This library currently makes assumptions about the byte ordering, and
  it is therefore not portable to other architectures

# Project Roadmap

This roadmap lists the target features and functionality and their
current development status.

## Framework

- [ ] Create build and deploy containers
- [X] Add I/O benchmarks
- [X] Add point record get/set benchmarks
- [ ] Add compression benchmarks
- [X] Update warning for OGC WKT to be more explicit
- [X] Add Doxygen support
- [X] Add Design by Contract functionality
- [X] Generate documentation
  - [ ] Command line application manual
  - [X] SDK
- [X] Reorganize namespaces and headers
  - [X] Separate application headers from API headers
  - [X] Create separate application namespace
- [X] Reorganize tests
  - [X] Separate app tests from unit tests
- [X] Create header only library with examples
- [X] Read/write uncompressed
- [X] Add NEXTRA field in header
- [X] Allow arbitrary number of extra fields
- [X] Rename 'extra\_size' -> 'extra\_fields'
- [X] Create interface to spoc\_file, so header file size and point
      vector size do not get out of line. Same for 'extra_size'
- [X] Gitlab runner
- [X] CI/CD
- [X] Version number in header
- [X] Memcheck with valgrind
- [X] Test data files
- [X] Unit test suite
- [X] Application test suite
  - [X] Separate from unit test suite
  - [X] Include in coverage and memcheck
- [X] Integration test suite
  - [X] Shell commands that manipulate PATH variable
- [X] Man page generation
- [X] Coverage
- [X] Cppcheck
- [X] read/write spoc files
- [X] read/write las files
- [X] Add --version option to all apps

## Python Interface

- [X] Python extension reader/writer using pybind11
- [X] Numpy/Pandas helpers in Python extension
- [X] Simple Python examples
- [ ] Simple SPOC file viewer using Plotly

## Applications

- [X] Speed up I/O by either increasing the read/write buffer sizes or
      by manually buffering reads and writes.

- [X] spoc info
  - [X] Header/summary
  - [X] Unit/integration tests
  - [X] By default, don't show quantiles to avoid sorting
  - [X] Flag to turn on quantile summary, just show min/max instead
  - [X] Read compressed files
  - [ ] Add metrics
    - [X] Points/m^2 (extent)
    - [X] Points/m^2 (grid cell)
    - [X] Points/m^3 (voxel)
    - [ ] Point spacing estimate

- [X] spoc spoc2text/text2spoc
  - [X] Unit/integration tests
  - [X] Read compressed files

- [X] spoc compress/decompress

- [X] spoc spoc2las/las2spoc
  - [X] Unit/integration tests
  - [X] Support streaming for large files
  - [X] Read compressed files

- [X] spoc srs: Get/set SRS
  - [X] Unit/integration tests

- [X] spoc diff: diff two point clouds, return error if different
  - [X] Header only
  - [X] Data only
  - [X] Field F only - x, y, z, c, p, i, r, g, b, 0-7
  - [X] Not - inverse results
  - [X] Unit/integration tests
  - [X] Read compressed files

- [X] spoc tile: Tile into regular non-overlapping tiles.
  - [X] Number of tiles on largest size
  - [X] Size of tile on one side
  - [X] Unit/integration tests
  - [X] Tile along a single axis in order to get transect slices
  - [X] Read/write compressed files

- [X] spoc merge: Combine several point clouds into one
  - [X] Set point id
  - [X] Warn if the SRS info strings differ
  - [X] Warn if the area of the merged file is too big
  - [X] Quiet (don't warn)
  - [X] Unit/integration tests
  - [X] Read/write compressed files

- [X] spoc transform: Transform each point record into a different point
      record. These are all capable of streaming. The output has the
      same number of points as the input. The ordering of the points
      does not change.
  - [X] Only allow one command at a time, multiple commands can be
        executed using pipes
  - [X] Allow string field specifications for extra[0..N]
  - [X] Set fields: set f #
  - [X] Replace fields: replace f # #
  - [X] Quantize: round X, Y, Z to nearest Nth decimal place
  - [X] Rotate by N degrees about X/Y/Z axis: rotatex/y/z #
  - [X] Add offset to X,Y,Z: addx/y/z #
  - [X] Scale by X,Y,Z: scale, scalex/y/z
  - [X] Set random seed
  - [X] Add Gaussian noise to XYZ
    - [X] Sigma=#
    - [X] Sigma-xyz=#,#,#
  - [X] Add Uniform noise to XYZ
    - [X] Size=#
    - [X] Size-xyz=#,#,#
  - [X] Copy field --src=F --dest=F
  - [ ] Add --precision-x, --precision-y, --precision-z arguments to
        compress in order to improve compression

- [X] spoc tool: Common operations
  - [X] Get/Set field F as text
    - [X] Check to make sure text file has the correct number of points
          when setting a field
  - [X] Recenter points about mean
  - [X] Subtract minimum X, Y, and Z from all points: subtract-min
  - [X] Resize extra
  - [X] Upsample classifications
    - [X] resolution
  - [X] Refactor: This application does not need to stream
  - [X] Read/write compressed files

- [X] spoc filter: Remove points with certain properties
  - [X] Keep classes
  - [X] Remove classes
  - [X] Unique: Remove duplicates with same X, Y, Z values
  - [X] Subsample: Remove duplicates with same voxel indexes
    - [X] random-seed=*#*
    - [X] save-voxel-indexes in e0,e1,e2
  - [X] Unit/app tests
  - [X] Read/write compressed files

## Script Examples

Examples of how to use the command line applications

- [ ] Machine learning
  - [ ] Rotate by 5 degrees off nadir, then project to XY plane
  - [ ] Slice a 1 meter transect, rotate by 90 degrees, project to get
        a transect
  - [ ] Regularization, add noise
  - [ ] Tile
- [ ] Subsample/upsample
- [ ] Transform with pipes
- [X] Stream averaging

## C++ Programming Examples

Examples of how to use the C++ API with compiled examples

- [X] Noop with streams
- [X] Very large spoc file with streaming
- [ ] Nearest neighbor processing
- [ ] DSM creation
  - [ ] Get voxel indexes
  - [ ] Project to XY plane
  - [ ] Write raw raster to disk
  - [ ] Convert raw to png using imagemagick
- [ ] Subsample with random indexes, assign class, undo subsampling

# Proposed Functionality

* spoctree: break into files arranged as an octree, access/create spoc files
  * Given a bunch of spoc files, create a spoctree file. Each entry in the spoctree file should contain:
    * Filename
    * Extent
    * MD5Sum
  * Spoctree is immutable (i.e. can't add to or delete from spoctree)
  * Octree cells divide until they reach a minimum size, based upon point record size (e.g. 1Mb per cell)
  * Generate a spoc file from a spoctree given an extent
    * API call
    * Application interface
    * X,Y extent
    * X,Y,Z extent
  * Unit/integration tests

# A final note about library design

This library adheres to Design by Contract which is implemented using
macros and are enabled during debug compilation. In the release version
of the build, the macros have no effect (i.e., they are no-ops).

Preconditions are specified typically upon function entry with the macro
**REQUIRE()**.

Postconditions are specified before function return with the macro
**ENSURE()**.

Invariants are specified throughout the code with the **assert()** macro.

Note that design by contract macros are used during development and aid
in code **design**. They are not used for debugging or for error
checking.

As noted above, side effects are typically reduced by adhering to a
function style of programming. Also, the **strong** exception safety
guarantee is often provided by passing parameters that are const
reference and by using copy semantics when performance allows it.

# References

https://www.boost.org/community/exception\_safety.html

https://en.cppreference.com/w/cpp/language/copy\_elision

https://stackoverflow.com/questions/21471836/what-can-c-offer-as-far-as-functional-programming

https://stackoverflow.com/questions/21605579/how-true-is-want-speed-pass-by-value
(Note that David Abraham's original article, "Want speed? Pass by
value", at the time of this writing, does not appear to have a
persistent URL, and cpp-next.com is defunct.

https://en.wikipedia.org/wiki/Lambda\_lifting
