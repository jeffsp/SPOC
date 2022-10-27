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
                'flex-direction': 'column',
                'width': '200px',
                },
            children=[
                html.Label('Point Size'),
                html.Div(dcc.Slider(1, 7, 1,
                                    id='pointsize-slider',
                                    vertical=False,
                                    value=2)),
                html.Label('Elevation'),
                html.Div(dcc.Slider(0, 10, 1,
                                    id='elevation-slider',
                                    vertical=False,
                                    value=10)),
                html.Label('Classification'),
                html.Div(dcc.Slider(0, 10, 1,
                                    id='classification-slider',
                                    vertical=False,
                                    value=0)),
                ]
            ),
        html.Div(
            id="point-cloud",
            style={
                'background-color': 'rgb(235,235,225)',
                'width': '100%',
                'height': '100%',
                },
            children=[
                    dcc.Graph(
                        id='point-cloud-graph',
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


@app.callback(
    Output('point-cloud-graph', 'figure'),
    Input('pointsize-slider', 'value'))
def update_figure(pointsize_value):
    """
    Update the point cloud graph when the point size changes
    """
    print('point size', pointsize_value)
    fig.update_traces(marker=dict(size=pointsize_value, color=colors))

    return fig


app.run_server(debug=True)
