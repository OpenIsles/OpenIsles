#ifndef _I_GRAPHICS_MGR_H
#define _I_GRAPHICS_MGR_H

#include <string>
#include "defines.h"
#include "game/GoodsSlot.h"
#include "game/Player.h"
#include "graphics/graphic/IGraphic.h"
#include "graphics/graphic/GraphicSet.h"
#include "graphics/renderer/IRenderer.h"
#include "map/StructureType.h"

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
    std::string getGraphicSetNameForStructure(StructureType structureType) const {
        // TODO Das muss irgendwie hübscher werden. In Zukunft muss alles mit den String-Keys laufen
        std::string graphicSetNameSuffix;

        if (structureType == StructureType::CHAPEL) { graphicSetNameSuffix = "chapel"; }
        else if (structureType == StructureType::PIONEERS_HOUSE1) { graphicSetNameSuffix = "pioneers-house1"; }
        else if (structureType == StructureType::PIONEERS_HOUSE2) { graphicSetNameSuffix = "pioneers-house2"; }
        else if (structureType == StructureType::PIONEERS_HOUSE3) { graphicSetNameSuffix = "pioneers-house3"; }
        else if (structureType == StructureType::PIONEERS_HOUSE4) { graphicSetNameSuffix = "pioneers-house4"; }
        else if (structureType == StructureType::PIONEERS_HOUSE5) { graphicSetNameSuffix = "pioneers-house5"; }
        else if (structureType == StructureType::STONEMASON) { graphicSetNameSuffix = "stonemason"; }
        else if (structureType == StructureType::OFFICE1) { graphicSetNameSuffix = "office1"; }
        else if (structureType == StructureType::MARKETPLACE) { graphicSetNameSuffix = "marketplace"; }
        else if (structureType == StructureType::FORESTERS) { graphicSetNameSuffix = "foresters"; }
        else if (structureType == StructureType::SHEEP_FARM) { graphicSetNameSuffix = "sheep-farm"; }
        else if (structureType == StructureType::WEAVING_MILL1) { graphicSetNameSuffix = "weaving-mill1"; }
        else if (structureType == StructureType::CATTLE_FARM) { graphicSetNameSuffix = "cattle-farm"; }
        else if (structureType == StructureType::BUTCHERS) { graphicSetNameSuffix = "butchers"; }
        else if (structureType == StructureType::TOOLSMITHS) { graphicSetNameSuffix = "toolsmiths"; }
        else if (structureType == StructureType::HUNTERS_HUT) { graphicSetNameSuffix = "hunters-hut"; }

        else if (structureType == StructureType::STREET) { graphicSetNameSuffix = "street-straight0"; } // wird für "allgemeine" Straße benutzt, z.B. wenn im Baumenü ausgewählt
        else if (structureType == StructureType::STREET_STRAIGHT_0) { graphicSetNameSuffix = "street-straight0"; }
        else if (structureType == StructureType::STREET_STRAIGHT_90) { graphicSetNameSuffix = "street-straight90"; }
        else if (structureType == StructureType::STREET_CURVE_0) { graphicSetNameSuffix = "street-curve0"; }
        else if (structureType == StructureType::STREET_CURVE_90) { graphicSetNameSuffix = "street-curve90"; }
        else if (structureType == StructureType::STREET_CURVE_180) { graphicSetNameSuffix = "street-curve180"; }
        else if (structureType == StructureType::STREET_CURVE_270) { graphicSetNameSuffix = "street-curve270"; }
        else if (structureType == StructureType::STREET_TEE_0) { graphicSetNameSuffix = "street-tee0"; }
        else if (structureType == StructureType::STREET_TEE_90) { graphicSetNameSuffix = "street-tee90"; }
        else if (structureType == StructureType::STREET_TEE_180) { graphicSetNameSuffix = "street-tee180"; }
        else if (structureType == StructureType::STREET_TEE_270) { graphicSetNameSuffix = "street-tee270"; }
        else if (structureType == StructureType::STREET_CROSS) { graphicSetNameSuffix = "street-cross"; }

        return "structures/" + graphicSetNameSuffix;
    }

    std::string getGraphicSetNameForGoodIcons(GoodsType goodsType, bool marketplaceIcon) const {
        std::string graphicSetNamePrefix = (marketplaceIcon) ? "goods-marketplace-icons" : "goods-icons";

        // TODO Das muss irgendwie hübscher werden. In Zukunft muss alles mit den String-Keys laufen
        std::string graphicSetNameSuffix;

        if (goodsType == GoodsType::WOOL) { graphicSetNameSuffix = "wool"; }
        else if (goodsType == GoodsType::CATTLE) { graphicSetNameSuffix = "cattle"; }
        else if (goodsType == GoodsType::FOOD) { graphicSetNameSuffix = "food"; }
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

};

#endif
