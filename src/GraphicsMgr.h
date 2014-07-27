#ifndef _GRAPHICS_MGR_H
#define _GRAPHICS_MGR_H

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include "Graphic.h"

/**
 * @brief Manager, der alle Grafiken verwaltet
 */
class GraphicsMgr {

public:
	/**
	 * @brief Größe einer Kachel
	 */
	static const int TILE_WIDTH = 64;
	static const int TILE_HEIGHT = 32;
	static const int TILE_WIDTH_HALF = TILE_WIDTH / 2;
	static const int TILE_HEIGHT_HALF = TILE_HEIGHT / 2;

private:
	/**
	 * @brief Array von Zeigern auf die Tile-Grafiken
	 */
	Graphic** tiles;

	/**
	 * @brief Array von Zeigern auf die Objekt-Grafiken
	 */
	Graphic** objects;

public:
	GraphicsMgr();
	~GraphicsMgr();

	/**
	 * @brief Liefert eine Tile-Grafik
	 * @param index Index ins Array #tiles dessen Eintrag zurückgeliefert werden soll
	 */
	Graphic* const getTile(int index) const {
		return tiles[index];
	}

	/**
	 * @brief Liefert eine Object-Grafik
	 * @param index Index ins Array #objects dessen Eintrag zurückgeliefert werden soll
	 */
	Graphic* const getObject(int index) const {
		return objects[index];
	}
};

#endif
