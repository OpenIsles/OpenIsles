#ifndef _CARRIER_H
#define _CARRIER_H

#include "economics/UpdateableObject.h"
#include "game/GoodsSlot.h"
#include "map/MapObject.h"
#include "pathfinding/AStar.h"

class Building;

/**
 * @brief Träger, das sind Map-Objekte, die Waren von A nach B transportieren
 */
class Carrier : public MapObject, public UpdateableObject {

    friend class EconomicsMgr; // EconomicsMgr soll zum Aktualisieren des Trägers einfach zugreifen können
    friend class Map; // Map soll zum Rendern des Trägers einfach zugreifen können

private:
    /**
     * @brief Referenz auf das Gebäude, zu dem dieser Träger gehört.
     */
    Building* owningBuilding;

    /**
     * @brief Route, die der Träger abzulaufen hat.
     * Die Route wird zerstört, wenn das Träger-Objekt zerstört wird.
     */
    Route* route;

    /**
     * @brief Waren, die der Träger hält bzw. abholen soll
     */
    GoodsSlot carriedGoods;

    /**
     * @brief X-Pixel-Offset der Animation innerhalb der Kachel
     */
    int xOffsetInTile;

    /**
     * @brief Y-Pixel-Offset der Animation innerhalb der Kachel
     */
    int yOffsetInTile;

    // TODO Animation sollte spezifiert sein, aktuell is fix die eine Animation in Verwendung

    /**
     * @brief aktueller Frame der Animation, die grade abgespielt wird. Dieser Wert ist ein double, da fortlaufend
     * aktualisiert wird. Für das Rendering muss diese Zahl zur Ganzzahl umgewandelt werden.
     * TODO ggf. macht es mehr Sinn, noch ein Bindeglied (erweitert UpdateableObject) zwischen Animation und der Anwenderklasse zu setzen, die diese Info enthält
     */
    double animationFrame;

public:
    /**
     * @brief Konstruiert einen neuen Träger mit einem konkreten Abholauftrag.
     * @param owningBuilding Gebäude, zu dem dieser Träger gehört.
     * @param route Route, die der Träger ablaufen soll
     * @param goodsType Warentyp, der abzuholen ist
     */
    Carrier(Building* owningBuilding, Route* route, GoodsType goodsType);

    /**
     * @brief Destruktor. Räumt die Route mit weg.
     */
    ~Carrier();
};

#endif
