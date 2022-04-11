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

* Points are stored as sets
    * The order of the point records is preserved
    * It is uncommon for two points to have the same exact location in
    3D space
* Point locations have 12 digits of precision preserved

# Requirements

* Don't pay for what you don't use: FreeWYDU
    The memory footprint is zero for fields that are all zero. When
    point clouds are resized each field is checked, and fields that
    contain all 0's are cleared and their memory is freed.
* Linear complexity
* 64-bit doubles
* Don't rely on data being spatially arranged
* Don't reinvent the wheel: rely on the system's optimization mechanisms
  (disk/memory cache)
* Streaming is not supported. Point clouds are random-access. Point
  clouds are assumed to fit into memory.
* Linux pipes are supported (not for LASlib I/O files)

# TODO

* [X] FreeWYDU support in memory and disk footprints
* [X] Gitlab runner
* [X] CI/CD
* [ ] Test data files
* [ ] Unit test suite
* [ ] Integration test suite
* [ ] Coverage
* [X] Cppcheck
* [X] read/write spoc files
* [X] read/write las files
* [ ] spoc compare: Compare two point clouds, specifying fields
* [ ] spoc connect: Connected components, saved in extra fields
* [ ] spoc crop: 2D/3D meters/percentage/by classification/by component
* [ ] spoc decimate
* [ ] spoc dump: Dump the contents of a spoc file as text
* [ ] spoc field\_filter: 2D/3D spatial filtering: does not changes xyz coords
* [ ] spoc info
* [ ] spoc interpolate
* [ ] spoc las2spoc
* [ ] spoc merge
* [ ] spoc octree: break into files arranged as an octree
* [ ] spoc quantize: on voxel boundaries
* [ ] spoc sort: by field/by voxel
* [ ] spoc spatial\_filter: 2D/3D spatial filtering: changes 3D structure
* [ ] spoc spoc2las: warn when fields are lost, warn when precision is lost
* [ ] spoc srs: Get/set SRS
* [ ] spoc tile
* [ ] spoc transform
* [ ] spoc transform\_lambda: Functional plugins with python/bash
