#!/usr/bin/env python3
"""
Spocpy example
"""

import pandas as pd
import plotly.express as px
import plotly.graph_objs as go
import spocpy as sp


def main():

    # Read a file
    fn = '../../test_data/eo/romeo007.spoc'
    f = sp.readspocfile(fn)

    # Create a Pandas dataframe
    df = pd.DataFrame()

    # Add columns
    df["x"] = f.getX()
    df["y"] = f.getY()
    df["z"] = f.getZ()
    df["r"] = f.getR()
    df["g"] = f.getG()
    df["b"] = f.getB()

    # Plot it
    colors = [f'rgb({r//256},{g//256},{b//256})'
              for r, g, b in zip(df.r.values, df.g.values, df.b.values)]
    trace = go.Scatter3d(x=df.x, y=df.y, z=df.z,
                         mode='markers', marker=dict(size=2, color=colors))
    data = [trace]
    layout = go.Layout(margin=dict(l=0, r=0, b=0, t=0))
    fig = go.Figure(data=data, layout=layout)
    fig['layout']['scene']['aspectmode'] = "data"
    fig.show()


if __name__ == "__main__":

    main()
