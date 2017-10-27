#!/bin/bash

MY_PWD="$(pwd)"

[[ -d build ]] && rm -r build

CFLAGS=-g
MY_PWD+="/build"
meson \
	-Ddebug=true \
	-Dhomedir-install=${HOME} \
	$@ . build
ninja -C build

# Uncomment to install
#ninja -C build/ install
