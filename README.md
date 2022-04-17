# Introduction

The ASPRS LAS file format is used to store geospatial data in the form
of point clouds.

The "LAS" stands for LASer, and means that the file came from a Lidar
system. Many, if not most, LAS files are not generated from Lidar
systems but can be Electro-optical, SAR, ..., etc.

A point cloud is a ...

Many of the data fields contained in ASPRS LAS files do not apply to
data collected from EO, SAR, ...

The aim of this file format is to be a subset of the LAS file format.

# Description of new format

The proposed extension is SPOC, standing for Simple POint Cloud.

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

# TODO

* [X] FreeWYDU support in memory and disk footprints
* [X] Gitlab runner
* [X] CI/CD
* [X] Version number in header
* [ ] Test data files
* [X] Unit test suite
* [ ] Integration test suite
* [X] Man page generation
* [X] Coverage
* [X] Cppcheck
* [X] read/write spoc files
* [X] read/write las files
* [ ] spoc compare: Compare two point clouds, specifying fields
* [ ] spoc connect: Connected components, saved in extra fields
* [ ] spoc crop: 2D/3D meters/percentage/by classification/by component
* [ ] spoc decimate
* [X] spoc spoc2text/text2spoc
* [ ] spoc field\_filter: 2D/3D spatial filtering: does not changes xyz coords
* [X] spoc info
* [ ] spoc interpolate
* [ ] spoc las2spoc
* [ ] spoc merge: optionally set point source id
* [ ] spoc neighbors: set index of neighbors in the extra fields
* [ ] spoc octree: break into files arranged as an octree
* [ ] spoc quantize: on voxel boundaries
* [ ] spoc sort: by field/by voxel
* [ ] spoc spatial\_filter: 2D/3D spatial filtering: changes 3D structure
* [ ] spoc spoc2las: warn when fields are lost, warn when precision is lost
* [ ] spoc srs: Get/set SRS
* [ ] spoc tile
* [ ] spoc transform/transform\_lambda: Functional plugins with python/bash
