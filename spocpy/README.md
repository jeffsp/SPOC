# Install Python

    $ sudo dnf install python39 python39-devel

# Setup

Setup the virtual env and activate it

    $ python3.9 -m venv venv
    $ source ./venv/bin/activate

Install support libraries

    $ pip install -r ./requirements.txt

# Build

Build the extension

    $ make build

# Install

Install the extension into the current virtual environment. You have to
do this every time you change/recompile the extension.

    $ make install

# Test

Run all tests

    $ make test

# Examples

Run the interactive examples

    $ make examples

# Troubleshooting

## Problem

## Solution

