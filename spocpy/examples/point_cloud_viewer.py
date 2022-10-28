#!/usr/bin/env python3
"""
Spocpy example
"""

from dash import Dash, dcc, html, Input, Output

import numpy as np
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

# Define a colormap
elevation_colormap = px.colors.sequential.Rainbow

# Create figure
fig = px.scatter_3d(data_frame=df, x='x', y='y', z='z')
fig.update_layout(template='simple_white')
fig.update_layout(margin=dict(l=0, r=0, b=0, t=0))
fig.update_scenes(aspectmode='data')

# Create the app
app = Dash(__name__)

# Define callback control variables
control = {
        'default_pointsize_value':  2,
        'default_elevation_value': 10,
        'default_rgb_value': 0,
        'default_classification_value': 0,
        'last_pointsize_value': 2,
        'last_elevation_value': 10,
        'last_rgb_value': 0,
        'last_classification_value': 0,
    }

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
                html.Label('Elevation'),
                html.Div(dcc.Slider(
                    0, 10, 1,
                    id='elevation-slider',
                    vertical=False,
                    value=control['default_elevation_value'])),
                html.Label('RGB'),
                html.Div(dcc.Slider(
                    0, 10, 1,
                    id='rgb-slider',
                    vertical=False,
                    value=control['default_rgb_value'])),
                html.Label('Classification'),
                html.Div(dcc.Slider(
                    0, 10, 1,
                    id='classification-slider',
                    vertical=False,
                    value=control['default_classification_value'])),
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
    Input('elevation-slider', 'value'),
    Input('rgb-slider', 'value'),
    Input('classification-slider', 'value'))
def update_figure(
        pointsize_value,
        elevation_value,
        rgb_value,
        classification_value):
    """
    Update the point cloud graph when the color or point size changes
    """

    if control['last_elevation_value'] != elevation_value \
            or \
            control['last_rgb_value'] != rgb_value \
            or \
            control['last_classification_value'] != classification_value:

        # Save state
        control['last_elevation_value'] = elevation_value
        control['last_rgb_value'] = rgb_value
        control['last_classification_value'] = classification_value

        # Get point cloud values
        z = f.getZ()
        r = f.getR()
        g = f.getG()
        b = f.getB()
        c = f.getC()

        # Recompute colors
        #
        # Get elevation colors
        zmin = np.amin(z)
        d = np.amax(z) - zmin
        elevation_indexes = [int((z[i]-zmin)*(len(elevation_colormap)-1)/d)
                             for i in range(0, len(z))]
        elevation_colors = [elevation_colormap[i]
                            for i in elevation_indexes]

        # Get RGB colors
        rgb_colors = [f'rgb({rr//256},{gg//256},{bb//256})'
                      for rr, gg, bb in zip(r, g, b)]

        # Get classification colors
        classification_colors = [classification_colormap[cc]
                                 for cc in c]

        # Change colors
        fig.update_traces(marker=dict(color=rgb_colors))

        # Show legend if needed
        showlegend = True if classification_value > 0 else False
        fig.update_layout(showlegend=showlegend)

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


app.run_server(debug=True)
