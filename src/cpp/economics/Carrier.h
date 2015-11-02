#ifndef _CARRIER_H
#define _CARRIER_H

#ifdef TESTS_ENABLED
#include <gtest/gtest.h>
#endif

#include "Context.h"
#include "game/GoodsSlot.h"
#include "graphics/graphic/Animation.h"
#include "graphics/graphic/GraphicSet.h"
#include "map/MapObject.h"
#include "pathfinding/AStar.h"

class Building;

/**
 * @brief Träger, das sind Map-Objekte, die Waren von A nach B transportieren
 */
class Carrier : public MapObjectMoving {

#ifdef TESTS_ENABLED
    FRIEND_TEST(CarrierSheepTest, checkThatSheepsHarvestMultipleTimesBeforeReturning);
    FRIEND_TEST(CarrierTest, checkThatCarrierIsSentToFullerInventoryFirst);
    FRIEND_TEST(CarrierTest, checkThatCarrierIsSentByLastCollectionWhenSameInventory);
    FRIEND_TEST(CarrierWalkingTest, checkThatCarrierWalksCorrectlyOnTheStreet);
#endif
    friend class EconomicsMgr; // EconomicsMgr soll zum Aktualisieren des Trägers einfach zugreifen können
    friend class GuiMap; // GuiMap soll zum Rendern des Trägers einfach zugreifen können

private:
    /**
     * @brief Referenz auf das Gebäude, zu dem dieser Träger gehört.
     */
    Building* owningBuilding;

    /**
     * @brief Route, die der Träger abzulaufen hat.
     */
    Route route;

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
    EighthDirection currentMovingDirection;

    /**
     * @brief aktueller Frame der Animation, die grade abgespielt wird. Dieser Wert ist ein double, da fortlaufend
     * aktualisiert wird. Für das Rendering muss diese Zahl zur Ganzzahl umgewandelt werden.
     * TODO ggf. macht es mehr Sinn, noch ein Bindeglied (erweitert MapObject) zwischen Animation und der Anwenderklasse zu setzen, die diese Info enthält
     */
    double animationFrame;

    /**
     * @brief Zustand des Träger, um zwischen "laufen" und "ernten" zu unterscheiden
     */
    enum {
        WALKING,
        HARVESTING
    } state = WALKING;

    /**
     * @brief [Zeitpunkt](@ref gameTicks), wann wir mit Ernten fertig sind.
     * Nur gültig, wenn `state == HARVESTING`.
     */
    unsigned long harvestingFinishedTicks;

public:
    Carrier() {}
    ~Carrier() {}

    virtual bool updateObject(const Context& context) override;

    /**
     * @brief Initialisiert einen eben neu instanziierten Träger und weist ihm einen konkreten Abhol-/Lieferauftrag zu.
     * Der Träger wird an die Wegmarke der Route gesetzt, die Animation gesetzt,
     * die Laufrichtung des Träger ausgerichtet, sowie der Status auf WALKING.
     *
     * @param owningBuilding Gebäude, zu dem dieser Träger gehört.
     * @param route Route, die der Träger ablaufen soll
     * @param good Gut, das abzuholen bzw. zu liefern ist
     * @param capacity Menge, die der Träger halten kann
     * @param onOutboundTrip `true` für Hinweg, `false` für Rückweg.
     * @param animations zu verwendende Animationen für den Träger
     * @param gameTicks aktuelle [Spielzeit](@ref gameTicks). Wird verwendet, um die lastUpdateTicks-Zeit zu setzen
     */
    void initRoute(Building* owningBuilding, Route route, const Good* good, unsigned int capacity,
                   bool onOutboundTrip, const EightDirectionsAnimation& animations, unsigned long gameTicks);

    const EightDirectionsAnimation& getAnimations() const {
        return animations;
    }

    void setAnimations(const GraphicSet& graphicSet) {
        forEachEighthDirection(view) {
            animations[view] = graphicSet.getByView(view);
        }
    }

    void setAnimations(const EightDirectionsAnimation& animations) {
        forEachEighthDirection(view) {
            this->animations[view] = animations[view];
        }
    }

    const EighthDirection& getCurrentMovingDirection() const {
        return currentMovingDirection;
    }

    /**
     * @brief Aktualisiert currentMovingDirection. Diese Methode muss immer dann aufgerufen werden, wenn der Träger
     * auf seiner Route die Richtung ändert.
     */
    void updateCurrentMovingDirection();

private:
    /**
     * @brief Handelt die Situation, wenn der Route die Träger fertig gelaufen ist.
     * Entweder wird der Träger zerstört, beginnt mit Ernten oder ihm wird eine neue Route generiert und zugewiesen.
     *
     * @param context (Dependency)
     * @param deleteMe Flag, mit dem man die Zerstörung des Trägers anweisen kann
     */
    void onRouteDone(const Context& context, bool& deleteMe);

    /**
     * @brief Handelt die Situation, wenn der Träger mit Ernten fertig ist.
     * Ihm wird eine neue Route zugewiesen.
     *
     * @param context (Dependency)
     */
    void onHarvestingFinished(const Context& context);

    /**
     * @brief Sucht für ein Gebäude einen GoodsSlot, in welchen ein Träger seine Waren laden soll.
     * Handelt es sich bei dem Gebäude um ein Lagergebäude, wird der passende Slot in der Kolonie zurückgegeben,
     * ansonsten wird der passende Slot `input` oder `input2` vom Gebäude zurückgegeben.
     *
     * @param building Gebäude, das das Ziel des Trägers ist
     * @param good Gut, das wir ausladen wollen
     * @return GoodsSlot, in welchen die Waren ausgeladen werden sollten
     */
    GoodsSlot* findGoodsSlotToUnloadTo(Building* building, const Good* good);
};

#endif
