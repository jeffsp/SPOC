#!/usr/bin/env python3
"""
Spocpy example
"""

from dash import Dash, dcc, html, Input, Output

import pandas as pd
import plotly.express as px
import spocpy as sp


# Read a file
fn = '../../test_data/eo/romeo007.spoc'
f = sp.readspocfile(fn)

# Create a Pandas dataframe
df = pd.DataFrame()

# Add columns
df["x"] = f.getX()
df["y"] = f.getY()
df["z"] = f.getZ()

# Compute colors
colors = [f'rgb({r//256},{g//256},{b//256})'
          for r, g, b in zip(f.getR(), f.getG(), f.getB())]

df["rgb"] = colors

# Create figure
fig = px.scatter_3d(data_frame=df, x='x', y='y', z='z')
fig.update_layout(template='simple_white')
fig.update_layout(margin=dict(l=0, r=0, b=0, t=0))
fig.update_scenes(aspectmode='data')
fig.update_traces(marker=dict(size=2, color=colors))

# Create the app
app = Dash(__name__)

# Create the layout
app.layout = html.Div(
    id='body',
    style={
        'display': 'flex',
        'width': '99vw',
        'height': '95vh',
        },
    children=[
        html.Div(
            id='controls',
            style={
                'display': 'flex',
                'background-color': 'rgb(245,245,245)',
                'flex-direction': 'row',
                },
            children=[
                html.Div(dcc.Slider(0, 100, 10, id='slider1', vertical=True)),
                html.Div(dcc.Slider(0, 100, 10, id='slider2', vertical=True)),
                html.Div(dcc.Slider(0, 100, 10, id='slider3', vertical=True)),
                html.Div(dcc.Slider(0, 100, 10, id='slider4', vertical=True)),
                ]
            ),
        html.Div(
            id="point-cloud",
            style={
                'background-color': 'rgb(0,255,0)',
                'width': '100%',
                'height': '100%',
                },
            children=[
                    dcc.Graph(
                        figure=fig,
                        style={
                            'width': '100%',
                            'height': '100%',
                            },
                        ),
                ],
            )
        ]
    )


app.run_server(debug=True)
