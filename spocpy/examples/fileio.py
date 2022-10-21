#!/usr/bin/env python3
"""
Spocpy example
"""

import spocpy as sp


def main():

    print(f'Module name = {sp.__name__}')
    print(f'Module doc = {sp.__doc__}')
    print(f'Module file = {sp.__file__}')

    # Read an RGB
    with rasterio.open('../test_data/12_2071_2100_4_rgb.tif', 'r') as d:
        rgb = d.read()

    # Read a DSM
    with rasterio.open('../test_data/12_2071_2100_4_dsm.tif', 'r') as d:
        dsm = d.read()

    rasterio.plot.show(rgb, title='fileio example')
    rasterio.plot.show(dsm, title='fileio example')


if __name__ == "__main__":

    main()
