#!/usr/bin/env python3
"""
Spocpy example
"""

import spocpy as sp
import plotly.express as px


def main():

    # Read an RGB
    with rasterio.open('../test_data/12_2071_2100_4_rgb.tif', 'r') as d:
        rgb = d.read()

    # Read a DSM
    with rasterio.open('../test_data/12_2071_2100_4_dsm.tif', 'r') as d:
        dsm = d.read()

    df = px.data.iris()
    fig = px.scatter_3d(df, x='sepal_length', y='sepal_width', z='petal_width',
                  color='species')
    fig.show()


if __name__ == "__main__":

    main()
