#!/bin/sh

c_files=$(echo ./*.c)
gcc $c_files -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -g3 -o ./game
