#!/usr/bin/bash

#build=debug
build=release

./build/${build}/generate_infinite_points | ./build/debug/stream_averaging
