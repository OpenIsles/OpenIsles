#ifndef _MAP_H
#define _MAP_H

#include <SDL.h>
#include <iostream>
#include <string.h>


class Map {

private:
	/**
	 * @brief Breite der Karte in Kacheln
	 */
	unsigned int width;

	/**
	 * @brief Breite der Karte in Kacheln
	 */
	unsigned int height;

	/**
	 * @brief Array (height x width) mit den Kacheln
	 */
	unsigned char* tiles;

public:
	Map(unsigned int width, unsigned int height) : width(width), height(height) {
		tiles = new unsigned char[width * height];

		// TODO Karte laden statt einfach nur leerzunullen
		memset(tiles, 0, width * height * sizeof(unsigned char));
	}

	~Map() {
		delete tiles;
	}

	unsigned int getHeight() const {
		return height;
	}

	unsigned int getWidth() const {
		return width;
	}

	unsigned char getTileAt(unsigned int x, unsigned int y) const {
		if (x >= width || y >= height) {
			std::cerr << "mapCoords (" << std::to_string(x) << ", " + std::to_string(y) << ") out of bounds";
		}

		return tiles[y * width + x];
	}

	/**
	 * @brief Rendert die Karte.
	 * @param renderer SDL-Renderer, auf den gezeichnet wird
	 */
	// TODO durch Graphics- oder Render-Manager abstrahieren, damit die Karte nicht die Grafiken kennen muss
	void renderMap(SDL_Renderer* renderer);

};

#endif
