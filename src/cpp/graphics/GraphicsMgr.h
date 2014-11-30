#ifndef _GRAPHICS_MGR_H
#define _GRAPHICS_MGR_H

#include <SDL.h>
#include <SDL_image.h>
#include "game/Player.h"
#include "graphics/Animation.h"
#include "graphics/Graphic.h"
#include "graphics/MapObjectGraphic.h"
#include "graphics/PlainGraphic.h"
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"

enum GoodsType : char;
enum StructureType : unsigned char;

typedef
enum OtherGraphic {
    // Münzensymbol
    COINS,

    // Hintergrund des Panels
    PANEL,

    // Hintergrund der Statusbar
    STATUSBAR,

    // Gebäude bauen - Grid mit den Symbolen
    ADD_BUILDING_GRID,

    // Grafiken fürs "Gebäude bauen"-Menü
    ADD_BUILDING_CHAPEL,
    ADD_BUILDING_MARKETPLACE,
    ADD_BUILDING_OFFICE1,
    ADD_BUILDING_STREET,
    ADD_BUILDING_SHEEP_FARM,
    ADD_BUILDING_WEAVING_MILL1,
    ADD_BUILDING_CATTLE_FARM,
    ADD_BUILDING_BUTCHERS,
    ADD_BUILDING_TOOLSMITHS,
    ADD_BUILDING_HOUSE,
    ADD_BUILDING_STONEMASON,
    ADD_BUILDING_DUMMY,

    // Plus-Zeichen und Pfeil für Produktion eines Gebäudes
    PRODUCTION_ARROW,
    PRODUCTION_PLUS,

    // Wappen
    COAT_OF_ARMS_POPULATION,
    COAT_OF_ARMS_POPULATION_RED = COAT_OF_ARMS_POPULATION + PLAYER_RED,
    COAT_OF_ARMS_POPULATION_BLUE = COAT_OF_ARMS_POPULATION + PLAYER_BLUE,
    COAT_OF_ARMS_POPULATION_YELLOW = COAT_OF_ARMS_POPULATION + PLAYER_YELLOW,
    COAT_OF_ARMS_POPULATION_WHITE = COAT_OF_ARMS_POPULATION + PLAYER_WHITE,
            
    MAX_GRAPHIC // Marker, wie viele Grafiken es gibt
} OtherGraphic;


typedef
enum AnimationType {
    // Typ, der Waren hin- und herträgt
    CARRIER,

    // Marktkarren
    CART_WITHOUT_CARGO,
    CART_WITH_CARGO,

    MAX_ANIMATION // Marker, wie viele Animationen es gibt
} AnimationType;


/**
 * @brief Manager, der alle Grafiken verwaltet
 */
class GraphicsMgr {

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
	/**
	 * @brief Array von Zeigern auf die Tile-Grafiken
	 */
    MapObjectGraphic** tiles;

	/**
	 * @brief Array von Zeigern auf die Struktur-Grafiken
	 */
	MapObjectGraphic** structures;
    
    /**
	 * @brief Array von Zeigern auf die Gütergrafiken (Symbole)
	 */
    PlainGraphic** goodsIcons;

    /**
     * @brief Array von Zeigern auf die Gütergrafiken (Marketplatz-Symbole)
     */
    PlainGraphic** goodsMarketplaceIcons;
    
    /**
     * @brief Array von Zeigern auf andere Grafiken
     */
    PlainGraphic** otherGraphics;

    /**
     * @brief Array von Zeigern auf Animationen
     */
    Animation** animations;

public:
    /**
     * @brief Lädt alle Grafiken für das Spiel
     */
	GraphicsMgr();

    /**
     * @brief Entlädt die Grafiken und gibt den Speicher wieder frei
     */
	~GraphicsMgr();

	/**
	 * @brief Liefert eine Tile-Grafik
	 * @param tileIndex Index ins Array #tiles dessen Eintrag zurückgeliefert werden soll
     * @return MapObjectGraphic
	 */
    MapObjectGraphic* const getGraphicForTile(int tileIndex) const {
		return tiles[tileIndex];
	}

	/**
	 * @brief Liefert eine Object-Grafik
	 * @param index Index ins Array #objects dessen Eintrag zurückgeliefert werden soll
     * @return MapObjectGraphic
	 */
    MapObjectGraphic* const getGraphicForStructure(StructureType structureType) const {
		return structures[structureType];
	}
    
    /**
     * @brief Liefert die Grafik für ein Güter-Symbol zurück
     * @param goodsType Güter-Typ
     * @return PlainGraphic
     */
    PlainGraphic* const getGraphicForGoodsIcon(GoodsType goodsType) const {
        return goodsIcons[goodsType];
    }

    /**
     * @brief Liefert die Grafik für ein Güter-Symbol (Marktplatz-Symbol) zurück
     * @param goodsType Güter-Typ
     * @return PlainGraphic
     */
    PlainGraphic* const getGraphicForGoodsMarketplaceIcon(GoodsType goodsType) const {
        return goodsMarketplaceIcons[goodsType];
    }
    
    /**
     * @brief Liefert eine sonstige Grafik zurück
     * @param otherGraphic welche Grafik
     * @return PlainGraphic
     */
    PlainGraphic* const getOtherGraphic(OtherGraphic otherGraphic) const {
        return otherGraphics[otherGraphic];
    }

    /**
     * @brief Liefert eine Animation zurück
     * @param animationType welche Animation
     * @return Animation
     */
    Animation* const getAnimation(AnimationType animationType) const {
        return animations[animationType];
    }

private:
	/**
	 * @brief Lädt die Tile-Grafiken
	 */
	void loadTiles();

};

#endif
