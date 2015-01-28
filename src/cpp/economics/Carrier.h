#ifndef _CARRIER_H
#define _CARRIER_H

#include <gtest/gtest.h>
#include "economics/UpdateableObject.h"
#include "game/GoodsSlot.h"
#include "graphics/graphic/Animation.h"
#include "map/MapObject.h"
#include "pathfinding/AStar.h"

class Building;

/**
 * @brief Träger, das sind Map-Objekte, die Waren von A nach B transportieren
 */
class Carrier : public MapObjectMoving, public UpdateableObject {

    FRIEND_TEST(EconomicsMgrTest, updateCarrier);
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
     * @brief Iterator in @ref route, der auf die nächste Kachel zeigt, zu der der Träger laufen muss.
     */
    Route::const_iterator nextHopInRoute;

    /**
     * @brief verwendete Animation für den Träger
     */
    const Animation* animation;

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

    /**
     * @brief Destruktor. Räumt die Route mit weg.
     */
    ~Carrier();

    const Animation* getAnimation() const {
        return animation;
    }

    void setAnimation(const Animation* animation) {
        this->animation = animation;
    }
};

#endif
