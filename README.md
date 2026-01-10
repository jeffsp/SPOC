# Introduction

This repository contains library code and applications for reading,
writing, and processing Simple POint Cloud, or SPOC, format files.

SPOC format files are used to store 3D geospatial data.

SPOC files comply with
[WebDataset](https://github.com/webdataset/webdataset) conventions.

SPOC files are GNU tar format files that conform to a certain set of
simple file naming conventions. SPOC files, like WebDataset, are
compatible with POSIX standards.

# Conventions

## Required point record fields

SPOC files must contain, at a minimum, files containing the following
three files.

```
x-00000000.double
y-00000000.double
z-00000000.double
```

If the file only contains a single collection of point records, you can omit
the -0000000 suffix from the point record names.

```
x.double
y.double
z.double
```

However, if the file contains more than one collection of point records,
enumeration suffixes are required.

## Optional point record fields

```
c-00000000.uint32_t
p-00000000.uint32_t
i-00000000.uint16_t
r-00000000.uint16_t
g-00000000.uint16_t
b-00000000.uint16_t
```

### Extra fields

In addition...

```
e0-00000000.<type>
e1-00000000.<type>
...
eN-00000000.<type>
```

You cannot have two extra field files with the same name and different types

```
e5-00000000.double  # First one is OK
e5-00000000.uint8_t # Not allowed, already have an 'e5' field
```

If the file only contains a single collection of point records, you can omit
the -0000000 suffix from the point record names.

```
c.uint32_t
p.uint32_t
i.uint16_t
r.uint16_t
g.uint16_t
b.uint16_t
e0.<type>
e1.<type>
...
eN.<type>
```

However, if the file contains more than one collection of point records,
enumeration suffixes are required on all filenames.

## Optional meta-data fields

```
version.json
{
    'major': 2,
    'minor': 0
}
```

[Open Geospatial Consortium
(OGC)](https://www.ogc.org/standards/wkt-crs/) well-known text (WKT)
format coordinate reference system string.

```
crs.txt
PROJCS["NAD83 / Texas Central (ftUS)",GEOGCS["NAD83",DATUM
["North American Datum 1983",SPHEROID["GRS 1980",6378137,298.257222101,
...
```

```
extents.json
{
    '00000000': {
        'minx': 100, 'miny': 100, 'minz': 100,
        'maxx': 100, 'maxy': 100, 'maxz': 100},
    '00000001': {
        'minx': 100, 'miny': 100, 'minz': 100,
        'maxx': 100, 'maxy': 100, 'maxz': 100},
        ...
}
```
