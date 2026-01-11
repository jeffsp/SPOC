% SPOC_VIEWER(1) SPOC User's Manual | Version 3.0
% jeffsp@gmail.com
% JAn 2, 2022

# NAME

spoc_viewer - view a LAS file

# USAGE

spoc_viewer [*options*] *input_filename* [*input_filename* ...]

# DESCRIPTION

View a point cloud given a LAS file.

# OPTIONS

--help, -h
:   Get help

--verbose, -v
:   Set verbose mode ON

--color-mode=*ARG*, -c *ARG*
:   Set the color mode: *C*=classification mode, *E*=elevation mode,
    *I*=intensity mode, *R*=region mode, *X*=random region mode, *B*=RGB
    mode, *S*=classification shaded with intensity, *G*=classification
    shaded with green band.

--box-mode, -b
:   Set box mode to ON. In box mode, boxes will be drawn instead of
    points. Note that rendering in box mode is typically very slow.

--palette=*FN*, -p *FN*
:   Set the palette filename. The palette file is a text file containing
    an R, G, B triplet on each row. The values should be separated by
    single spaces.

--resolution=*N*, -r *N*
:   Voxel resolution. By default the points will be quantized to *N*
    meter resolution and duplicates will be eliminated.

    Set to 0 to disable voxelization and use all unquantized points.

--camera-coordinates=*STRING*, -a *STRING*
:   Provide a string of three sets of x,y,z points that specify the
    camera position, focal point, and viewup, separated by commas.

    This is provided in order to serialize camera views. You may print
    the current camera coordinates using the key command listed below.
    This string can then be used as a command line argument on
    subsequent calls to the viewer.

--screenshot-filename=*FN*, -s *FN*
:   Specify a screenshot PNG format filename. When this option is
    specified, the image will be rendered (at the coordinates specified by
    the *camera-coordinates* argument, if provided), saved to the
    *filename* in PNG format, and then the application will exit.

--opacity=*N*, -o *N*
:   Set the opacity of the rendered points. Default is opacity=1.0,
    i.e. not see through at all.

--point-size=*N*, -i *N*
:   Set the point size for the points when not rendering in box-mode.
    default is point-size=5


# USER INTERFACE

Keypress q: Quit

Button 1: Rotate the camera around its focal point

Button 2: Pan the camera

Button 3: Zoom the camera

Keypress h: Toggle help

Keypress f: Fly to the picked point

Keypress r: Reset the camera view along the current view direction.
            Centers the actors and moves the camera so that all actors are
            visible.

Keypress c: Print the current camera coordinates string to the terminal

Keypress Keypad+: Zoom in

Keypress Keypad-: Zoom out
