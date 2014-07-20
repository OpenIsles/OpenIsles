SDL_CFLAGS = `sdl2-config --cflags`
SDL_LDFLAGS = `sdl2-config --libs`

BUILD_DIRECTORY = build

CC = /usr/bin/g++
CFLAGS = -std=c++11 -Wall -Werror -pedantic -g -O0 $(SDL_CFLAGS)
LDFLAGS = $(SDL_LDFLAGS) -lSDL2_image -lSDL2_ttf


.PHONY: all clean

all: $(BUILD_DIRECTORY) $(BUILD_DIRECTORY)/OpenIsles

clean:
	rm -rf $(BUILD_DIRECTORY)
	
$(BUILD_DIRECTORY):
	mkdir -p $@

$(BUILD_DIRECTORY)/%.o: %.cpp | $(BUILD_DIRECTORY)
	$(CC) $(CFLAGS) -o $@ -c $<
	
$(BUILD_DIRECTORY)/OpenIsles: $(BUILD_DIRECTORY)/main.o $(BUILD_DIRECTORY)/Map.o | $(BUILD_DIRECTORY)
	$(CC) $(CFLAGS) -o $@ $+ $(LDFLAGS)
