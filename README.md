# Introduction

SPOC is the Simple POint Cloud format used to store 3D geospatial point
data. Each record in a SPOC file has an associated X, Y, and Z
coordinate, and possibly other data fields, like red, green, and blue
color information, classification information, point collection identifier,
intensity information, or arbitrary integer values.

The X, Y, and Z values are stored at 64-bit precision floating point
numbers.

The ASPRS LAS file format is used to store geospatial data in the form
of point clouds.

The "LAS" stands for LASer, and means that the file came from a Lidar
system. Many, if not most, LAS files are not generated from Lidar
systems but can be Electro-optical, SAR, ..., etc.

A point cloud is a ...

Many of the data fields contained in ASPRS LAS files do not apply to
data collected from EO, SAR, ...

The aim of this file format is to be a subset of the LAS file format.

This project is defined for Linux-based systems, including MAC OS and
the Windows Linux subsystem.

# Description of SPOC Format

The proposed extension is SPOC, or Simple POint Cloud.

* Points are stored as vectors
    * The order of the point records is preserved
    * It is uncommon for two points to have the same exact location in
    3D space
* Point locations are stored as 64-bit double precision numbers, so
  about 16 decimal digits of precision is preserved, assuming an IEEE
  754 standard.

# Requirements

* Don't pay for what you don't use: FreeWYDU
    The memory footprint is zero for fields that are all zero. When
    point clouds are resized each field is checked, and fields that
    contain all 0's are cleared and their memory is freed.
    Setting individual point values does not trigger a resizing check --
    only resizing operations, or a explicit call to `reallocate()` will
    trigger checks. Also, writing a non-zero value to a field that
    contains all zeros will trigger a resize operation for that field.
    Files are compressed on disk, so this doesn't have as much effect on
    the disk footprint, although most compression schemes have some
    overhead associated with each large block stored on disk. This
    scheme stores a single flag for the entire field, regardless of its
    size.
* Linear complexity
* 64-bit doubles
* Don't rely on data being spatially arranged
* Don't reinvent the wheel: rely on the system's optimization mechanisms
  (disk/memory cache)
* Streaming is not supported. Point clouds are random-access. Point
  clouds are assumed to fit into memory.
* Linux pipes are supported (not for LASlib I/O files)
* Avoid dependencies
    * zlib compression library
    * laslib for las/spoc translation utilities
* Functional programming style: avoid OOP
* SPOC makes assumptions about the byte ordering, and is therefore not
  portable

# TODO

* [X] FreeWYDU support in memory and disk footprints
* [X] Gitlab runner
* [X] CI/CD
* [X] Version number in header
* [X] Test data files
* [X] Unit test suite
* [X] Integration test suite
* [X] Man page generation
* [X] Coverage
* [X] Cppcheck
* [X] read/write spoc files
* [X] read/write las files
* [X] spoc info
* [X] spoc spoc2text/text2spoc
* [X] spoc spoc2las/las2spoc
* [X] spoc srs: Get/set SRS
* [X] spoc tile: Tile into regular non-overlapping tiles.
      [ ] Tile into irregular, overlapping tiles by component, point
          source ID, cluster ID -- specified by any field
* [X] spoc diff: diff two point clouds, return error if different
      [X] Header only
      [X] Data only
      [X] Field F only - x, y, z, c, p, i, r, g, b, 0-7
      [X] Not - inverse results
* [X] spoc transform:
      [X] Replace fields
      [X] Allow arbitrary operations using named pipes
      [ ] Allow numeric operations on fields, using parallel calls to `bc`
* [ ] spoc decimate - Remove points
* [ ] spoc voxelize - Replace X, Y, Z locations with voxelized locations
      [ ] Offset, random offset (radius)
      [ ] Seed
      [ ] Unique - Replace duplicate locations, use voting to set field values
* [ ] spoc radius search: get indexes of neighbors within a radius
      [ ] Set in extra fields 0-7
      [ ] Save to a text file
      [ ] Save the nearest K neighbors
      [ ] Randomly select K neighbors within the radius
      [ ] Set a random seed
* [ ] spoc filter:
      [ ] Remove/keep points with certain properties
      [ ] 2D/3D field filtering - does not changes xyz coords
      [ ] Spatial Filter - 2D/3D spatial filtering, changes 3D structure
                         - Guassian filter
                         - Median filter
                         - X, Y, Z
      [ ] Allow arbitrary operations using named pipes
* [ ] spoc connect: Connected components, component id saved in extra[n]
* [ ] spoc cluster: Separate into K clusters based upon:
      [ ] Location, X/Y/Z, X/Y
      [ ] Intensity
      [ ] Color
* [ ] spoc crop: 2D/3D meters/percentage/by classification/by component
* [ ] spoc merge: optionally set point source id
      [ ] -f force merge
      [ ] -size threshold = 16 (X number of point clouds), don't merge
          if area increases by more than a factor of this threshold
* [ ] spoc octree: break into files arranged as an octree, access/create spoc files
           -z Don't break on z value (create quadtrees)
