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
    friend class GuiMap; // GuiMap soll zum Rendern des Trägers einfach zugreifen können

private:
    /**
     * @brief Referenz auf das Gebäude, zu dem dieser Träger gehört.
     */
    Building* owningBuilding;

    /**
     * @brief Route, die der Träger abzulaufen hat.
     * Die Route wird spätestens zerstört, wenn das Träger-Objekt zerstört wird.
     */
    Route* route;

    /**
     * @brief Waren, die der Träger hält bzw. abholen soll
     */
    GoodsSlot carriedGoods;

    /**
     * @brief Flag, das angibt, ob wir auf dem Hin- oder dem Rückweg sind.
     * - `true` = Hinweg, d.h. Waren am Ziel aufladen und danach auf den Rückweg machen
     * - `false` = Rückweg, d.h. Waren am Ziel ausladen und den Träger zerstören
     */
    bool onOutboundTrip;

    /**
     * @brief X-Map-Koordinate-Offset, der auf den normalen mapX-Wert addiert wird
     * Der Offset bewegt sich im Bereich 0.0 bis <1.0.
     */
    double mapXFraction;

    /**
    * @brief Y-Map-Koordinate-Offset, der auf den normalen mapY-Wert addiert wird
    * Der Offset bewegt sich im Bereich 0.0 bis <1.0.
    */
    double mapYFraction;

    /**
     * @brief Iterator in @ref route, der auf die nächste Kachel zeigt, zu der der Träger laufen muss.
     */
    Route::const_iterator nextHopInRoute;

    /**
     * @brief verwendete Animation für den Träger
     */
    IAnimation* animation;

    /**
     * @brief aktueller Frame der Animation, die grade abgespielt wird. Dieser Wert ist ein double, da fortlaufend
     * aktualisiert wird. Für das Rendering muss diese Zahl zur Ganzzahl umgewandelt werden.
     * TODO ggf. macht es mehr Sinn, noch ein Bindeglied (erweitert UpdateableObject) zwischen Animation und der Anwenderklasse zu setzen, die diese Info enthält
     */
    double animationFrame;

public:
    /**
     * @brief Konstruiert einen neuen Träger mit einem konkreten Abhol-/Lieferauftrag.
     * @param owningBuilding Gebäude, zu dem dieser Träger gehört.
     * @param route Route, die der Träger ablaufen soll
     * @param goodsType Warentyp, der abzuholen bzw. zu liefern ist
     * @param onOutboundTrip `true` für Hinweg, `false` für Rückweg.
     */
    Carrier(Building* owningBuilding, Route* route, GoodsType goodsType, bool onOutboundTrip);


    IAnimation* getAnimation() const {
        return animation;
    }

    void setAnimation(IAnimation* animation) {
        this->animation = animation;
    }

    /**
     * @brief Destruktor. Räumt die Route mit weg.
     */
    ~Carrier();
};

#endif
