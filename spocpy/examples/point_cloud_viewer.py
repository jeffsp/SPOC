#!/usr/bin/env python3
"""
Spocpy example
"""

from dash import Dash, dcc, html, Input, Output

import numpy as np
import pandas as pd
import plotly.express as px
import spocpy as sp
from random import randrange


# Read the file
fn = '../../test_data/eo/romeo007_segmented_e0.spoc'

# First read the header
h = sp.readheader(fn)

# Read the entire file
f = sp.readspocfile(fn)

# Create a Pandas dataframe
df = pd.DataFrame()

# Add columns
df["x"] = f.getX()
df["y"] = f.getY()
df["z"] = f.getZ()

# Define callback control variables
control = {
        'default_pointsize_value':  2,
        'default_marker_value': 'ele',
        'last_pointsize_value': 2,
        'last_marker_value': 'rgb',
    }

# Create figure
fig = px.scatter_3d(data_frame=df, x='x', y='y', z='z')
fig.update_layout(template='simple_white')
fig.update_layout(margin=dict(l=0, r=0, b=0, t=0))
fig.update_scenes(aspectmode='data')
fig.update_traces(marker=dict(size=control['default_pointsize_value']))

# Create the app
app = Dash(__name__)

# Define a colormap
elevation_colormap = px.colors.sequential.Rainbow

# Create a colormap
classification_colormap = [
    'rgb(0,64,64)',      # 0 never classed
    'rgb(255,0,0)',      # 1 unknown
    'rgb(128,64,0)',     # 2 ground
    'rgb(0,213,0)',      # 3 low veg
    'rgb(0,128,0)',      # 4 medium veg
    'rgb(0,192,0)',      # 5 high veg
    'rgb(0,255,255)',    # 6 building
    'rgb(128,128,192)',  # 7 low noise
    'rgb(128,128,128)',  # 8 key point
    'rgb(0,0,255)',      # 9 water
    'rgb(128,128,0)',    # 10 rail
    'rgb(192,192,192)',  # 11 road
    'rgb(128,128,128)',  # 12 overlap
    'rgb(255,255,0)',    # 13 wires
    'rgb(0,128,192)',    # 14 docks
    'rgb(255,200,192)',  # 15 transmission tower
    'rgb(255,128,192)',  # 16 bad data
    'rgb(64,32,0)',      # 17 bridge
    'rgb(255,128,192)',  # 18 high noise
    'rgb(243,158,0)',    # 19 vertical obstruction
    'rgb(128,64,64)',    # 20 non-noise subterrain
    'rgb(128,0,255)',    # 21 vehicle
    'rgb(128,128,128)',  # 22 manmade
    'rgb(255,0,128)',    # 23 perimeter
    'rgb(128,0,128)',    # 24 urban
    'rgb(0,0,128)',      # 25 ducts
    'rgb(128,0,128)',    # 26 aircraft
    'rgb(0,0,160)',      # 27 solar panel
    ]

# Create a random colormap
segment_colormap = [f'rgb({randrange(0,256)},'
                    f'{randrange(0,256)},'
                    f'{randrange(0,256)})' for i in range(256)]

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
                html.Div(dcc.Slider(
                    1, 7, 1,
                    id='pointsize-slider',
                    vertical=False,
                    value=control['default_pointsize_value'])),
                html.Label('Marker'),
                html.Div(dcc.Dropdown(
                    id='marker-dropdown',
                    options=[
                        {'label': 'Elevation', 'value': 'ele'},
                        {'label': 'RGB', 'value': 'rgb'},
                        {'label': 'Classification', 'value': 'cls'},
                        {'label': 'Segment', 'value': 'seg'},
                        ],
                    value=control['default_marker_value'])),
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
    Input('pointsize-slider', 'value'),
    Input('marker-dropdown', 'value'))
def update_figure(
        pointsize_value,
        marker_value):
    """
    Update the point cloud graph when the marker
    """

    if control['last_marker_value'] != marker_value:

        # Save state
        control['last_marker_value'] = marker_value

        if marker_value == 'ele':

            # Get elevation colors
            z = f.getZ()
            zmin = np.amin(z)
            d = np.amax(z) - zmin
            elevation_indexes = [int((z[i]-zmin)*(len(elevation_colormap)-1)/d)
                                 for i in range(0, len(z))]
            colors = [elevation_colormap[i]
                      for i in elevation_indexes]

            fig.update_layout(showlegend=False)

        elif marker_value == 'rgb':

            # Get RGB colors
            r = f.getR()
            g = f.getG()
            b = f.getB()
            colors = [f'rgb({rr//256},{gg//256},{bb//256})'
                      for rr, gg, bb in zip(r, g, b)]

            fig.update_layout(showlegend=False)

        elif marker_value == 'cls':

            # Get classification colors
            c = f.getC()
            colors = [classification_colormap[cc] for cc in c]

            fig.update_layout(showlegend=False)

        elif marker_value == 'seg':

            # Segments should be in e[0]
            if h.extra_fields < 1:
                raise Exception("Expected segment numbers in e[0],"
                                "but this file has no extra fields")

            # Get the segment colors
            e = f.getExtra(0)
            colors = [segment_colormap[ee] for ee in e]

            fig.update_layout(showlegend=False)

        else:
            print('Invalid marker value')

        # Change colors
        fig.update_traces(marker=dict(color=colors))

    elif control['last_pointsize_value'] != pointsize_value:
        # Reset state
        control['last_pointsize_value'] = pointsize_value

        # Change point size
        fig.update_traces(marker=dict(size=pointsize_value))

    """
    Don't change the camera position, zoom, ..., etc.
    For more information on UI state, search for:

        "dash preserving ui state with uirevision"
    """
    fig.update_layout(uirevision='0')

    return fig


# If you want to host across your network, uncomment the next line
# app.run_server(debug=True, host='0.0.0.0')
app.run_server(debug=True)
