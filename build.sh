#!/bin/sh

set -xe

CFLAGS="-Wall -Wextra -ggdb"
LIBS=""

if [ `uname` = "Darwin" ]; then
    CFLAGS+=" -framework OpenGL -framework Cocoa"
else
    LIBS+="-lX11 -lGL -lXrandr"
fi

cc $CFLAGS $LIBS -o tinygl main.c
