#ifndef _I_GRAPHICS_MGR_H
#define _I_GRAPHICS_MGR_H

#include "game/Player.h"
#include "graphics/graphic/IAnimation.h"
#include "graphics/graphic/IGraphic.h"
#include "graphics/graphic/IMapObjectGraphic.h"
#include "graphics/graphic/IPlainGraphic.h"
#include "graphics/renderer/IRenderer.h"
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"

enum GoodsType : char;
enum StructureType : unsigned char;

class ConfigMgr;

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
    ADD_BUILDING_FORESTERS,
    ADD_BUILDING_HUNTERS_HUT,
    ADD_BUILDING_DUMMY,

    // Grafiken für die Gruppen vom "Gebäude bauen"-Menü
    ADD_BUILDING_GROUP_CRAFTSMAN,
    ADD_BUILDING_GROUP_CRAFTSMAN_PRESSED,
    ADD_BUILDING_GROUP_FARM,
    ADD_BUILDING_GROUP_FARM_PRESSED,
    ADD_BUILDING_GROUP_PORT,
    ADD_BUILDING_GROUP_PORT_PRESSED,
    ADD_BUILDING_GROUP_PUBLIC,
    ADD_BUILDING_GROUP_PUBLIC_PRESSED,

    // Plus-Zeichen und Pfeil für Produktion eines Gebäudes
    PRODUCTION_ARROW,
    PRODUCTION_PLUS,

    // Wappen
    COAT_OF_ARMS_POPULATION,
    COAT_OF_ARMS_POPULATION_RED = COAT_OF_ARMS_POPULATION + PLAYER_RED,
    COAT_OF_ARMS_POPULATION_BLUE = COAT_OF_ARMS_POPULATION + PLAYER_BLUE,
    COAT_OF_ARMS_POPULATION_YELLOW = COAT_OF_ARMS_POPULATION + PLAYER_YELLOW,
    COAT_OF_ARMS_POPULATION_WHITE = COAT_OF_ARMS_POPULATION + PLAYER_WHITE,

    // Optionen-Panel
    MUSIC,
    MUSIC_PRESSED,

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
class IGraphicsMgr {

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

public:
    /**
     * @brief Entlädt die Grafiken und gibt den Speicher wieder frei
     */
    virtual ~IGraphicsMgr() {}

    /**
     * @brief Lädt alle Grafiken für das Spiel.
     * Info: Das Laden der Grafiken kann nicht im Konstruktur gemacht werden, weil wir hierzu virtuelle Methoden
     * aufrufen müssen und das im Konstruktor nicht funktioniert.
     */
    virtual void loadGraphics() = 0;

    /**
     * @brief Liefert eine Tile-Grafik
     * @param tileIndex Index ins Array #tiles dessen Eintrag zurückgeliefert werden soll
     * @return MapObjectGraphic
     */
    virtual IMapObjectGraphic* const getGraphicForTile(int tileIndex) const = 0;

    /**
     * @brief Liefert eine Object-Grafik
     * @param index Index ins Array #objects dessen Eintrag zurückgeliefert werden soll
     * @return MapObjectGraphic
     */
    virtual IMapObjectGraphic* const getGraphicForStructure(StructureType structureType) const = 0;

    /**
     * @brief Liefert die Grafik für ein Güter-Symbol zurück
     * @param goodsType Güter-Typ
     * @return PlainGraphic
     */
    virtual IPlainGraphic* const getGraphicForGoodsIcon(GoodsType goodsType)  const = 0;

    /**
     * @brief Liefert die Grafik für ein Güter-Symbol (Marktplatz-Symbol) zurück
     * @param goodsType Güter-Typ
     * @return PlainGraphic
     */
    virtual IPlainGraphic* const getGraphicForGoodsMarketplaceIcon(GoodsType goodsType) const = 0;

    /**
     * @brief Liefert eine sonstige Grafik zurück
     * @param otherGraphic welche Grafikb
     * @return PlainGraphic
     */
    virtual IPlainGraphic* const getOtherGraphic(OtherGraphic otherGraphic) const = 0;

    /**
     * @brief Liefert eine Animation zurück
     * @param animationType welche Animation
     * @return Animation
     */
    virtual IAnimation* const getAnimation(AnimationType animationType) const = 0;

    /**
     * @brief Liefert den Renderer zurück.
     * @return Renderer
     */
    virtual IRenderer* const getRenderer() const = 0;

};

#endif
