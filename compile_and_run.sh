#!/bin/bash

if [ ! -d "build" ]; then
	mkdir build
fi

gcc src/lpf.c -Wall -Werror -Wextra -pedantic -std=c11 -o build/lpf $(pkg-config --cflags --libs sndfile)

if [ "$?" != "0" ]; then
	echo "compilation error"
	exit 1
fi

./build/lpf "$@"

if [ "$?" != "0" ]; then
	echo "runtime error"
	exit 1
fi

echo "done"
