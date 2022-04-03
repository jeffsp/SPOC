# Introduction

The ASPRS LAS file format is used to store geospatial data in the form
of point clouds.

The "LAS" stands for LASer, and means that the file came from a Lidar
system. Many, if not most, LAS files are not generated from Lidar
systems but can be Electro-optical, SAR, ..., etc.

A point cloud is a ...

Many of the data fields contained in ASPRS LAS files are obsolete.

The aim of this file format is to be a subset of the LAS file format.

# Description of new format

The proposed extension is SPOC, standing for Simple POint Cloud.

* Points are stored as sets
    * The order of the point records is not important and is not preserved
    * It is uncommon for two points to have the same exact location in
    3D space
* Point locations have 12 digits of precision preserved

# Requirements

* Don't pay for what you don't use
* Linear complexity
* 64-bit doubles
* Don't rely on data being spatially arranged
* Don't reinvent the wheel: rely on the system's optimization mechanisms
  (disk/memory cache)
