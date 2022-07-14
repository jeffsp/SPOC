# Introduction

SPOC is a Simple POint Cloud format used to store 3D geospatial data.

# File format

A SPOC file consists of a **HEADER** followed by **POINT RECORDS**.

The **HEADER** contains the following information:

* SPOC file identifier
* SPOC format major and minor version numbers
* An arbitrary length character string that can be used to store
  spatial reference system data
* The number of 64-bit extra fields associated with each point record
* The number of points records contained in the file
* A flag indicating whether or not the contents are compressed

Each **POINT RECORD** in a SPOC file contains:

* X, Y, and Z coordinates stored as 64-bit precision floating point numbers
* 32-bit classification
* 32-bit point ID
* 16-bit intensity/NIR information
* 16-bit red channel information
* 16-bit green channel information
* 16-bit blue channel information
* Zero or more 64-bit extra fields

Points are stored in SPOC file as a vector of point records.

The order of point records is preserved.

Point coordinates are stored as 64-bit double precision numbers, so
about 16 decimal digits of precision is preserved, assuming an IEEE 754
standard.

# Design

* 64-bit double precision
* Linear complexity algorithms
    * Exception: Quantiles in `spoc_info`
    * Exception: Nearest neighbor is linear in number of neighbors, not
      number of points
* Functional programming style: avoid OOP
* Implicit streaming support
    * Streaming is provided by anonymous and named pipes
    * Point record ordering is always preserved
    * LASlib does not support streaming
* Avoid dependencies. Current dependencies are:
    * zlib for compression
    * OpenMP for parallelization
    * laslib for las/spoc translation utilities
* SPOC makes assumptions about the byte ordering, and is therefore not
  portable
* This project is defined for Linux-based systems, including MAC OS and
  the Windows Linux subsystem.

# Project Roadmap

## Framework

- [X] Add Doxygen support
- [X] Add Design by Contract functionality
- [X] Generate documentation
  - [ ] Manual
  - [X] SDK
- [ ] Reorganize namespaces and headers
  - [ ] Separate application headers from API headers
  - [ ] Create separate application namespace
- [ ] Reorganize tests
  - [ ] Separate app tests from unit tests
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
- [X] Test data files
- [X] Unit test suite
- [ ] Application test suite
  - [ ] Separate from unit test suite
  - [ ] Include in coverage and memcheck
- [X] Integration test suite
  - [X] Shell commands that manipulate PATH variable
- [X] Man page generation
- [X] Coverage
- [X] Cppcheck
- [X] read/write spoc files
- [X] read/write las files
- [X] Add --version option to all apps

## Applications

- [X] spoc info
  - [X] Header/summary
  - [X] Unit/integration tests
  - [ ] By default, don't show quantiles to avoid sorting
  - [ ] Flag to turn on quantile summary, just show min/max instead

- [X] spoc spoc2text/text2spoc
  - [X] Unit/integration tests

- [X] spoc compress/decompress
  - [ ] Add --precision argument to compress to improve compression?
  - [ ] Revisit command line input/output file specifications, support pipes?

- [X] spoc spoc2las/las2spoc
  - [X] Unit/integration tests

- [X] spoc srs: Get/set SRS
  - [X] Unit/integration tests

- [X] spoc diff: diff two point clouds, return error if different
  - [X] Header only
  - [X] Data only
  - [X] Field F only - x, y, z, c, p, i, r, g, b, 0-7
  - [X] Not - inverse results
  - [X] Unit/integration tests

- [X] spoc tile: Tile into regular non-overlapping tiles.
  - [X] Number of tiles on largest size
  - [X] Size of tile on one side
  - [ ] Multi-pass tiling for large files
  - [X] Unit/integration tests
  - [ ] Tile along a single axis in order to get transect slices

- [X] spoc merge: Combine several point clouds into one
  - [X] Set point id
  - [X] Warn if the SRS info strings differ
  - [X] Warn if the area of the merged file is too big
  - [X] Quiet (don't warn)
  - [X] Unit/integration tests

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
  - [ ] Add Gaussian noise to XYZ
    - [ ] Set seed
  - [ ] Add Uniform noise to XYZ
    - [ ] Set seed

- [ ] spoc color: Set RGB values
  - [ ] Transform classification to RGB
  - [ ] Transform elevation to RGB
  - [ ] Transform e# to RGB
  - [ ] Read palette
  - [ ] Random palette=size
  - [ ] Random seed

- [ ] spoc tool: Common operations
  - [X] Get/Set field F as text
    - [ ] Check to make sure text file has the correct number of points
          when setting a field
  - [X] Recenter points about mean
  - [X] Subtract minimum X, Y, and Z from all points: subtract-min
  - [ ] Resize extra
  - [ ] Copy field --src=F --dest=F

- [ ] spoc filter: Remove points with certain properties
  - [ ] Remove points when field f==, !=, <=, >=, <, > value
    - [ ] Don't allow == on doubles
  - [ ] Unique: Remove duplicates with same X, Y, Z values
  - [ ] Downsample: Remove duplicates with same voxel indexes
    - [ ] voxel resolution
    - [ ] voxel resolution X, Y, Z
    - [ ] samples=K: keep K samples from each voxel

- [ ] spoc restore: Restore points from one point cloud to another with
      the same voxel indexes

## C++ Utility Header Functions

  - [ ] Set random seed
  - [ ] Generate voxel indexes
  - [ ] Generate grid indexes
  - [ ] Generate connected component IDs based upon location and, optionally, other fields
    - [ ] Connection radius
    - [ ] Connection field(s)
  - [ ] Generate cluster IDs based upon data fields, xyz, cpi, rgb, extra[n]:
    - [ ] Save component ID to extra[n]
    - [ ] Set value of K
    - [ ] Use K means clustering
    - [ ] Use spectral clustering
    - [ ] Use Newman clustering
  - [ ] Generate neighbor indexes within a radius
    - [ ] Save the nearest K neighbors
    - [ ] Randomly select K neighbors within the radius
  - [ ] 2D/3D field smoothing - does not change xyz coords
        smooth f sigma
  - [ ] Spatial smoothing - 2D/3D spatial filtering, changes 3D structure
                          - Gaussian filter
                          - Median filter
                          - X, Y, Z

## Problems

How do we handle projection? This is a raster operation, not a point
cloud operation.

  - [ ] Projection: Project points onto a 2D plane. In order to project
                    onto XZ or YZ or an arbitrary plane, first rotate the
                    point cloud, then project.
    - [ ] Grid size (resolution)
    - [ ] Field: z, c, p, i, r, g, b, extra[0..N]
    - [ ] Z elevation = min/max/median/mode/avg/%centile
    - [ ] Normalize output
    - [ ] Z-score output
    - [ ] Pixel data type, int/uint/float, 8/16/32/64
    - [ ] Interpolate results: Use after filtering ground points to
          create DTMs
      - [ ] Extrapolate edges on/off
    - [ ] Geotiff output
      - [ ] Nodata value
    - [ ] Png output
      - [ ] Grayscale/RGB

# Script Examples

Examples of how to use the command line applications

- [ ] Machine learning
  - [ ] Rotate by 5 degrees off nadir, then project to XY plane
  - [ ] Slice a 1 meter transect, rotate by 90 degrees, project to get
        a transect
  - [ ] Regularization, add noise
  - [ ] Tile
- [ ] Decimate/undecimate
- [ ] Transform with pipes
- [X] Stream averaging

# C++ Programming Examples

Examples of how to use the C++ API with compiled examples

- [X] Noop a with streams
- [X] Very large spoc file with streaming
- [ ] Nearest neighbor processing

# Proposed Functionality

* spoc octree: break into files arranged as an octree, access/create spoc files
  * Given a bunch of spoc files, create a quadtree structure
  * Don't divide on z value (create quadtrees)
  * Create a spoc file from an octree given an extent
  * Unit/integration tests

