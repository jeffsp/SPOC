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
    * The order of the point records is not important and is not preserved
    * It is uncommon for two points to have the same exact location in
    3D space
* Point locations have 12 digits of precision preserved

# Requirements

* Don't pay for what you don't use: FreeWYDU
* Linear complexity
* 64-bit doubles
* Don't rely on data being spatially arranged
* Don't reinvent the wheel: rely on the system's optimization mechanisms
  (disk/memory cache)
* Streaming is not supported. Point clouds are random-access. Point
  clouds are assumed to fit into memory.
* Linux pipes are supported (not for LASlib I/O files)

# TODO

* [ ] FreeWYDU support in memory and disk footprints
* [ ] Gitlab runner
* [ ] CI/CD
* [ ] Test suite
* [ ] Coverage
* [ ] Cppcheck
* [ ] read/write spoc files
* [ ] read/write las files
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
* [ ] spoc quantize
* [ ] spoc sort: by field/by voxel
* [ ] spoc spatial\_filter: 2D/3D spatial filtering: changes 3D structure
* [ ] spoc spoc2las: warn when fields are lost, warn when precision is lost
* [ ] spoc tile
* [ ] spoc transform
* [ ] spoc transform\_lambda: Functional plugins with python/bash
