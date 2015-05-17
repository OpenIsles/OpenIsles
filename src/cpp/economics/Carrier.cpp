#include <cmath>
#include "economics/Carrier.h"
#include "game/Colony.h"
#include "game/Game.h"


Carrier::Carrier(Building* owningBuilding, Route route, const Good* good, bool onOutboundTrip) :
    owningBuilding(owningBuilding), route(route), onOutboundTrip(onOutboundTrip) {

    // TODO Bewegliche Map-Objekte dürfen aktuell nur 1 Kachel groß sein. Später, wenn Schiffe (speziell Fischerboot-Carrier) da sind, müssen wir das erweitern.
    // Träger fix als 1x1-groß markieren
    setMapWidth(1);
    setMapHeight(1);

    carriedGoods.good = good;

    // Animation initialisieren
    animationFrame = 0;

    // Route initialisieren
    nextHopInRoute = this->route.cbegin();
    nextHopInRoute++;
}

void Carrier::updateCurrentMovingDirection() {
    const MapCoords& nextHopOnRoute = *nextHopInRoute;
    const double diffX = nextHopOnRoute.x() - mapCoords.x();
    const double diffY = nextHopOnRoute.y() - mapCoords.y();

    if (diffX > 0 && diffY == 0) {
        currentMovingDirection = Direction::EAST;
    } else if (diffX > 0 && diffY > 0) {
        currentMovingDirection = Direction::SOUTHEAST;
    } else if (diffX == 0 && diffY > 0) {
        currentMovingDirection = Direction::SOUTH;
    } else if (diffX < 0 && diffY > 0) {
        currentMovingDirection = Direction::SOUTHWEST;
    } else if (diffX < 0 && diffY == 0) {
        currentMovingDirection = Direction::WEST;
    } else if (diffX < 0 && diffY < 0) {
        currentMovingDirection = Direction::NORTHWEST;
    } else if (diffX == 0 && diffY < 0) {
        currentMovingDirection = Direction::NORTH;
    } else if (diffX > 0 && diffY < 0) {
        currentMovingDirection = Direction::NORTHEAST;
    }
}

bool Carrier::updateObject(const Context& context) {
    unsigned int ticksPastSinceLastUpdate = context.sdlTicks - lastUpdateTime;
    double oneSecondTicks = (double) 1000 / context.game->getSpeed();

    Map* map = context.game->getMap();
    bool deleteMe = false; // Flag, um den Träger zu löschen

    // Animieren
    const Animation* animation = getAnimations()[0]; // TODO wir brauchen nur fps und framesCount. Das sollte in allen Richtungen gleich sein.

    animationFrame += (double) ticksPastSinceLastUpdate / oneSecondTicks * animation->getFps();
    while (animationFrame >= animation->getFramesCount()) {
        animationFrame -= animation->getFramesCount();
    }

    // Bewegen
    const double speed = 0.75; // Kacheln pro Sekunde (auf 1x Spielgeschwindigkeit), erstmal fix, die Animation muss man eh nochmal anpassen, weil das Männchen mehr schwebt, als läuft

    unsigned int ticksToProcess = ticksPastSinceLastUpdate;
    do {
        /* Wir gehen kachelweise vor. Erst ausrechnen, wie viele Ticks notwendig sind, um den nächsten Hop fertig zu
         * machen. Dann gehen wir soweit wir kommen. Bestcase wir schaffen den Hop, dann loopen wir zum nächsten.
         * Ansonsten rücken wir eben nur ein kleines Stückchen weiter.
         */

        const MapCoords& nextHopOnRoute = *nextHopInRoute;
        DoubleMapCoords& mapCoords = getMapCoords();

        double mapDeltaXToNextHop = (nextHopOnRoute.x() - mapCoords.x());
        double mapDeltaYToNextHop = (nextHopOnRoute.y() - mapCoords.y());

        //  Halbe Ticks gibts nicht! Wir müssen hier runden.
        unsigned int ticksToNextHop = (unsigned int)
            (std::max(std::abs(mapDeltaXToNextHop), std::abs(mapDeltaYToNextHop)) * oneSecondTicks / speed);

        // Hop geschafft?
        if (ticksToNextHop <= ticksToProcess) {
            map->moveMapObject(this, nextHopOnRoute);
            nextHopInRoute++;
            updateCurrentMovingDirection();

            // Route fertig?
            if (nextHopInRoute == route.cend()) {
                MapTile* mapTile = map->getMapTileAt(route.back());
                Building* targetBuilding = dynamic_cast<Building*>(mapTile->mapObjectFixed);

                // targetBuilding == nullptr -> Nutzer hat die Map inzwischen geändert und das Zielgebäude abgerissen
                // Der Träger verschwindet einfach
                if (targetBuilding == nullptr) {
                    deleteMe = true;
                    owningBuilding->carrier = nullptr;
                }

                // Das war Hinweg -> Waren aufladen und Träger auf Rückweg schicken
                else if (onOutboundTrip) {
                    // Waren aufladen
                    GoodsSlot* goodsSlotToTakeFrom;
                    if (targetBuilding->isStorageBuilding()) {
                        // Lagergebäude: Waren aus der Siedlung aufladen
                        Colony* colony = context.game->getColony(targetBuilding);
                        goodsSlotToTakeFrom = &colony->getGoods(carriedGoods.good);
                    } else {
                        // Waren aus dem Gebäude aufladen
                        goodsSlotToTakeFrom = &targetBuilding->productionSlots.output;
                    }

                    int goodsWeCollect = (int) goodsSlotToTakeFrom->inventory;

                    // Wir dürfen nicht mehr aufladen, als im Ziellager abgeladen werden kann, sonst gehen die Waren
                    // verloren. Wir müssen hier in Ganzzahlen denken: Haben wir 2,999t Platz, dürfen wir nicht 3t
                    // aufladen, sondern nur 2t.
                    GoodsSlot* goodsSlotWeWillUnloadToLater =
                        findGoodsSlotToUnloadTo(context, owningBuilding, carriedGoods.good);
                    int remainingCapacityToUnloadLater = (int) (
                        (double) goodsSlotWeWillUnloadToLater->capacity - goodsSlotWeWillUnloadToLater->inventory);
                    if (goodsWeCollect > remainingCapacityToUnloadLater) {
                        goodsWeCollect = remainingCapacityToUnloadLater;
                    }

                    // Ein Träger kann maximal 6t Waren halten
                    if (goodsWeCollect > 6) {
                        goodsWeCollect = 6;
                    }

                    goodsSlotToTakeFrom->inventory -= goodsWeCollect;
                    targetBuilding->lastGoodsCollections = context.sdlTicks;

                    AStar aStar(&context, owningBuilding, true, owningBuilding->isStorageBuilding(), false);
                    Route returnRoute = aStar.getRoute(route.back(), route.front());

                    deleteMe = true;
                    owningBuilding->carrier = nullptr;

                    // Es kann sein, dass es keine Rückroute gibt, wenn der Nutzer inzwischen die Map verändert hat.
                    // In diesem Fall hat er Pech gehabt, die Waren verschwinden mit dem Träger
                    if (returnRoute.routeExists()) {
                        const MapCoords& firstHopOnReturnRoute = returnRoute.front();

                        const GraphicSet& graphicSet = *context.graphicsMgr->getGraphicSet(
                            owningBuilding->isStorageBuilding() ? "cart-with-cargo" : "carrier");

                        Carrier* returnCarrier = new Carrier(
                            owningBuilding, returnRoute, goodsSlotToTakeFrom->good, false);
                        returnCarrier->setMapCoords((DoubleMapCoords) firstHopOnReturnRoute);
                        returnCarrier->updateCurrentMovingDirection();
                        returnCarrier->setAnimations(graphicSet);
                        returnCarrier->carriedGoods.inventory = goodsWeCollect;

                        map->addMapObject(returnCarrier);
                        owningBuilding->carrier = returnCarrier;

                        // Slot wieder freimachen
                        if (goodsSlotToTakeFrom->markedForPickup) {
                            goodsSlotToTakeFrom->markedForPickup = false;
                        }
                    }
                }

                // Das war Rückweg -> Waren ausladen und Träger zerstören
                else if (!onOutboundTrip) {
                    GoodsSlot* goodsSlotToUnloadTo =
                        findGoodsSlotToUnloadTo(context, owningBuilding, carriedGoods.good);
                    goodsSlotToUnloadTo->increaseInventory(carriedGoods.inventory);

                    deleteMe = true;
                    owningBuilding->carrier = nullptr;
                }

                break; // Loop verlassen. Rückweg erst im nächsten Update, wir sind hier fertig.
            }

            ticksToProcess -= ticksToNextHop;
        }
        // Hop nicht ganz geschafft,
        else {
            // Anteil der Reststrecke in der Kachel ausrechnen, die wir schaffen
            double fractionOfRestToNextHop = (double) ticksToProcess / (double) ticksToNextHop;
            DoubleMapCoords newMapCoords = DoubleMapCoords(
                mapCoords.x() + mapDeltaXToNextHop * fractionOfRestToNextHop,
                mapCoords.y() + mapDeltaYToNextHop * fractionOfRestToNextHop
            );
            map->moveMapObject(this, newMapCoords);

            break; // ticksToProcess auf 0 setzen -> können wir gleich die Schleife brechen :-)
        }

    } while(ticksToProcess > 0);

    return (!deleteMe);
}

GoodsSlot* Carrier::findGoodsSlotToUnloadTo(const Context& context, Building* building, const Good* good) {
    bool isStorageBuilding = building->isStorageBuilding();

    // Lagergebäude: Waren der Siedlung gutschreiben
    if (isStorageBuilding) {
        Colony* colony = context.game->getColony(building);
        return &colony->getGoods(good);
    }
        // Produktionsgebäude: Gucken, in welchen Slot die Waren müssen
    else {
        if (building->productionSlots.input.good == good) {
            return &building->productionSlots.input;
        }
        else if (building->productionSlots.input2.good == good) {
            return &building->productionSlots.input2;
        }

        assert(false); // Der Träger passt nicht zu den Gebäude-Slots!? Darf nicht passieren.
        return nullptr;
    }
}