#include <cmath>
#include "economics/Carrier.h"
#include "game/Colony.h"
#include "game/Game.h"
#include "graphics/graphic/GraphicSetKeyState.h"
#include "map/MapObjectUtils.h"


void Carrier::initRoute(
    Building* owningBuilding, Route route, const Good* good, unsigned int capacity,
    bool onOutboundTrip, const EightDirectionsAnimation& animations, unsigned long gameTicks) {

    this->owningBuilding = owningBuilding;
    this->route = route;
    this->onOutboundTrip = onOutboundTrip;

    // TODO Bewegliche Map-Objekte dürfen aktuell nur 1 Kachel groß sein. Später, wenn Schiffe (speziell Fischerboot-Carrier) da sind, müssen wir das erweitern.
    // Träger fix als 1x1-groß markieren
    setMapWidth(1);
    setMapHeight(1);

    carriedGoods.good = good;
    carriedGoods.capacity = capacity;

    // Route initialisieren
    nextHopInRoute = this->route.cbegin();
    nextHopInRoute++;

    const MapCoords& firstHopOnRoute = route.front();
    setMapCoords((DoubleMapCoords) firstHopOnRoute);

    state = WALKING;

    // Animation initialisieren
    setAnimations(animations);
    animationFrame = 0;
    updateCurrentMovingDirection();

    setLastUpdateTicks(gameTicks);
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

// TODO Refactoren und carrierAnimation nutzen
bool Carrier::updateObject(const Context& context) {
    unsigned int ticksSinceLastUpdate = getTicksSinceLastUpdate(context);

    // Animieren
    const Animation* animation = getAnimations()[0]; // fps und framesCount sollte in allen Richtungen gleich sein!
    MapObjectUtils::animateObject(*this, animation, context);

    // TODO verallgemeinern
    if (state != WALKING) {
        // Fertig mit ernten?
        if (context.game->getTicks() >= harvestingFinishedTicks) {
            onHarvestingFinished(context);
        }
        return true;
    }

    // Bewegen
    const double speed = 0.75; // Kacheln pro Sekunde Spielzeit, erstmal fix, die Animation muss man eh nochmal anpassen, weil das Männchen mehr schwebt, als läuft

    Map* map = context.game->getMap();
    bool deleteMe = false; // Flag, um den Träger zu löschen

    unsigned int ticksToProcess = ticksSinceLastUpdate;
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
            (std::max(std::abs(mapDeltaXToNextHop), std::abs(mapDeltaYToNextHop)) * (double) TICKS_PER_SECOND / speed);

        // Hop geschafft?
        if (ticksToNextHop <= ticksToProcess) {
            map->moveMapObject(this, nextHopOnRoute);
            nextHopInRoute++;

            // Route fertig?
            if (nextHopInRoute == route.cend()) {
                onRouteDone(context, deleteMe);
                break; // Loop verlassen. Rückweg erst im nächsten Update, wir sind hier fertig.
            }

            updateCurrentMovingDirection();
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

void Carrier::onRouteDone(const Context& context, bool& deleteMe) {
    // TODO verallgemeinern: Träger zu Erntefeld vs. Träger zu Gebäude
    if (mapObjectType->name == "sheep" || mapObjectType->name == "cattle") {
        // Das war Hinweg zum Erntefeld -> Animation umstellen und mit Ernten anfangen
        if (onOutboundTrip) {
            const GraphicSetKeyState& graphicSetKeyState =
                (carriedGoods.inventory > 0) ?GraphicSetKeyState::EATING_WITH_GOODS : GraphicSetKeyState::EATING;

            animations = mapObjectType->getAnimationsForState(graphicSetKeyState);
            animationFrame = 0;
            state = HARVESTING;
            harvestingFinishedTicks =
                context.game->getTicks() + (unsigned long) (mapObjectType->secondsToProduce * TICKS_PER_SECOND);
            return;
        }
    }

    Map* map = context.game->getMap();
    MapTile* mapTile = map->getMapTileAt(route.back());
    Building* targetBuilding = dynamic_cast<Building*>(mapTile->mapObjectFixed);

    // targetBuilding == nullptr -> Nutzer hat die Map inzwischen geändert und das Zielgebäude abgerissen
    // Der Träger verschwindet einfach
    if (targetBuilding == nullptr) {
        deleteMe = true;
        owningBuilding->carriers.erase(this);
    }

    // Das war Hinweg -> Waren aufladen und Träger auf Rückweg schicken
    else if (onOutboundTrip) {
        // Waren aufladen
        GoodsSlot* goodsSlotToTakeFrom;
        if (targetBuilding->isStorageBuilding()) {
            // Lagergebäude: Waren aus der Siedlung aufladen
            Colony* colony = targetBuilding->getColony();
            goodsSlotToTakeFrom = &colony->getGoods(carriedGoods.good);
        } else {
            // Waren aus dem Gebäude aufladen
            goodsSlotToTakeFrom = &targetBuilding->productionSlots.output;
        }

        unsigned int goodsWeCollect = goodsSlotToTakeFrom->inventory;

        // Wir dürfen nicht mehr aufladen, als im Ziellager abgeladen werden kann, sonst gehen die Waren
        // verloren.
        GoodsSlot* goodsSlotWeWillUnloadToLater = findGoodsSlotToUnloadTo(owningBuilding, carriedGoods.good);
        unsigned int remainingCapacityToUnloadLater =
            goodsSlotWeWillUnloadToLater->capacity - goodsSlotWeWillUnloadToLater->inventory;
        if (goodsWeCollect > remainingCapacityToUnloadLater) {
            goodsWeCollect = remainingCapacityToUnloadLater;
        }

        // Mehr Waren als der Träger tragen kann?
        if (goodsWeCollect > mapObjectType->carrier.capacity) {
            goodsWeCollect = mapObjectType->carrier.capacity;
        }

        goodsSlotToTakeFrom->inventory -= goodsWeCollect;
        targetBuilding->lastGoodsCollections = context.game->getTicks();

        CatchmentAreaIterator catchmentAreaIterator(*owningBuilding, false);
        AStar aStar(context, &catchmentAreaIterator, true, owningBuilding->isStorageBuilding(), false);
        Route returnRoute = aStar.getRoute(route.back(), route.front());

        deleteMe = true;
        owningBuilding->carriers.erase(this);

        // Es kann sein, dass es keine Rückroute gibt, wenn der Nutzer inzwischen die Map verändert hat.
        // In diesem Fall hat er Pech gehabt, die Waren verschwinden mit dem Träger
        if (returnRoute.routeExists()) {
            Carrier* returnCarrier = (Carrier*) MapObject::instantiate(mapObjectType);
            returnCarrier->initRoute(
                owningBuilding, returnRoute, goodsSlotToTakeFrom->good, mapObjectType->carrier.capacity,
                false, mapObjectType->getAnimationsForState(WALKING_WITH_GOODS), context.game->getTicks());
            returnCarrier->carriedGoods.inventory = goodsWeCollect;

            map->addMapObject(returnCarrier);
            owningBuilding->carriers.insert(returnCarrier);

            // Slot wieder freimachen
            if (goodsSlotToTakeFrom->markedForPickup) {
                goodsSlotToTakeFrom->markedForPickup = false;
            }
        }
    }

    // Das war Rückweg -> Waren ausladen und Träger zerstören
    else if (!onOutboundTrip) {
        GoodsSlot* goodsSlotToUnloadTo = findGoodsSlotToUnloadTo(owningBuilding, carriedGoods.good);
        goodsSlotToUnloadTo->increaseInventory(carriedGoods.inventory);

        deleteMe = true;
        owningBuilding->carriers.erase(this);
    }
}

void Carrier::onHarvestingFinished(const Context& context) {
    // Spezialfall: Gebäude inzwischen abgerissen -> Träger mitsamt Waren verschwinden lassen
    // Nicht wie bei Anno 1602 an die ehemalige Stellen laufen lassen.
    if (owningBuilding == nullptr) {
        assert(false); // TODO
    }

    CatchmentAreaIterator catchmentAreaIterator(*owningBuilding, false);

    // Kachel als abgeerntet markieren und wieder freigeben
    MapCoords currentMapCoords = *route.crbegin();
    MapTile* mapTileHarvested = context.game->getMap()->getMapTileAt(currentMapCoords);
    mapTileHarvested->nextHarvestTicks = context.game->getTicks() + MapTile::HARVEST_REFRESH_TICKS;
    mapTileHarvested->harvestBusy = false;

    // Waren aufnehmen
    carriedGoods.increaseInventory(1);

    // Träger nicht voll? Dann nächste Kachel suchen, die wir ernten können
    if (!carriedGoods.isInventoryFull()) {
        // TODO Träger über Config steuern
        // TODO Rinderfarm sollte später Rinder haben, keine Schafe ;-)
        if (owningBuilding->getMapObjectType()->name == "sheep-farm" ||
            owningBuilding->getMapObjectType()->name == "cattle-farm") {

            const Good* goodGrass = context.configMgr->getGood("grass");

            InCatchmentAreaFinder inCatchmentAreaFinder(context, &catchmentAreaIterator);
            InCatchmentAreaFinderResult result;

            result = inCatchmentAreaFinder.findMapTileWithInvisibleGood(goodGrass, currentMapCoords);
            if (result) {
                EightDirectionsAnimation animations =
                    mapObjectType->getAnimationsForState(GraphicSetKeyState::WALKING_WITH_GOODS);

                initRoute(owningBuilding, result.route, goodGrass, carriedGoods.capacity,
                          true, animations, context.game->getTicks());

                // Slot markieren, dass nicht ein zweiter Träger hinläuft.
                context.game->getMap()->getMapTileAt(result.mapCoords)->harvestBusy = true;
                return;
            }

            // kein Gras mehr zum Ernten da? fall-through
        }

        // TODO weitere Fälle kommen später mal...
        else {
           assert(false);
        }
    }

    // Träger voll bzw. nix mehr zum ernten gefunden? Dann auf den Heimweg zum Gebäude zurück machen
    AStar aStar(context, &catchmentAreaIterator, true, false, false);
    Route routeBackToOwningBuilding = aStar.getRoute(
        MapCoords((int) getMapCoords().x(), (int) getMapCoords().y()), owningBuilding->getMapCoords());

    if (!routeBackToOwningBuilding.routeExists()) {
        // Spezialfall: Gebäude inzwischen abgerissen -> Träger mitsamt Waren verschwinden lassen
        // Nicht wie bei Anno 1602 an die ehemalige Stellen laufen lassen.
        assert(false); // TODO selbe Funktionalität wie oben
    }

    const Good* goodGrass = context.configMgr->getGood("grass");
    EightDirectionsAnimation animations = mapObjectType->getAnimationsForState(GraphicSetKeyState::WALKING_WITH_GOODS);

    initRoute(owningBuilding, routeBackToOwningBuilding, goodGrass, carriedGoods.capacity,
              false, animations, context.game->getTicks());
}

GoodsSlot* Carrier::findGoodsSlotToUnloadTo(Building* building, const Good* good) {
    bool isStorageBuilding = building->isStorageBuilding();

    // Lagergebäude: Waren der Siedlung gutschreiben
    if (isStorageBuilding) {
        Colony* colony = building->getColony();
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

        // TODO Träger über Config steuern. GoodSlots mit invisibleGoods unterstützen
        if (mapObjectType->name == "sheep" || mapObjectType->name == "cattle") {
            return &building->productionSlots.output;
        }

        assert(false); // Der Träger passt nicht zu den Gebäude-Slots!? Darf nicht passieren.
        return nullptr;
    }
}