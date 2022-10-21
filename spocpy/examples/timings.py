#!/usr/bin/env python3
"""
Spocpy example
"""

import spocpy as sp


def main():

    # Read a DSM
    with rasterio.open('../test_data/12_2071_2100_4_dsm.tif', 'r') as d:
        dsm = d.read()

    # Inline numpy version
    tic = time.perf_counter()
    y1 = np.zeros(x.shape, dtype=np.dtype('b'))
    y1[(x != 0) & (x != np.nan) & (x > -999)] = 1
    toc = time.perf_counter() - tic
    print(f'Inline numpy version: {toc:0.5f} seconds elapsed')

    # Python functions version
    tic = time.perf_counter()
    y2 = texmeshpy.utils.getmask(x)
    toc = time.perf_counter() - tic
    print(f'Python module version: {toc:0.5f} seconds elapsed')

    # C++ version
    tic = time.perf_counter()
    y3 = texmeshpy.utilscpp.getmask(x)
    toc = time.perf_counter() - tic
    print(f'C++ extension version: {toc:0.5f} seconds elapsed')


if __name__ == "__main__":

    main()
