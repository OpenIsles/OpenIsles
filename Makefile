SDL_CFLAGS = `sdl2-config --cflags`
SDL_LDFLAGS = `sdl2-config --libs`

SRC_DIRECTORY = src
DATA_DIRECTORY = data
BUILD_DIRECTORY = build
SRC_FILENAMES = main.cpp Map.cpp GraphicsMgr.cpp Graphic.cpp
SRC_FILES = $(SRC_FILENAMES:%.cpp=$(SRC_DIRECTORY)/%.cpp)
OBJ_FILES = $(SRC_FILENAMES:%.cpp=$(BUILD_DIRECTORY)/%.o)

CC = /usr/bin/g++
CFLAGS = -std=c++11 -Wall -Werror -pedantic -g -O0 $(SDL_CFLAGS)
LDFLAGS = $(SDL_LDFLAGS) -lSDL2_image -lSDL2_ttf


.PHONY: all clean

all: $(BUILD_DIRECTORY) $(BUILD_DIRECTORY)/OpenIsles $(BUILD_DIRECTORY)/tileset.png

clean:
	rm -rf $(BUILD_DIRECTORY)
	
$(BUILD_DIRECTORY):
	mkdir -p $@

$(BUILD_DIRECTORY)/%.o: $(SRC_DIRECTORY)/%.cpp | $(BUILD_DIRECTORY)
	$(CC) $(CFLAGS) -o $@ -c $<
	
$(BUILD_DIRECTORY)/OpenIsles: $(OBJ_FILES) | $(BUILD_DIRECTORY)
	$(CC) $(CFLAGS) -o $@ $+ $(LDFLAGS)

# TODO Datei mit Zuordnung der Tile-Indexes, damit sie nicht nur von der Dateinamen-Sortierung abhängig sind
$(BUILD_DIRECTORY)/tileset.png: $(DATA_DIRECTORY)/img/tiles/*
	montage -background transparent $+ -tile 2x2 -geometry 64x32 $@
