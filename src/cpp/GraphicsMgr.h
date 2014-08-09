#ifndef _GRAPHICS_MGR_H
#define _GRAPHICS_MGR_H

#include <SDL.h>
#include <SDL_image.h>
#include "Graphic.h"

enum StructureType : unsigned char;

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
	 * @brief Array von Zeigern auf die Struktur-Grafiken
	 */
	Graphic** structures;

public:
	GraphicsMgr();
	~GraphicsMgr();

	/**
	 * @brief Liefert eine Tile-Grafik
	 * @param index Index ins Array #tiles dessen Eintrag zurückgeliefert werden soll
	 */
	Graphic* const getGraphicForTile(int index) const {
		return tiles[index];
	}

	/**
	 * @brief Liefert eine Object-Grafik
	 * @param index Index ins Array #objects dessen Eintrag zurückgeliefert werden soll
	 */
	Graphic* const getGraphicForStructure(StructureType structureType) const {
		return structures[structureType];
	}

private:
	/**
	 * @brief Lädt die Tile-Grafiken
	 */
	void loadTiles();

};

#endif
