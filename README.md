# Introduction

This repository contains library code and applications for reading,
writing, and processing Simple POint Cloud, or SPOC, format files.

SPOC format files are used to store 3D geospatial data.

SPOC files comply with
[WebDataset](https://github.com/webdataset/webdataset) conventions.

SPOC files are GNU tar format files that conform to a certain set of
simple file naming conventions. SPOC files, like WebDataset, are
compatible with POSIX standards.

A SPOC file is a WebDataset shard, meaning that the data contained in
the file can be only part of a larger collection of point cloud point
records.

Point record fields are grouped together in a SPOC file so that the
data contained in the fields can be compressed more efficiently.

In a SPOC file, each AOI is a 'sample'. Point records are not samples.

The file format intentionally does not contain a central index or
metadata file. This gives the shard creator to have the flexibility to
divide the shard in any way they choose. Applications at a higher
level can for example, arrange the shards in 3D Z-ordering or octrees.
The SPOC repository contains applications that perform both Z-ordering
and octree ordering of points across multiple shards.

SPOC shards typically are limited in size to 1GB.

# Conventions

## Required point record fields

A SPOC file must contain, at a minimum, the following three files.

```
0000.x.double
0000.y.double
0000.z.double
```

The "0000" can be any string, but the point records must associated with ...
must all be named the same.

For example,
```
AOI-0.x.double
AOI-0.y.double
AOI-0.z.double
AOI-1.x.double
AOI-1.y.double
AOI-1.z.double
...
building-0.x.double
building-0.y.double
building-0.z.double
building-1.x.double
building-1.y.double
building-1.z.double
...
cars.x.double
cars.y.double
cars.z.double
busses.x.double
busses.y.double
busses.z.double
...
```

Each of these file are blocks of N elements containing 64bit, double precision floating point format.
In particular, [IEEE 754 format](https://en.wikipedia.org/wiki/Decimal64_floating-point_format).

These files must all contain the same number of elements.

## Optional point record fields

```
0000.c.int32_t
0000.p.int32_t
0000.i.int16_t
0000.r.int16_t
0000.g.int16_t
0000.b.int16_t
```

### Extra fields

In addition...

```
0000.e0.<type>
0000.e1.<type>
...
0000.e99.<type>
```

You cannot have two extra field files with the same name and different types

```
0000.e5.double  # First one is OK
0000.e5.uint8_t # Not allowed, already have an 'e5' field
```

## Optional meta-data files

```
spoc_version.json
{
    'major': 2,
    'minor': 0
}
```

[Open Geospatial Consortium
(OGC)](https://www.ogc.org/standards/wkt-crs/) well-known text (WKT)
format coordinate reference system string.

```
0000.crs.txt
PROJCS["NAD83 / Texas Central (ftUS)",GEOGCS["NAD83",DATUM
["North American Datum 1983",SPHEROID["GRS 1980",6378137,298.257222101,
...
```

```
0000.extent.json
{
    'minx': 100, 'miny': 100, 'minz': 100,
    'maxx': 100, 'maxy': 100, 'maxz': 100,
}
```
