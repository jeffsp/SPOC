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

- [X] Add Doxygen support
- [X] Add Design by Contract functionality
- [X] Generate documentation
  - [ ] Manual
  - [X] SDK
- [ ] Reorganize namespaces and headers
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
- [X] Integration test suite
- [X] Man page generation
- [X] Coverage
- [X] Cppcheck
- [X] read/write spoc files
- [X] read/write las files
- [X] Add --version option to all apps

- [X] spoc info
  - [X] Header/summary
  - [X] Unit/integration tests
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
  - [ ] Tile in one direction only to get a transect slice
- [X] spoc merge: Combine several point clouds into one
  - [X] Set point id
  - [X] Warn if the SRS info strings differ
  - [X] Warn if the area of the merged file is too big
  - [X] Quiet (don't warn)
  - [X] Unit/integration tests

- [X] spoc\_transform: These are all capable of streaming. The output
      has the same number of points as the input
  - [X] Only allow one command at a time, multiple commands can be
        executed using pipes
  - [X] Allow string field specifications for extra[0..N]
  - [X] Set fields: set f #
  - [X] Replace fields: replace f # #
  - [X] Quantize: round X, Y, Z to nearest Nth decimal place
  - [X] Rotate by N degrees about X/Y/Z axis: rotatex/y/z #
  - [X] Add offset to X,Y,Z: addx/y/z #
  - [X] Scale by X,Y,Z: scale, scalex/y/z

- [ ] spoc\_filter: Remove point records from a spoc file
  - [ ] Remove when field f==, <=, >=, <, > value
  - [ ] Keep when field f==, <=, >=, <, > value
  - [ ] Unique: Remove duplicates with same X, Y, Z values
  - [ ] Subsample: Remove duplicates with same voxel indexes
    - [ ] Use voxel indexes in extra[0,1,2] fields
    - [ ] Voxel size (resolution)
    - [ ] Voxel size in X, Y, Z
    - [ ] subsample=K: keep K duplicates

- [ ] spoc\_project: Project points onto a 2D plane. In order to project
                     onto XZ or YZ or an arbitrary plane, first rotate the
                     point cloud, then project.
  - [ ] Grid size (resolution)
  - [ ] Field: z, c, p, i, r, g, b, extra[0..N]
  - [ ] Z elevation = min/max/median/mode/avg/%centile
  - [ ] normalize output
  - [ ] z-score output
  - [ ] pixel data type, int/uint/float, 8/16/32/64
  - [ ] geotiff output
    - [ ] nodata value
  - [ ] png output

- [ ] spoc\_tool: Common operations that do not stream
  - [ ] Get/Set field F as text + integration test
  - [X] Recenter points about mean
  - [X] Subtract minimum X, Y, and Z from all points: subtract-min
  - [ ] Use the voxel indexes for aligning points when
        the point clouds contain a different number of points.
        1. Generate voxel indexes 2. subsample using those indexes. 3. restore
        the points using the original voxel indexes
        Note that this is useful for restoring points to a point cloud
        that has been subsampled (decimated).
  - [ ] 2D/3D field smoothing - does not change xyz coords
        smooth f sigma
  - [ ] Spatial smoothing - 2D/3D spatial filtering, changes 3D structure
                          - Gaussian filter
                          - Median filter
                          - X, Y, Z

- [ ] spoc config: Show configuration values
  - [ ] ~/.config/spoc/config
  - [ ] ~/.config/spoc/palettes

- [ ] spoc octree: break into files arranged as an octree, access/create spoc files
  - [ ] Given a bunch of spoc files, create a quadtree structure
  - [ ] Don't divide on z value (create quadtrees)
  - [ ] Create a spoc file from an octree given an extent
  - [ ] Unit/integration tests

- [ ] C++ SPOC Utility Header Functions
  - [ ] Set random seed
  - [ ] Add random Gaussian noise to X,Y,Z
  - [ ] Add random uniform noise to X,Y,Z
  - [ ] Color by classification
  - [ ] Color by elevation
  - [ ] Color by segment
  - [ ] Get/set palette
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

- [ ] Script Examples
  - [ ] Machine learning
    - [ ] Rotate by 5 degrees off nadir, then project to XY plane
    - [ ] Slice a 1 meter transect, rotate by 90 degrees, project to get
          a transect
    - [ ] Regularization, add noise
    - [ ] Tile
  - [ ] Decimate/undecimate
  - [ ] Transform with pipes
  - [X] Stream averaging

- [ ] C++ Examples
  - [X] Noop
  - [X] Very large spoc file with streaming
  - [ ] Nearest neighbor processing
