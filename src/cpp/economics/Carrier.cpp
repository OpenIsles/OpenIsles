#include <cmath>
#include "economics/Carrier.h"
#include "game/Colony.h"
#include "game/Game.h"


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
    const Animation* animation = getAnimations()[0]; // TODO wir brauchen nur fps und framesCount. Das sollte in allen Richtungen gleich sein.

    animationFrame += (double) ticksSinceLastUpdate / (double) TICKS_PER_SECOND * animation->getFps();
    while (animationFrame >= animation->getFramesCount()) {
        animationFrame -= animation->getFramesCount();
    }

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
    // TODO verallgemeinern
    if (mapObjectType->name == "sheep") {
        // Das war Hinweg zum Erntefeld -> Animation umstellen und mit Ernten anfangen
        if (onOutboundTrip) {
            const std::string& graphicName = (carriedGoods.inventory > 0) ? "sheep1" : "sheep0";

            animations = context.graphicsMgr->getGraphicSet(graphicName)->getEightDirectionsAnimation("eating");
            animationFrame = 0;
            state = HARVESTING;
            harvestingFinishedTicks = context.game->getTicks() + mapObjectType->secondsToProduce * TICKS_PER_SECOND;
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

        int goodsWeCollect = (int) goodsSlotToTakeFrom->inventory;

        // Wir dürfen nicht mehr aufladen, als im Ziellager abgeladen werden kann, sonst gehen die Waren
        // verloren. Wir müssen hier in Ganzzahlen denken: Haben wir 2,999t Platz, dürfen wir nicht 3t
        // aufladen, sondern nur 2t.
        GoodsSlot* goodsSlotWeWillUnloadToLater =
            findGoodsSlotToUnloadTo(owningBuilding, carriedGoods.good);
        int remainingCapacityToUnloadLater = (int) (
            (double) goodsSlotWeWillUnloadToLater->capacity - goodsSlotWeWillUnloadToLater->inventory);
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
        AStar aStar(&context, &catchmentAreaIterator, true, owningBuilding->isStorageBuilding(), false);
        Route returnRoute = aStar.getRoute(route.back(), route.front());

        deleteMe = true;
        owningBuilding->carriers.erase(this);

        // Es kann sein, dass es keine Rückroute gibt, wenn der Nutzer inzwischen die Map verändert hat.
        // In diesem Fall hat er Pech gehabt, die Waren verschwinden mit dem Träger
        if (returnRoute.routeExists()) {
            const GraphicSet& graphicSet = *context.graphicsMgr->getGraphicSet(
                owningBuilding->isStorageBuilding() ? "cart-with-cargo" : "carrier");

            Carrier* returnCarrier = (Carrier*) MapObject::instantiate(mapObjectType);
            returnCarrier->initRoute(
                owningBuilding, returnRoute, goodsSlotToTakeFrom->good, mapObjectType->carrier.capacity,
                false, graphicSet.getEightDirectionsAnimation(), context.game->getTicks());
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

            InCatchmentAreaFinder inCatchmentAreaFinder(&context, &catchmentAreaIterator);
            InCatchmentAreaFinderResult result;

            result = inCatchmentAreaFinder.findMapTileWithInvisibleGood(goodGrass, currentMapCoords);
            if (result) {
                EightDirectionsAnimation animations =
                    context.graphicsMgr->getGraphicSet("sheep1")->getEightDirectionsAnimation("walking");

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
    AStar aStar(&context, &catchmentAreaIterator, true, false, false);
    Route routeBackToOwningBuilding = aStar.getRoute(
        MapCoords((int) getMapCoords().x(), (int) getMapCoords().y()), owningBuilding->getMapCoords());

    if (!routeBackToOwningBuilding.routeExists()) {
        // Spezialfall: Gebäude inzwischen abgerissen -> Träger mitsamt Waren verschwinden lassen
        // Nicht wie bei Anno 1602 an die ehemalige Stellen laufen lassen.
        assert(false); // TODO selbe Funktionalität wie oben
    }

    const Good* goodGrass = context.configMgr->getGood("grass");
    EightDirectionsAnimation animations =
                    context.graphicsMgr->getGraphicSet("sheep1")->getEightDirectionsAnimation("walking");

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
        if (mapObjectType->name == "sheep") {
            return &building->productionSlots.output;
        }

        assert(false); // Der Träger passt nicht zu den Gebäude-Slots!? Darf nicht passieren.
        return nullptr;
    }
}