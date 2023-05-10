#!/bin/sh

c_files=$(echo ./*.c)
gcc $c_files -lSDL2 -lSDL2_image -g3 -o ./game
