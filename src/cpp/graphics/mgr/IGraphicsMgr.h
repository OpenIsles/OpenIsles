#ifndef _I_GRAPHICS_MGR_H
#define _I_GRAPHICS_MGR_H

#include <cassert>
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
    std::string getGraphicSetNameForMapObject(const MapObjectType* mapObjectType) const {
        // TODO Das muss irgendwie hübscher werden. In Zukunft muss alles mit den String-Keys laufen
        std::string graphicSetNamePrefix;
        std::string graphicSetNameSuffix;

        if (mapObjectType->type == HARVESTABLE) {
            graphicSetNamePrefix = "harvestable";
        } else if (mapObjectType->type == STRUCTURE) {
            graphicSetNamePrefix = "structure";
        } else if (mapObjectType->type == BUILDING) {
            graphicSetNamePrefix = "building";
        } else {
            assert(false);
        }

        graphicSetNameSuffix = mapObjectType->name;

        return graphicSetNamePrefix + "/" + graphicSetNameSuffix;
    }

    std::string getGraphicSetNameForGoodIcons(const Good* good, bool marketplaceIcon) const {
        const std::string graphicSetNamePrefix = (marketplaceIcon) ? "goods-marketplace-icon" : "goods-icon";

        return graphicSetNamePrefix + "/" + good->name;
    }

    std::string getGraphicSetNameForCoatOfArms(const std::string& prefix, PlayerColor playerColor) const {
        std::string graphicSetNameSuffix;

        // TODO Das muss irgendwie hübscher werden. In Zukunft muss alles mit den String-Keys laufen
        if (playerColor == PlayerColor::RED) { graphicSetNameSuffix = "red"; }
        else if (playerColor == PlayerColor::BLUE) { graphicSetNameSuffix = "blue"; }
        else if (playerColor == PlayerColor::YELLOW) { graphicSetNameSuffix = "yellow"; }
        else if (playerColor == PlayerColor::WHITE) { graphicSetNameSuffix = "white"; }

        return "coat-of-arms/" + prefix + "/" + graphicSetNameSuffix;
    }

    /**
     * @brief Liefert den Renderer zurück.
     * @return Renderer
     */
    virtual IRenderer* const getRenderer() const = 0;

};

#endif