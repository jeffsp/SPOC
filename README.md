# Introduction

SPOC is the Simple POint Cloud format used to store 3D geospatial data.

A SPOC file consists of a **HEADER** followed by **POINT RECORDS**.

The **HEADER** contains a SPOC file identifier, the spoc format version
numbers, the number of points records contained in the file, and an
arbitrary length data string that can be used to store spatial reference
system data.

Each **POINT RECORD** in a SPOC file contains an X, Y, and Z coordinate
stored as 64-bit precision floating point numbers, as well as other
associated data fields, like red, green, and blue color information,
classification information, point identifier, intensity information, or
arbitrary integer values.

# Description of SPOC Format

The proposed extension is SPOC, or Simple POint Cloud.

* Points are stored as a vector of point records
* The order of the point records is preserved
* Point locations are stored as 64-bit double precision numbers, so
  about 16 decimal digits of precision is preserved, assuming an IEEE
  754 standard.
* Each point has associated 16- and 32-bit fields that are common to 3D
  point collections, like red, green, blue, intensity, collection ID,
  classification, and arbitrary 64-bit unsigned integer side data.

# Requirements

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

## OS Support

This project is defined for Linux-based systems, including MAC OS and
the Windows Linux subsystem.

# TODO

- [ ] Reorganize namespaces and headers
- [X] Create header only library with examples
- [X] Read/write uncompressed
- [X] Create compression utilities read/write compressed files
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
  - [ ] Multi-pass tiling for large files
  - [X] Unit/integration tests
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
  - [ ] Subsample: Remove duplicates with same voxel indexes in extra[0,1,2]
        fields, subsample=K: keep N duplicates

- [ ] spoc\_generate: Generate values for each point
  - [ ] Write to stdout in text format
  - [ ] Write to extra[0..N]
  - [ ] Generate 2D grid indexes for each point
    - [ ] Grid size (resolution)
    - [ ] Grid size in X, Y
    - [ ] Specify a projection plane, XY, XZ, YZ
  - [ ] Generate 3D voxel indexes for each point
    - [ ] Voxel size (resolution)
    - [ ] Voxel size in X, Y, Z

- [ ] spoc\_project: Project points onto a plane
  - [ ] grid indexes are stored in extra[0..N]
  - [ ] pixel data type, int/float, 8/16/32/64
  - [ ] pixel size in m/pixel
  - [ ] Specify a projection plane, XY, XZ, YZ
  - [ ] Specify a projection plane, normal=x,y,z
  - [ ] Direction of projection, normal=x,y,z
  - [ ] nodata value
  - [ ] field: z, c, p, i, r, g, b, 0-7
  - [ ] normalize output
  - [ ] z-score output
  - [ ] min/max/%quantile
  - [ ] randomly select/vote
  - [ ] geotiff output
  - [ ] png output

- [ ] spoc\_tool: Common operations that do not stream
  - [ ] Get/Set XYZ
    - [ ] text/binary
  - [ ] Get/Set field F
    - [ ] text/binary
    - [ ] integer/float
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
    - [ ] Rotate, project, etc.
    - [ ] Regularization, add noise
    - [ ] Tile
  - [ ] Decimate/undecimate
  - [ ] Transform with pipes
  - [X] Stream averaging

- [ ] C++ Examples
  - [X] Noop
  - [X] Very large spoc file with streaming
  - [ ] Nearest neighbor processing
