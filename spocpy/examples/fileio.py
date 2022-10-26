#!/usr/bin/env python3
"""
Spocpy file I/O example
"""

import glob
import spocpy as sp


def main():

    # Get all spoc and zpoc files in the test data
    fns = [fn for fn in glob.glob('../../test_data/*/*.spoc')]
    fns.extend([fn for fn in glob.glob('../../test_data/*/*.zpoc')])

    # Read and print all headers
    print('Headers:')
    for fn in fns:
        h = sp.readheader(fn)
        print(fn)
        print(f'\tversion {h.major_version}.{h.minor_version}')
        print(f'\twkt {len(h.wkt)} bytes')
        print(f'\textra_fields {h.extra_fields}')
        print(f'\ttotal_points {h.total_points}')
        print(f'\tcompressed {h.compressed}')

    # Read and print summary of all file contents
    print('')
    print('Files:')
    for fn in fns:

        # Read the file
        f = sp.readspocfile(fn)

        # Print the version
        print(fn)
        print(f'\tSPOC Version {f.getMajorVersion()}.{f.getMinorVersion()}')

        # Print a portion of the WKT
        wkt = f.getWKT()
        if len(wkt) < 10:
            print(f"\tWKT '{wkt}'")
        else:
            print(f"\tWKT '{wkt[0:5]}...{wkt[-5:]}'")

        # Print compression and number of records
        print(f'\tCompressed {f.getCompressed()}')
        prs = f.getPointRecords()
        print(f'\t{len(prs)} point records')

        # Print first and last records
        for r in prs[0:3]:
            print(f'\t\t({r.x:0.5f}, {r.y:0.5f}, {r.z:0.5f})', end="")
            print(f' cls={r.c}, id={r.p}, intensity={r.i}', end="")
            print(f' rgb=[{r.r}, {r.g}, {r.b}]')
        print('\t\t...')
        for r in prs[-3:]:
            print(f'\t\t({r.x:0.5f}, {r.y:0.5f}, {r.z:0.5f})', end="")
            print(f' cls={r.c}, id={r.p}, intensity={r.i}', end="")
            print(f' rgb=[{r.r}, {r.g}, {r.b}]')


if __name__ == "__main__":

    main()
