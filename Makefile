SDL_CFLAGS = `sdl-config --cflags`
SDL_LDFLAGS = `sdl-config --libs`

BUILD_DIRECTORY = build

CC = /usr/bin/g++
CFLAGS = -Wall -Werror -g -O0 $(SDL_CFLAGS)
LDFLAGS = $(SDL_LDFLAGS)


.PHONY: all clean

all: $(BUILD_DIRECTORY) $(BUILD_DIRECTORY)/OpenIsles

clean:
	rm -rf $(BUILD_DIRECTORY)
	
$(BUILD_DIRECTORY):
	mkdir -p $@

$(BUILD_DIRECTORY)/%.o: %.cpp | $(BUILD_DIRECTORY)
	$(CC) $(CFLAGS) -o $@ -c $<
	
$(BUILD_DIRECTORY)/OpenIsles: $(BUILD_DIRECTORY)/main.o | $(BUILD_DIRECTORY)
	$(CC) $(CFLAGS) -o $@ $+ $(LDFLAGS)
