#!/usr/bin/env python3
"""
Spocpy timings example
"""

import glob
import spocpy as sp
import time


def main():

    # Get all spoc and zpoc files in the test data
    fns = [fn for fn in glob.glob('../../test_data/*/*.spoc')]
    fns.extend([fn for fn in glob.glob('../../test_data/*/*.zpoc')])

    # Read just the headers
    print('Reading all headers...')

    tic = time.perf_counter()
    for fn in fns:
        h = sp.readheader(fn)
    toc = time.perf_counter() - tic

    print(f'{toc:0.8f} seconds elapsed')

    # Read all point records
    print('Reading all point records...')

    total_point_records = 0
    tic = time.perf_counter()
    for fn in fns:
        f = sp.readspocfile(fn)
        prs = f.getPointRecords()
        total_point_records += len(prs)
    toc = time.perf_counter() - tic

    print(f'{toc:0.8f} seconds elapsed')
    print(f'{total_point_records} total point records read')


if __name__ == "__main__":

    main()
