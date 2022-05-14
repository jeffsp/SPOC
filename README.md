# Introduction

SPOC is the Simple POint Cloud format used to store 3D geospatial point
data. Each record in a SPOC file has an associated X, Y, and Z
coordinate, and possibly other data fields, like red, green, and blue
color information, classification information, point collection identifier,
intensity information, or arbitrary integer values.

The X, Y, and Z values are stored at 64-bit precision floating point
numbers.

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

* Points are stored as vectors
* The order of the point records is preserved
* Point locations are stored as 64-bit double precision numbers, so
  about 16 decimal digits of precision is preserved, assuming an IEEE
  754 standard.
* Each point has associated 16- and 32-bit fields that are common to 3D
  point collections, like red, green, blue, intensity, collection ID,
  classification, and arbitrary 64-bit unsigned integer side data.

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
* [X] spoc diff: diff two point clouds, return error if different
      [X] Header only
      [X] Data only
      [X] Field F only - x, y, z, c, p, i, r, g, b, 0-7
      [X] Not - inverse results
* [X] spoc tile: Tile into regular non-overlapping tiles.
      [X] Number of tiles on largest size
* [ ] spoc merge: Combine several point clouds into one
      [ ] Set point id
      [ ] Size threshold = 16 (X number of point clouds), don't merge
          if area increases by more than a factor of this threshold
      [ ] Force merge
* [ ] spoc crop: Remove edge points from a point cloud
      [ ] 2D/3D
      [ ] meters/percentage
      [ ] Top/bottom/north/south/east/west
* [X] spoc transform: Change fields in a point cloud. The output point
                      cloud will have the same number of points and the
                      points will be in the same order.
      [X] Replace fields
      [ ] Center points about mean
      [ ] Rotate by N degrees about X/Y/Z axis
      [ ] Add offset to X,Y,Z
      [ ] Scale by X,Y,Z
      [ ] Set random seed
      [ ] Add random Gaussian noise to X,Y,Z
      [ ] Add random uniform noise to X,Y,Z
      [ ] Allow numeric operations on fields using parallel calls to `bc`
* [ ] spoc subsample: Remove points
      [ ] subsampling radius
      [ ] choose N random points within subsampling radius
      [ ] Set random seed
      [ ] voxelize flag Relocate points to voxel centers
      [ ] Average R, G, B within voxels
      [ ] Average intensity within voxels
      [ ] Vote for c and p fields within voxels
* [ ] spoc assign: Assign fields in one point cloud with fields
                   from another point cloud
      [ ] Specify which fields to assign: all,x,y,z,c,p,i,r,g,b,extra
          Note that assigning X,Y,Z is useful for unrotating/uncentering
          a point cloud after performing transformations
      [ ] Use voxels of size R for assignment
      [ ] Use nearest neighbors for assignment
      [ ] Use point index in extra field N for assignment
      [ ] Use component ID for assignment
      [ ] Use cluster ID for assignment
* [ ] spoc filter:
      [ ] Remove/keep points with certain properties
      [ ] 2D/3D field filtering - does not changes xyz coords
      [ ] Spatial Filter - 2D/3D spatial filtering, changes 3D structure
                         - Guassian filter
                         - Median filter
                         - X, Y, Z
      [X] Allow arbitrary operations using a filter interface and pipes
      [ ] Add support for a two pass filter so that points can be
          changed based upon global point cloud properties, e.g.: add a
          preprocess() function
      [ ] Add support for point removal in filter interface by adding a
          function that accepts a vector of point records and returns a
          filtered vector of point records
* [ ] spoc connect: Generate connected component IDs based upon location
                    and, optionally, other fields
      [ ] Save component ID to extra[n]
      [ ] Connection radius
      [ ] Connection field
* [ ] spoc cluster: Generate cluster IDs based upon data fields, xyz,
                    cpi, rgb, extra[n]:
      [ ] Save component ID to extra[n]
      [ ] Set value of K
      [ ] Use K means clustering
      [ ] Use spectral clustering
      [ ] Use Newman clustering
* [ ] spoc radius search: get indexes of neighbors within a radius
      [ ] Automatically determine the search radius
      [ ] Set in extra fields 0-7
      [ ] Save to a text file
      [ ] Save the nearest K neighbors
      [ ] Randomly select K neighbors within the radius
      [ ] Set a random seed
* [ ] spoc octree: break into files arranged as an octree, access/create spoc files
      [ ] Given a bunch of spoc files, create a quadtree structure
      [ ] Don't divide on z value (create quadtrees)
      [ ] Create a spoc file from an octree given an extent
