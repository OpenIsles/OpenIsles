#ifndef _I_GRAPHICS_MGR_H
#define _I_GRAPHICS_MGR_H

#include <string>
#include "defines.h"
#include "game/GoodsSlot.h"
#include "game/Player.h"
#include "graphics/graphic/IGraphic.h"
#include "graphics/graphic/GraphicSet.h"
#include "graphics/renderer/IRenderer.h"
#include "map/MapObjectType.h"

class ConfigMgr;


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

    virtual const GraphicSet* getGraphicSet(std::string graphicName) const = 0;

    VIRTUAL_ONLY_IN_TESTS
    std::string getGraphicSetNameForMapObject(MapObjectType mapObjectType) const {
        // TODO Das muss irgendwie hübscher werden. In Zukunft muss alles mit den String-Keys laufen
        std::string graphicSetNamePrefix;
        std::string graphicSetNameSuffix;

        if (mapObjectType == MapObjectType::NORTHERN_FOREST1) { graphicSetNameSuffix = "northern-forest1"; }
        else if (mapObjectType == MapObjectType::NORTHERN_FOREST2) { graphicSetNameSuffix = "northern-forest2"; }
        else if (mapObjectType == MapObjectType::SUGARCANE_FIELD) { graphicSetNameSuffix = "sugarcane-field"; }
        else if (mapObjectType == MapObjectType::TOBACCO_FIELD) { graphicSetNameSuffix = "tobacco-field"; }

        else if (mapObjectType == MapObjectType::CHAPEL) { graphicSetNameSuffix = "chapel"; }
        else if (mapObjectType == MapObjectType::PIONEERS_HOUSE1) { graphicSetNameSuffix = "pioneers-house1"; }
        else if (mapObjectType == MapObjectType::PIONEERS_HOUSE2) { graphicSetNameSuffix = "pioneers-house2"; }
        else if (mapObjectType == MapObjectType::PIONEERS_HOUSE3) { graphicSetNameSuffix = "pioneers-house3"; }
        else if (mapObjectType == MapObjectType::PIONEERS_HOUSE4) { graphicSetNameSuffix = "pioneers-house4"; }
        else if (mapObjectType == MapObjectType::PIONEERS_HOUSE5) { graphicSetNameSuffix = "pioneers-house5"; }
        else if (mapObjectType == MapObjectType::SETTLERS_HOUSE1) { graphicSetNameSuffix = "settlers-house1"; }
        else if (mapObjectType == MapObjectType::SETTLERS_HOUSE2) { graphicSetNameSuffix = "settlers-house2"; }
        else if (mapObjectType == MapObjectType::SETTLERS_HOUSE3) { graphicSetNameSuffix = "settlers-house3"; }
        else if (mapObjectType == MapObjectType::SETTLERS_HOUSE4) { graphicSetNameSuffix = "settlers-house4"; }
        else if (mapObjectType == MapObjectType::SETTLERS_HOUSE5) { graphicSetNameSuffix = "settlers-house5"; }
        else if (mapObjectType == MapObjectType::STONEMASON) { graphicSetNameSuffix = "stonemason"; }
        else if (mapObjectType == MapObjectType::OFFICE1) { graphicSetNameSuffix = "office1"; }
        else if (mapObjectType == MapObjectType::OFFICE2) { graphicSetNameSuffix = "office2"; }
        else if (mapObjectType == MapObjectType::MARKETPLACE) { graphicSetNameSuffix = "marketplace"; }
        else if (mapObjectType == MapObjectType::FORESTERS) { graphicSetNameSuffix = "foresters"; }
        else if (mapObjectType == MapObjectType::SHEEP_FARM) { graphicSetNameSuffix = "sheep-farm"; }
        else if (mapObjectType == MapObjectType::WEAVING_MILL1) { graphicSetNameSuffix = "weaving-mill1"; }
        else if (mapObjectType == MapObjectType::CATTLE_FARM) { graphicSetNameSuffix = "cattle-farm"; }
        else if (mapObjectType == MapObjectType::BUTCHERS) { graphicSetNameSuffix = "butchers"; }
        else if (mapObjectType == MapObjectType::TOOLSMITHS) { graphicSetNameSuffix = "toolsmiths"; }
        else if (mapObjectType == MapObjectType::HUNTERS_HUT) { graphicSetNameSuffix = "hunters-hut"; }
        else if (mapObjectType == MapObjectType::CATHEDRAL) { graphicSetNameSuffix = "cathedral"; }
        else if (mapObjectType == MapObjectType::TAVERN) { graphicSetNameSuffix = "tavern"; }
        else if (mapObjectType == MapObjectType::SUGARCANE_PLANTATION) { graphicSetNameSuffix = "sugarcane-plantation"; }
        else if (mapObjectType == MapObjectType::TOBACCO_PLANTATION) { graphicSetNameSuffix = "tobacco-plantation"; }
        else if (mapObjectType == MapObjectType::SQUARE1) { graphicSetNameSuffix = "square1"; }
        else if (mapObjectType == MapObjectType::SQUARE2) { graphicSetNameSuffix = "square2"; }
        else if (mapObjectType == MapObjectType::SQUARE3) { graphicSetNameSuffix = "square3"; }

        else if (mapObjectType == MapObjectType::COBBLED_STREET_STRAIGHT_0) { graphicSetNameSuffix = "cobbled-street-straight0"; }
        else if (mapObjectType == MapObjectType::COBBLED_STREET_STRAIGHT_90) { graphicSetNameSuffix = "cobbled-street-straight90"; }
        else if (mapObjectType == MapObjectType::COBBLED_STREET_CURVE_0) { graphicSetNameSuffix = "cobbled-street-curve0"; }
        else if (mapObjectType == MapObjectType::COBBLED_STREET_CURVE_90) { graphicSetNameSuffix = "cobbled-street-curve90"; }
        else if (mapObjectType == MapObjectType::COBBLED_STREET_CURVE_180) { graphicSetNameSuffix = "cobbled-street-curve180"; }
        else if (mapObjectType == MapObjectType::COBBLED_STREET_CURVE_270) { graphicSetNameSuffix = "cobbled-street-curve270"; }
        else if (mapObjectType == MapObjectType::COBBLED_STREET_TEE_0) { graphicSetNameSuffix = "cobbled-street-tee0"; }
        else if (mapObjectType == MapObjectType::COBBLED_STREET_TEE_90) { graphicSetNameSuffix = "cobbled-street-tee90"; }
        else if (mapObjectType == MapObjectType::COBBLED_STREET_TEE_180) { graphicSetNameSuffix = "cobbled-street-tee180"; }
        else if (mapObjectType == MapObjectType::COBBLED_STREET_TEE_270) { graphicSetNameSuffix = "cobbled-street-tee270"; }
        else if (mapObjectType == MapObjectType::COBBLED_STREET_CROSS) { graphicSetNameSuffix = "cobbled-street-cross"; }

        else if (mapObjectType == MapObjectType::FARM_ROAD_STRAIGHT_0) { graphicSetNameSuffix = "farm-road-straight0"; }
        else if (mapObjectType == MapObjectType::FARM_ROAD_STRAIGHT_90) { graphicSetNameSuffix = "farm-road-straight90"; }
        else if (mapObjectType == MapObjectType::FARM_ROAD_CURVE_0) { graphicSetNameSuffix = "farm-road-curve0"; }
        else if (mapObjectType == MapObjectType::FARM_ROAD_CURVE_90) { graphicSetNameSuffix = "farm-road-curve90"; }
        else if (mapObjectType == MapObjectType::FARM_ROAD_CURVE_180) { graphicSetNameSuffix = "farm-road-curve180"; }
        else if (mapObjectType == MapObjectType::FARM_ROAD_CURVE_270) { graphicSetNameSuffix = "farm-road-curve270"; }
        else if (mapObjectType == MapObjectType::FARM_ROAD_TEE_0) { graphicSetNameSuffix = "farm-road-tee0"; }
        else if (mapObjectType == MapObjectType::FARM_ROAD_TEE_90) { graphicSetNameSuffix = "farm-road-tee90"; }
        else if (mapObjectType == MapObjectType::FARM_ROAD_TEE_180) { graphicSetNameSuffix = "farm-road-tee180"; }
        else if (mapObjectType == MapObjectType::FARM_ROAD_TEE_270) { graphicSetNameSuffix = "farm-road-tee270"; }
        else if (mapObjectType == MapObjectType::FARM_ROAD_CROSS) { graphicSetNameSuffix = "farm-road-cross"; }

        graphicSetNamePrefix = (mapObjectType >= MapObjectType::START_STRUCTURES) ? "structures" : "harvestables";

        return graphicSetNamePrefix + "/" + graphicSetNameSuffix;
    }

    std::string getGraphicSetNameForGoodIcons(GoodsType goodsType, bool marketplaceIcon) const {
        std::string graphicSetNamePrefix = (marketplaceIcon) ? "goods-marketplace-icons" : "goods-icons";

        // TODO Das muss irgendwie hübscher werden. In Zukunft muss alles mit den String-Keys laufen
        std::string graphicSetNameSuffix;

        if (goodsType == GoodsType::WOOL) { graphicSetNameSuffix = "wool"; }
        else if (goodsType == GoodsType::SUGARCANE) { graphicSetNameSuffix = "sugarcane"; }
        else if (goodsType == GoodsType::TOBACCO) { graphicSetNameSuffix = "tobacco"; }
        else if (goodsType == GoodsType::CATTLE) { graphicSetNameSuffix = "cattle"; }
        else if (goodsType == GoodsType::FOOD) { graphicSetNameSuffix = "food"; }
        else if (goodsType == GoodsType::ALCOHOL) { graphicSetNameSuffix = "alcohol"; }
        else if (goodsType == GoodsType::CLOTH) { graphicSetNameSuffix = "cloth"; }
        else if (goodsType == GoodsType::TOOLS) { graphicSetNameSuffix = "tools"; }
        else if (goodsType == GoodsType::WOOD) { graphicSetNameSuffix = "wood"; }
        else if (goodsType == GoodsType::BRICKS) { graphicSetNameSuffix = "bricks"; }

        return graphicSetNamePrefix + "/" + graphicSetNameSuffix;
    }

    std::string getGraphicSetNameForCoatOfArmsPopulation(PlayerColor playerColor) const {
        std::string graphicSetNameSuffix;

        // TODO Das muss irgendwie hübscher werden. In Zukunft muss alles mit den String-Keys laufen
        if (playerColor == PlayerColor::RED) { graphicSetNameSuffix = "red"; }
        else if (playerColor == PlayerColor::BLUE) { graphicSetNameSuffix = "blue"; }
        else if (playerColor == PlayerColor::YELLOW) { graphicSetNameSuffix = "yellow"; }
        else if (playerColor == PlayerColor::WHITE) { graphicSetNameSuffix = "white"; }

        return "coat-of-arms/population/" + graphicSetNameSuffix;
    }

    /**
     * @brief Liefert den Renderer zurück.
     * @return Renderer
     */
    virtual IRenderer* const getRenderer() const = 0;

    /**
     * @brief Hilfs-Methode, um mittels eines `mapObjectType` die `mapWidth` und `mapHeight` zu ermitteln.
     * Diese Methode übernimmt die Fallunterscheidung `Structure` vs. `Harvestable`. Für `Harvestable`-Objekte
     * brauchen wir Growth-Zustand, um an die Grafik zu kommen.
     *
     * @param mapObjectType Typ des Objekts
     * @param view gewünschte Ansicht, für die `mapWidth` und `mapHeight` ermitteln werden soll
     * @param mapWidth (OUT) erhält die Breite des Objekts in Kacheleinheiten
     * @param mapHeight (OUT) erhält die Höhe des Objekts in Kacheleinheiten
     */
    void getMapWidthHeightByMapObjectType(const MapObjectType& mapObjectType, const FourthDirection& view,
                                          unsigned char* mapWidth, unsigned char* mapHeight) {

        const std::string graphicSetName = getGraphicSetNameForMapObject(mapObjectType);
        const GraphicSet* graphicSet = getGraphicSet(graphicSetName);

        const IGraphic* graphic;
        if (mapObjectType >= MapObjectType::START_STRUCTURES) {
            graphic = graphicSet->getByView(view)->getGraphic();
        } else {
            // Harvestable? Growth-Zustand 0 nehmen, die sind eh alle gleich
            graphic = graphicSet->getByStateAndView("growth0", view)->getGraphic();
        }

        *mapWidth = graphic->getMapWidth();
        *mapHeight = graphic->getMapHeight();
    }
};

#endif