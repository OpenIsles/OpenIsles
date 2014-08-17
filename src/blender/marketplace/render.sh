#!/bin/bash

blender -b marketplace.blend \
-S Szene-Nord -o //rendered/north.png -f 1 \
-S Szene-Süd -o //rendered/south.png -f 1 \
-S Szene-West -o //rendered/west.png -f 1 \
-S Szene-Ost -o //rendered/east.png -f 1