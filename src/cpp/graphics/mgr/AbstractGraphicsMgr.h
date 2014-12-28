#ifndef _ABSTRACT_GRAPHICS_MGR_H
#define _ABSTRACT_GRAPHICS_MGR_H

#include "config/ConfigMgr.h"
#include "game/Player.h"
#include "graphics/graphic/IAnimation.h"
#include "graphics/graphic/IGraphic.h"
#include "graphics/graphic/IMapObjectGraphic.h"
#include "graphics/graphic/IPlainGraphic.h"
#include "graphics/mgr/IGraphicsMgr.h"
#include "graphics/renderer/IRenderer.h"
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"



/**
 * @brief Abstrakter Grafik-Manager, der die gemeinsame Funktionalität zwischen dem
 * `SDLGraphicsMgr` und dem `NoSDLGraphicsMgr` enthält.
 */
class AbstractGraphicsMgr : public IGraphicsMgr {

protected:
    // Dependencies
    IRenderer* const renderer;
    const ConfigMgr* const configMgr;

	/**
	 * @brief Array von Zeigern auf die Tile-Grafiken
	 */
    IMapObjectGraphic** tiles;

	/**
	 * @brief Array von Zeigern auf die Struktur-Grafiken
	 */
	IMapObjectGraphic** structures;

    /**
	 * @brief Array von Zeigern auf die Gütergrafiken (Symbole)
	 */
    IPlainGraphic** goodsIcons;

    /**
     * @brief Array von Zeigern auf die Gütergrafiken (Marketplatz-Symbole)
     */
    IPlainGraphic** goodsMarketplaceIcons;

    /**
     * @brief Array von Zeigern auf andere Grafiken
     */
    IPlainGraphic** otherGraphics;

    /**
     * @brief Array von Zeigern auf Animationen
     */
    IAnimation** animations;

public:
    /**
     * @brief Konstruktor. Zum Laden der Grafiken muss anschließend `loadGraphics()` aufgerufen werden.
     * @param renderer (Dependency)
     * @param configMgr (Dependency)
     */
    AbstractGraphicsMgr(IRenderer* const renderer, const ConfigMgr* const configMgr) :
        renderer(renderer), configMgr(configMgr) {}

    /**
     * @brief Entlädt die Grafiken und gibt den Speicher wieder frei.
     */
    virtual ~AbstractGraphicsMgr();

    virtual void loadGraphics();


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

protected:
    /**
	 * @brief Lädt die Tile-Grafiken
	 */
	void loadTiles();


    /**
     * @brief Lädt eine einfache Grafik. Implementierungen wählen die entsprechende Grafik-Implementierung.
     * @param filename Dateiname der zu ladenden Grafik
     */
    virtual IPlainGraphic* loadPlainGraphic(const char* filename) = 0;

    /**
     * @brief Lädt eine Grafik für ein MapObject. Implementierungen wählen die entsprechende Grafik-Implementierung.
     * @param filename Dateiname der zu ladenden Grafik
     * @param mapWidth Breite in mapCoords
     * @param mapHeight Höhe in mapCoords
     */
    virtual IMapObjectGraphic* loadMapObjectGraphic(
        const char* filename, unsigned char mapWidth, unsigned char mapHeight) = 0;

    /**
     * @brief Lädt eine Grafik für eine Animation. Implementierungen wählen die entsprechende Grafik-Implementierung.
     * @param filename Dateiname der zu ladenden Animationsgrafik
     * @param mapWidth Breite in mapCoords
     * @param mapHeight Höhe in mapCoords
     * @param framesCount Anzahl der Frames, die sich in dieser Datei befinden
     * @param fps FPS = Geschwindigkeit, wie viele Frames pro Sekunde abgespielt werden sollen
     */
    virtual IAnimation* loadAnimation(const char* filename, unsigned char mapWidth, unsigned char mapHeight,
                                      unsigned int framesCount, double fps) = 0;

};

#endif
