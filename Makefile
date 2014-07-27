SDL_CFLAGS = `sdl2-config --cflags`
SDL_LDFLAGS = `sdl2-config --libs`

SRC_DIRECTORY = src
DATA_DIRECTORY = data
BUILD_DIRECTORY = build
SRC_FILENAMES = main.cpp Map.cpp GraphicsMgr.cpp Graphic.cpp
SRC_FILES = $(SRC_FILENAMES:%.cpp=$(SRC_DIRECTORY)/%.cpp)
OBJ_FILES = $(SRC_FILENAMES:%.cpp=$(BUILD_DIRECTORY)/%.o)

# Für das fertige Tileset für Tiled: Anzahl Kacheln pro Zeile
TILESET_WIDTH = 4

CC = /usr/bin/g++
CFLAGS = -std=c++11 -Wall -Werror -pedantic -g -O0 $(SDL_CFLAGS)
LDFLAGS = $(SDL_LDFLAGS) -lSDL2_image -lSDL2_ttf


.PHONY: all clean build-tiles clean-tiles

all: $(BUILD_DIRECTORY) $(BUILD_DIRECTORY)/OpenIsles

clean: clean-tiles
	rm -rf $(BUILD_DIRECTORY)

$(BUILD_DIRECTORY):
	mkdir $@
	
$(BUILD_DIRECTORY)/%.o: $(SRC_DIRECTORY)/%.cpp | $(BUILD_DIRECTORY)
	$(CC) $(CFLAGS) -o $@ -c $<
	
$(BUILD_DIRECTORY)/OpenIsles: $(OBJ_FILES) | $(BUILD_DIRECTORY)
	$(CC) $(CFLAGS) -o $@ $+ $(LDFLAGS)
	
build-tiles:
	awk 'NF > 0 && /^[^#]/ { print "convert $(DATA_DIRECTORY)/img/tiles/tileset-src.xcf -crop 64x32+" $$2 "+" $$3 " $(DATA_DIRECTORY)/img/tiles/alpha-mask.png -alpha Off -compose CopyOpacity -composite $(DATA_DIRECTORY)/img/tiles/" $$4 }' $(DATA_DIRECTORY)/img/tiles/tiles.txt | bash
	convert -size $$(($(TILESET_WIDTH)*64))x256 canvas:transparent $(DATA_DIRECTORY)/img/tiles/tileset.png
	awk 'NF > 0 && /^[^#]/ { print "convert $(DATA_DIRECTORY)/img/tiles/tileset.png $(DATA_DIRECTORY)/img/tiles/" $$4 " -geometry +" (($$1-1)%$(TILESET_WIDTH))*64 "+" int(($$1-1)/$(TILESET_WIDTH))*32 " -composite $(DATA_DIRECTORY)/img/tiles/tileset.png" }' $(DATA_DIRECTORY)/img/tiles/tiles.txt | bash
	
clean-tiles:
	awk 'NF > 0 && /^[^#]/ { print "rm -f $(DATA_DIRECTORY)/img/tiles/" $$4 }' $(DATA_DIRECTORY)/img/tiles/tiles.txt | bash
	rm -f $(DATA_DIRECTORY)/img/tiles/tileset.png