#!/bin/bash

if [ ! -d "build" ]; then
	mkdir build
fi

gcc src/lpf.c -Wall -Werror -pedantic -std=c23 -o build/lpf $(pkg-config --cflags --libs sndfile)

if [ "$?" != "0" ]; then
	echo "compilation error"
	exit 1
fi

args=""
while [ "$#" != 0 ]; do
	args+="$1 "; shift
done

./build/lpf $args

if [ "$?" != "0" ]; then
	echo "runtime error"
	exit 1
fi
