#ifndef _GRAPHICS_MGR_H
#define _GRAPHICS_MGR_H

#include <SDL.h>
#include <SDL_image.h>
#include "Graphic.h"

enum GoodsType : unsigned char;
enum StructureType : unsigned char;

typedef
enum OtherGraphic {
    // Münzensymbol
    COINS = 1,
            
    MAX_GRAPHIC // Marker, wie viele Grafiken es gibt
} OtherGraphic;

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
    
    /**
	 * @brief Array von Zeigern auf die Gütergrafiken (Symbole)
	 */
	Graphic** goodsIcons;
    
    /**
     * @brief Array von Zeigern auf andere Grafiken
     */
    Graphic** otherGraphics;

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
    
    /**
     * @brief Liefert die Grafik für ein Güter-Symbol zurück
     * @param goodsType Güter-Typ
     * @return Graphic
     */
    Graphic* const getGraphicForGoodsIcon(GoodsType goodsType) const {
        return goodsIcons[goodsType];
    }
    
    /**
     * @brief Liefert eine sonstige Grafik zurück
     * @param otherGraphic welche Grafik
     * @return Graphic
     */
    Graphic* const getOtherGraphic(OtherGraphic otherGraphic) const {
        return otherGraphics[otherGraphic];
    }

private:
	/**
	 * @brief Lädt die Tile-Grafiken
	 */
	void loadTiles();

};

#endif
