#ifndef _CARRIER_H
#define _CARRIER_H

#include <gtest/gtest.h>
#include "economics/UpdateableObject.h"
#include "game/GoodsSlot.h"
#include "graphics/graphic/Animation.h"
#include "graphics/graphic/GraphicSet.h"
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
     * @brief verwendete Animationen für den Träger
     */
    EightDirectionsAnimation animations;

    /**
     * @brief aktuelle Richtung, in die der Träger auf der Karte läuft. Wir wollen nicht bei jedem Frame das
     * ausrechnen müssen, drum ist hier immer der aktuelle Wert.
     */
    EightDirectionsView currentMovingDirection;

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

    const EightDirectionsAnimation& getAnimations() const {
        return animations;
    }

    void setAnimations(const GraphicSet& graphicSet) {
        for (const EightDirectionsView& view : EightDirectionsView::ALL_VIEWS) {
            animations[view.getViewIndex()] = graphicSet.getByView(view);
        }
    }

    const EightDirectionsView& getCurrentMovingDirection() const {
        return currentMovingDirection;
    }

    /**
     * @brief Aktualisiert currentMovingDirection. Diese Methode muss immer dann aufgerufen werden, wenn der Träger
     * auf seiner Route die Richtung ändert.
     */
    void updateCurrentMovingDirection();
};

#endif
