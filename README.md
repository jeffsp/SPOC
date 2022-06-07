# Introduction

SPOC is the Simple POint Cloud format used to store 3D geospatial data.

A SPOC file consists of a header followed by point records.

The header contains a SPOC file identifier, the spoc format version
numbers, the number of points records contained in the file, and an
arbitrary length data string that can be used to store spatial reference
system data.

Each point record in a SPOC file contains an X, Y, and Z coordinate
stored as 64-bit precision floating point numbers. A point record can
optionally have other associated data fields, like red, green, and blue
color information, classification information, point identifier,
intensity information, or arbitrary integer values.

## Motivation

3D point cloud formats (like LAS) do not provide enough support for
common operations. In particular, we need 64-bit precision for the XYZ
locations, and 64-bit point side data for things like connected
component IDs.

In addition, many of the fields in current point cloud formats (like
LAS) contain support for data that are relevant only to files coming
from Lidar sensors. For example, scan angle, first return, number of
returns, timestamp, ..., etc. These fields contain 'dummy' data when
generated by electro-optical sensors, or SAR sensors.

Furthermore, spatial coordiante system information is not relevvant in
many instances.

## LAS files

The ASPRS LAS file format is used to store geospatial data in the form
of point clouds.

The "LAS" stands for LASer, and means that the file came from a Lidar
system. Many, if not most, LAS files are not generated from Lidar
systems but can be Electro-optical, SAR, ..., etc.

## OS Support

This project is defined for Linux-based systems, including MAC OS and
the Windows Linux subsystem.

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

* Linear complexity
* 64-bit doubles
* Don't rely on data being spatially arranged
* Don't reinvent the wheel: rely on the system's optimization mechanisms
  (disk/memory cache)
* Linux pipes are supported (not for LASlib I/O files)
* Avoid dependencies
    * zlib compression library
    * laslib for las/spoc translation utilities
* Functional programming style: avoid OOP
* SPOC makes assumptions about the byte ordering, and is therefore not
  portable

# TODO

- [X] Create header only library with examples
- [X] Read/write uncompressed
- [X] Create compression utilities read/write compressed files
- [X] Add NEXTRA field in header
- [X] Allow arbitrary number of extra fields
- [ ] Revisit compression rules/defaults for reading and writing
- [ ] Rename 'extra_size' -> 'extra_fields'
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
- [ ] Add --version to all apps
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
- [ ] spoc\_tool
  - [ ] Show progress
  - [ ] Set random seed
  - [X] Rewrite apps so that all I/O streams when possible
  - [X] Commands executed in the order in which they appear on command line
  - [X] Only allow one command at a time, multiple commands can be
        executed using pipes
  - [ ] Copy: Copy fields in one point cloud to fields in another point cloud
    - [ ] Specify reference point cloud
    - [ ] Specify which fields to copy: all,x,y,z,c,p,i,r,g,b,extra
          Note that copying X,Y,Z is useful for unrotating/uncentering
          a point cloud after performing transformations
          The point clouds must have the same number of points
    - [ ] Allow string field specifications for extra[0..N]
    - [ ] Use the voxel indexes in extra[0,1,2] for aligning points when
          the point clouds contain a different number of points.
          1. Generate voxel indexes 2. subsample using those indexes. 3. restore
          the points using the original voxel indexes
          Note that this is useful for restoring points to a point cloud
          that has been subsampled (decimated).
  - [ ] Transformation: Change fields in a point cloud. The output point
        cloud will have the same number of points and the points will be in
        the same order. Transformations occur in the order in which they
        were encounterd on the command line.
    - [X] Allow arbitrary operations using a transformer interface
    - [X] Add support for a two pass filter so that points can be
          changed based upon global point cloud properties, e.g.: add a
          preprocess() function
    - [X] Set fields: set f #
    - [X] Replace fields: replace f # #
    - [X] Recenter points about mean
    - [X] Subtract minimum X, Y, and Z from all points: subtract-min
    - [X] Quantize: round X, Y, Z to nearest Nth decimal place
    - [ ] Get/Set field F as text
    - [ ] Rotate by N degrees about X/Y/Z axis: rotatex/y/z #
    - [ ] Add offset to X,Y,Z: addx/y/z #
    - [ ] Scale by X,Y,Z: scale, scalex/y/z
    - [ ] 2D/3D field smoothing - does not changes xyz coords
          smooth f sigma
    - [ ] Spatial smoothing - 2D/3D spatial filtering, changes 3D structure
                           - Gaussian filter
                           - Median filter
                           - X, Y, Z
    - [ ] Add random Gaussian noise to X,Y,Z
    - [ ] Add random uniform noise to X,Y,Z
    - [ ] Color by classification
    - [ ] Color by elevation
    - [ ] Color by segment
    - [ ] Get/set palette
  - [ ] Filtering: Remove points with certain properties
    - [ ] Remove when field f==, <=, >=, <, > value
    - [ ] Unique / Subsample: Remove points
      - [ ] Uniform random selection of duplicates
      - [ ] Remove duplicates with same X, Y, Z values
      - [ ] Remove duplicates with same voxel indexes in extra[0,1,2] fields
      - [ ] Keep N (default=1) duplicates
    - [X] Allow arbitrary operations using a filter interface and pipes
    - [X] Add support for point removal in filter interface by adding a
          function that accepts a vector of point records and returns a
          filtered vector of point records
  - [ ] Generation: Generate data for each point
    - [ ] Generate grid indexes for each point
      - [ ] Specify a projection plane
      - [ ] Save in extra[0,1]
      - [ ] Append to extra[-1]
      - [ ] Save as text
    - [ ] Generate voxel indexes for each point
      - [ ] Save in extra[0..2]
      - [ ] Append to extra[-1]
      - [ ] Save as text
    - [ ] Perform a principal components analysis
      - [ ] Compute within voxels
      - [ ] Set the PCA radius
      - [ ] Save in extra[0..11]
      - [ ] Append to extra[-1]
      - [ ] Save as text
    - [ ] Generate connected component IDs based upon location and, optionally, other fields
      - [ ] Save component ID to extra[n]
      - [ ] Connection radius
      - [ ] Connection field(s)
    - [ ] Generate cluster IDs based upon data fields, xyz, cpi, rgb, extra[n]:
      - [ ] Save component ID to extra[n]
      - [ ] Set value of K
      - [ ] Use K means clustering
      - [ ] Use spectral clustering
      - [ ] Use Newman clustering
    - [ ] Generate neighbor indexes within a radius
      - [ ] Automatically determine the search radius
      - [ ] Save to a text file
      - [ ] Save in extra[0..n]
      - [ ] Save the nearest K neighbors
      - [ ] Randomly select K neighbors within the radius
  - [ ] Projection: project points onto a plane
    - [ ] pixel size in m/pixel
    - [ ] nodata value
    - [ ] field: norm\_Z, c, p, i, r, g, b, 0-7
    - [ ] min/max/%quantile
    - [ ] randomly select/vote
    - [ ] geotiff output
    - [ ] png output
- [ ] spoc config: Show configuration values
  - [ ] ~/.config/spoc/config
  - [ ] ~/.config/spoc/palettes
- [ ] spoc octree: break into files arranged as an octree, access/create spoc files
  - [ ] Given a bunch of spoc files, create a quadtree structure
  - [ ] Don't divide on z value (create quadtrees)
  - [ ] Create a spoc file from an octree given an extent
  - [ ] Unit/integration tests
- [ ] Examples
  - [X] Noop
  - [X] Very large spoc file with streaming

## Use Cases

* Given contiguous tiles/octrees of point clouds, create a point cloud
  covering a specified extent.
* Given contiguous tiles/octrees of point clouds, create products across
  all point clouds, but without artifacts near tile boundaries.
* Given contiguous tiles/octrees of point clouds, generate
  multi-resolution points clouds across the entire extent, adjusting
  extents to support lower resolutions as needed. For example, increase
  extent by 4X, while lowering resolution by 4X.
* Machine learning:
    * Rotate, project, etc.
    * Regularization, add noise
* Decimate/undecimate
