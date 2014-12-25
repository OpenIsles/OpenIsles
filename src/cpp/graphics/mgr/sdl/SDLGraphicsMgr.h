#ifndef _SDL_GRAPHICS_MGR_H
#define _SDL_GRAPHICS_MGR_H

#include "game/Player.h"
#include "graphics/graphic/sdl/SDLAnimation.h"
#include "graphics/graphic/sdl/SDLGraphic.h"
#include "graphics/graphic/sdl/SDLMapObjectGraphic.h"
#include "graphics/graphic/sdl/SDLPlainGraphic.h"
#include "graphics/mgr/IGraphicsMgr.h"
#include "graphics/renderer/IRenderer.h"
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"



/**
 * @brief Manager, der alle Grafiken verwaltet
 */
class SDLGraphicsMgr : public IGraphicsMgr {

public:
    /**
     * @brief Breite einer Kachel in Pixel
     */
	static const int TILE_WIDTH = 64;

    /**
     * @brief Höhe einer Kachel in Pixel
     */
	static const int TILE_HEIGHT = 32;

    /**
     * @brief Halbe Breite einer Kachel in Pixel
     */
	static const int TILE_WIDTH_HALF = TILE_WIDTH / 2;

    /**
     * @brief Halbe Höhe einer Kachel in Pixel
     */
	static const int TILE_HEIGHT_HALF = TILE_HEIGHT / 2;

    /**
     * @brief Höhe der Elevation in Pixel. Entspricht dem e in doc/map-coords.xcf
     */
    static const int ELEVATION_HEIGHT = 32;

private:
    // Dependencies
    IRenderer* const renderer;
    const ConfigMgr* const configMgr;

	/**
	 * @brief Array von Zeigern auf die Tile-Grafiken
	 */
    SDLMapObjectGraphic** tiles;

	/**
	 * @brief Array von Zeigern auf die Struktur-Grafiken
	 */
	SDLMapObjectGraphic** structures;
    
    /**
	 * @brief Array von Zeigern auf die Gütergrafiken (Symbole)
	 */
    SDLPlainGraphic** goodsIcons;

    /**
     * @brief Array von Zeigern auf die Gütergrafiken (Marketplatz-Symbole)
     */
    SDLPlainGraphic** goodsMarketplaceIcons;
    
    /**
     * @brief Array von Zeigern auf andere Grafiken
     */
    SDLPlainGraphic** otherGraphics;

    /**
     * @brief Array von Zeigern auf Animationen
     */
    SDLAnimation** animations;

public:
    /**
     * @brief Lädt alle Grafiken für das Spiel
     * @param renderer (Dependency)
     * @param configMgr (Dependency)
     */
    SDLGraphicsMgr(IRenderer* const renderer, const ConfigMgr* const configMgr);
    ~SDLGraphicsMgr();

    IMapObjectGraphic* const getGraphicForTile(int tileIndex) const {
		return tiles[tileIndex];
	}

    IMapObjectGraphic* const getGraphicForStructure(StructureType structureType) const {
		return structures[structureType];
	}
    
    IPlainGraphic* const getGraphicForGoodsIcon(GoodsType goodsType) const {
        return goodsIcons[goodsType];
    }

    IPlainGraphic* const getGraphicForGoodsMarketplaceIcon(GoodsType goodsType) const {
        return goodsMarketplaceIcons[goodsType];
    }
    
    IPlainGraphic* const getOtherGraphic(OtherGraphic otherGraphic) const {
        return otherGraphics[otherGraphic];
    }

    IAnimation* const getAnimation(AnimationType animationType) const {
        return animations[animationType];
    }

    IRenderer* const getRenderer() const {
        return renderer;
    }

private:
	/**
	 * @brief Lädt die Tile-Grafiken
	 */
	void loadTiles();

};

#endif
