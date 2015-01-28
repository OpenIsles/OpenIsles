#include <algorithm>
#include <cassert>
#include <cmath>
#include <set>
#include "config/ConfigMgr.h"
#include "economics/EconomicsMgr.h"
#include "game/Colony.h"
#include "game/Game.h"
#include "map/coords/MapCoords.h"
#include "map/Map.h"


EconomicsMgr::EconomicsMgr(const Context* const context) : ContextAware(context) {
}

EconomicsMgr::~EconomicsMgr() {
}

void EconomicsMgr::update(Structure* structure) {
    // Erstmal nur Gebäude machen
    Building* building = dynamic_cast<Building*>(structure);
    if (building == nullptr) {
        return;
    }

    updateProduction(building);
    updateCarrier(building);

    building->setLastUpdateTime(context->sdlTicks);
}

void EconomicsMgr::updateProduction(Building* building) {
    StructureType structureType = building->getStructureType();
    const BuildingConfig* buildingConfig = context->configMgr->getBuildingConfig(structureType);

    // Produziert eh nix bzw. Lager schon voll? Dann nix zu tun.
    if (!buildingConfig->getBuildingProduction()->output.isUsed() ||
        building->productionSlots.output.isInventoryFull()) {

        return;
    }

    unsigned int ticksPastSinceLastUpdate = context->sdlTicks - building->getLastUpdateTime();
    unsigned int ticksInputConsumed = 0, ticksInput2Consumed = 0; // Zeiten, in denen wirklich verbraucht wurde
    double inputConsumed, input2Consumed, outputProduced;   // verbrauchte/produzierte Güter
    double oneMinuteTicks = (double) 60000 / context->game->getSpeed();

    // Haben wir Eingabegüter, dann wird nur produziert, wie diese verfügbar sind
    if (buildingConfig->getBuildingProduction()->input.isUsed()) {
        inputConsumed = (double) ticksPastSinceLastUpdate / oneMinuteTicks * buildingConfig->inputConsumptionRate;

        // nur verbrauchen, was auch da is
        if (inputConsumed > building->productionSlots.input.inventory) {
            inputConsumed = building->productionSlots.input.inventory;
        }
        ticksInputConsumed = (unsigned int) (inputConsumed * oneMinuteTicks / buildingConfig->inputConsumptionRate);

        if (buildingConfig->getBuildingProduction()->input2.isUsed()) {
            input2Consumed = (double) ticksPastSinceLastUpdate / oneMinuteTicks * buildingConfig->input2ConsumptionRate;

            // nur verbrauchen, was auch da is
            if (input2Consumed > building->productionSlots.input2.inventory) {
                input2Consumed = building->productionSlots.input2.inventory;
            }
            ticksInput2Consumed = (unsigned int) (input2Consumed * oneMinuteTicks / buildingConfig->input2ConsumptionRate);
        }
    }

    // Minimum-Ticks ermitteln, in denen wirklich produziert wurde
    unsigned int ticksWeReallyProduced = ticksPastSinceLastUpdate;
    if (buildingConfig->getBuildingProduction()->input.isUsed()) {
        ticksWeReallyProduced = std::min(ticksWeReallyProduced, ticksInputConsumed);

        if (buildingConfig->getBuildingProduction()->input2.isUsed()) {
            ticksWeReallyProduced = std::min(ticksWeReallyProduced, ticksInput2Consumed);
        }
    }

    // Jetzt die Produktion durchführen
    if (buildingConfig->getBuildingProduction()->input.isUsed()) {
        inputConsumed = (double) ticksWeReallyProduced / oneMinuteTicks * buildingConfig->inputConsumptionRate;
        building->productionSlots.input.decreaseInventory(inputConsumed);

        if (buildingConfig->getBuildingProduction()->input2.isUsed()) {
            input2Consumed = (double) ticksWeReallyProduced / oneMinuteTicks * buildingConfig->input2ConsumptionRate;
            building->productionSlots.input2.decreaseInventory(input2Consumed);
        }
    }
    outputProduced = (double) ticksWeReallyProduced / oneMinuteTicks * buildingConfig->productionRate;
    building->productionSlots.output.increaseInventory(outputProduced);
}

void EconomicsMgr::updateCarrier(Building* building) {
    bool isStorageBuilding = building->isStorageBuilding();

    // Weder Lagergebäude (was Marktkarren hat), noch Produktionsgebäude (was Eingabegüter hat)? Nix zu tun
    if (!building->productionSlots.input.isUsed() && !isStorageBuilding) {
        return;
    }

    // Ist der Träger zu Hause? Gucken, wo was zu holen is und Abholung einleiten
    if (building->carrier == nullptr) {
        FindBuildingToGetGoodsFromResult result = findBuildingToGetGoodsFrom(building);
        if (result.building == nullptr) {
            return; // nix zu tun
        }

        // Träger anlegen und zuweisen
        const MapCoords& firstHopOnRoute = result.route->front();

        Carrier* carrier = new Carrier(result.building, result.route, result.goodsSlot.goodsType, true);
        carrier->setMapCoords((DoubleMapCoords) firstHopOnRoute);
        carrier->setAnimation(
            context->graphicsMgr->getGraphicSet(isStorageBuilding ? "cart-without-cargo" : "carrier")->getStatic());

        building->carrier = carrier;

        // Slot markieren, dass nicht ein zweiter Träger hinläuft.
        // Zu einem Lagergebäude dürfen natürlich mehrere hinlaufen und sich bedienen.
        if (!result.building->isStorageBuilding()) {
            result.building->productionSlots.output.markedForPickup = true;
        }

        return; // Erst beim nächsten Update bewegen wir den Träger
    }

    // Träger unterwegs? fortbewegen
    unsigned int ticksPastSinceLastUpdate = context->sdlTicks - building->getLastUpdateTime();
    double oneSecondTicks = (double) 1000 / context->game->getSpeed();

    Carrier* carrier = building->carrier;
    const Animation* animation = carrier->getAnimation();

    // Animieren
    carrier->animationFrame += (double) ticksPastSinceLastUpdate / oneSecondTicks * animation->getFps();
    while (carrier->animationFrame >= animation->getFramesCount()) {
        carrier->animationFrame -= animation->getFramesCount();
    }

    // Bewegen
    const double speed = 0.75; // Kacheln pro Sekunde (auf 1x Spielgeschwindigkeit), erstmal fix, die Animation muss man eh nochmal anpassen, weil das Männchen mehr schwebt, als läuft

    unsigned int ticksToProcess = ticksPastSinceLastUpdate;
    do {
        /* Wir gehen kachelweise vor. Erst ausrechnen, wie viele Ticks notwendig sind, um den nächsten Hop fertig zu
         * machen. Dann gehen wir soweit wir kommen. Bestcase wir schaffen den Hop, dann loopen wir zum nächsten.
         * Ansonsten rücken wir eben nur ein kleines Stückchen weiter.
         */

        const MapCoords& nextHopOnRoute = *carrier->nextHopInRoute;
        DoubleMapCoords& carrierMapCoords = carrier->getMapCoords();

        double mapDeltaXToNextHop = (nextHopOnRoute.x() - carrierMapCoords.x());
        double mapDeltaYToNextHop = (nextHopOnRoute.y() - carrierMapCoords.y());

        //  Halbe Ticks gibts nicht! Wir müssen hier runden.
        unsigned int ticksToNextHop = (unsigned int)
            (std::max(std::abs(mapDeltaXToNextHop), std::abs(mapDeltaYToNextHop)) * oneSecondTicks / speed);

        // Hop geschafft?
        if (ticksToNextHop <= ticksToProcess) {
            carrierMapCoords.setMapCoords(nextHopOnRoute);
            carrier->nextHopInRoute++;

            // Route fertig?
            if (carrier->nextHopInRoute == carrier->route->cend()) {
                MapTile* mapTile = context->game->getMap()->getMapTileAt(carrier->route->back());
                Building* targetBuilding = dynamic_cast<Building*>(mapTile->mapObject);

                // targetBuilding == nullptr -> Nutzer hat die Map inzwischen geändert und das Zielgebäude abgerissen
                // Der Träger verschwindet einfach
                if (targetBuilding == nullptr) {
                    delete carrier;
                    building->carrier = nullptr;
                }

                // Das war Hinweg -> Waren aufladen und Träger auf Rückweg schicken
                else if (carrier->onOutboundTrip) {
                    // Waren aufladen
                    GoodsSlot* goodsSlotToTakeFrom;
                    if (targetBuilding->isStorageBuilding()) {
                        // Lagergebäude: Waren aus der Siedlung aufladen
                        Colony* colony = context->game->getColony(targetBuilding);
                        goodsSlotToTakeFrom = &colony->getGoods(carrier->carriedGoods.goodsType);
                    } else {
                        // Waren aus dem Gebäude aufladen
                        goodsSlotToTakeFrom = &targetBuilding->productionSlots.output;
                    }

                    int goodsWeCollect = (int) goodsSlotToTakeFrom->inventory;

                    // Wir dürfen nicht mehr aufladen, als im Ziellager abgeladen werden kann, sonst gehen die Waren
                    // verloren. Wir müssen hier in Ganzzahlen denken: Haben wir 2,999t Platz, dürfen wir nicht 3t
                    // aufladen, sondern nur 2t.
                    GoodsSlot* goodsSlotWeWillUnloadToLater = findGoodsSlotToUnloadTo(building, carrier);
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
                    targetBuilding->lastGoodsCollections = context->sdlTicks;

                    Route* route = carrier->route;
                    AStar* aStar = new AStar(context, route->back(), route->front(), building, isStorageBuilding);
                    aStar->cutRouteInsideBuildings();
                    Route* returnRoute = aStar->getRoute();
                    delete aStar;

                    delete carrier;
                    building->carrier = nullptr;

                    // Es kann sein, dass es keine Rückroute gibt, wenn der Nutzer inzwischen die Map verändert hat.
                    // In diesem Fall hat er Pech gehabt, die Waren verschwinden mit dem Träger
                    if (returnRoute != nullptr) {
                        const MapCoords& firstHopOnReturnRoute = returnRoute->front();

                        Carrier* returnCarrier = new Carrier(
                            building, returnRoute, goodsSlotToTakeFrom->goodsType, false);
                        returnCarrier->setMapCoords((DoubleMapCoords) firstHopOnReturnRoute);
                        returnCarrier->setAnimation(
                            context->graphicsMgr->getGraphicSet(isStorageBuilding ? "cart-with-cargo" : "carrier")->getStatic());
                        returnCarrier->carriedGoods.inventory = goodsWeCollect;

                        building->carrier = returnCarrier;

                        // Slot wieder freimachen
                        if (goodsSlotToTakeFrom->markedForPickup) {
                            goodsSlotToTakeFrom->markedForPickup = false;
                        }
                    }
                }

                // Das war Rückweg -> Waren ausladen und Träger zerstören
                else if (!carrier->onOutboundTrip) {
                    GoodsSlot* goodsSlotToUnloadTo = findGoodsSlotToUnloadTo(building, carrier);
                    goodsSlotToUnloadTo->increaseInventory(carrier->carriedGoods.inventory);

                    delete carrier;
                    building->carrier = nullptr;
                }

                break; // Loop verlassen. Rückweg erst im nächsten Update, wir sind hier fertig.
            }

            ticksToProcess -= ticksToNextHop;
        }
        // Hop nicht ganz geschafft,
        else {
            // Anteil der Reststrecke in der Kachel ausrechnen, die wir schaffen
            double fractionOfRestToNextHop = (double) ticksToProcess / (double) ticksToNextHop;

            carrierMapCoords.addX(mapDeltaXToNextHop * fractionOfRestToNextHop);
            carrierMapCoords.addY(mapDeltaYToNextHop * fractionOfRestToNextHop);

            break; // ticksToProcess auf 0 setzen -> können wir gleich die Schleife brechen :-)
        }

    } while(ticksToProcess > 0);
}

GoodsSlot* EconomicsMgr::findGoodsSlotToUnloadTo(Building* building, Carrier* carrier) {
    bool isStorageBuilding = building->isStorageBuilding();

    // Lagergebäude: Waren der Siedlung gutschreiben
    if (isStorageBuilding) {
        Colony* colony = context->game->getColony(building);
        return &colony->getGoods(carrier->carriedGoods.goodsType);
    }
        // Produktionsgebäude: Gucken, in welchen Slot die Waren müssen
    else {
        if (building->productionSlots.input.goodsType == carrier->carriedGoods.goodsType) {
            return &building->productionSlots.input;
        }
        else if (building->productionSlots.input2.goodsType == carrier->carriedGoods.goodsType) {
            return &building->productionSlots.input2;
        }

        assert(false); // Der Träger passt nicht zu den Gebäude-Slots!? Darf nicht passieren.
        return nullptr;
    }
}

FindBuildingToGetGoodsFromResult EconomicsMgr::findBuildingToGetGoodsFrom(Building* building) {
    StructureType structureType = building->getStructureType();
    const BuildingConfig* buildingConfig = context->configMgr->getBuildingConfig(structureType);
    const RectangleData<char>* catchmentArea = buildingConfig->getCatchmentArea();
    if (catchmentArea == nullptr) {
        return FindBuildingToGetGoodsFromResult(); // kein Einzugsbereich
    }

    /*
     * Anno 1602 macht es so: Ein Gebäude, was zwei Waren braucht, schickt den Träger hintereinander los für die Waren,
     * Nur, wenn die erste Ware auf Lager ist, wird der Träger mit dem Auftrag, die zweite Waren zu holen, losgeschickt.
     * Ist die erste Ware nicht verfügbar, wird die zweite Ware auch niemals geholt, selbst wie diese da wäre.
     *
     * Wir verbessern diesen Mechanismus: Wir versuchen grundsätzlich so viele Waren wie möglich abzuholen. Stehen
     * mehrere zur Verfügung, so  wählen wir die beste Alternative, nämlich die für die Ware, die drigender gebraucht
     * wird. Somit würde die zweite Ware geholt werden, selbst wenn die erste noch nicht zur Verfügung steht.
     */

    // Welche Ware brauchen wir überhaupt?
    GoodsType goodsRequired1 = GoodsType::NO_GOODS;
    GoodsType goodsRequired2 = GoodsType::NO_GOODS;

    bool isStorageBuilding = building->isStorageBuilding();
    if (!isStorageBuilding) {
        goodsRequired1 = buildingConfig->getBuildingProduction()->input.goodsType;
        goodsRequired2 = buildingConfig->getBuildingProduction()->input2.goodsType;

        // Wir brauchen nur, wenn die Lager nicht voll sind
        if (goodsRequired1 != GoodsType::NO_GOODS) {
            if (building->productionSlots.input.getRemainingCapacity() < 1) {
                goodsRequired1 = GoodsType::NO_GOODS;
            }
        }
        if (goodsRequired2 != GoodsType::NO_GOODS) {
            if (building->productionSlots.input2.getRemainingCapacity() < 1) {
                goodsRequired2 = GoodsType::NO_GOODS;
            }
        }

        if (goodsRequired1 == GoodsType::NO_GOODS && goodsRequired2 == GoodsType::NO_GOODS) {
            return FindBuildingToGetGoodsFromResult(); // Wir brauchen (aktuell?) keine Waren
        }
    }

    // Meinen Einzugsbereich durchsehen und Liste aller Gebäude innerhalb anfertigen, die als Ziel in Frage kommen
    std::list<FindBuildingToGetGoodsFromResult> potentialResults;

    std::set<Building*> buildingChecked; // Gebäude in dieses Set aufnehmen, wenn wir es schon behandelt haben.
                                         // Da wir kachelweise arbeiten, erhalten wir dasselbe Gebäude mehrfach.

    Map* map = context->game->getMap();
    for (int y = 0; y < catchmentArea->height; y++) {
        for (int x = 0; x < catchmentArea->width; x++) {
            int mapWidth = building->getMapWidth();
            int mapHeight = building->getMapHeight();

            const MapCoords& mapCoords = building->getMapCoords();
            int mapX = mapCoords.x() + (x - (catchmentArea->width - mapWidth) / 2);
            int mapY = mapCoords.y() + (y - (catchmentArea->height - mapHeight) / 2);

            // Gebäude da?
            MapTile* mapTile = map->getMapTileAt(MapCoords(mapX, mapY));
            if (mapTile == nullptr) {
                continue;
            }

            Building* buildingThere = dynamic_cast<Building*>(mapTile->mapObject);
            if (buildingThere == nullptr) {
                continue;
            }

            // Gebäude in Set einfügen bzw. gucken, ob wir das Gebäude schon haben
            std::pair<std::set<Building*>::iterator, bool> setInsertionResult = buildingChecked.insert(buildingThere);
            if (!setInsertionResult.second) {
                continue; // Dieses Gebäude hatten wir schon mal
            }

            // Von Lagergebäude zu Lagergebäude wird nix transportiert
            bool isStorgeBuildingThere = buildingThere->isStorageBuilding();
            if (isStorageBuilding && isStorgeBuildingThere) {
                continue;
            }

            // Gebäude, die gar nix produzieren, bringen uns nix, z.B. öffentliche Gebäude.
            const BuildingConfig* buildingThereConfig = context->configMgr->getBuildingConfig(buildingThere->getStructureType());
            if (!isStorgeBuildingThere && !buildingThereConfig->getBuildingProduction()->output.isUsed()) {
                continue;
            }

            // Liefert das Gebäude was passendes?
            if (!isStorageBuilding && !isStorgeBuildingThere && (
                    buildingThereConfig->getBuildingProduction()->output.goodsType != goodsRequired1 &&
                    buildingThereConfig->getBuildingProduction()->output.goodsType != goodsRequired2)) {

                continue; // produziert nix passendes
            }

            // Von diesem Slot holt schon ein anderer Träger ab. Verhindern, dass zwei Träger zum selben Ziel
            // unterwegs sind.
            if (!isStorgeBuildingThere && buildingThere->productionSlots.output.markedForPickup) {
                continue;
            }

            // Waren zum Abholen da?
            if (!isStorgeBuildingThere && buildingThere->productionSlots.output.inventory < 1) {
                continue;
            }

            // Bin ich ein Lagergebäude?
            if (isStorageBuilding) {
                Colony* colony = context->game->getColony(building);

                GoodsSlot* buildingThereOutputProductionSlot = &buildingThere->productionSlots.output;

                // Wir holen nur ab, wenn die Lager der Siedlung nicht schon voll sind
                if (colony->getGoods(buildingThereOutputProductionSlot->goodsType).getRemainingCapacity() < 1) {
                    continue;
                }

                // Gebäude, die Rohstoffe herstellen, werden grundsätzlich nur vom Marktkarren abgeholt,
                // wenn die Lager voll sind
                if (buildingThereOutputProductionSlot->isRawMaterial() &&
                    !buildingThereOutputProductionSlot->isInventoryFull()) {

                    continue;
                }
            }

            // Checken, ob eine Route dahin existiert
            const MapCoords& mapCoordsSource = building->getMapCoords();
            const MapCoords& mapCoordsDestination = buildingThere->getMapCoords();

            AStar* aStar = new AStar(context, mapCoordsSource, mapCoordsDestination, building, isStorageBuilding);
            aStar->cutRouteInsideBuildings();
            Route* route = aStar->getRoute();
            delete aStar;
            if (route == nullptr) {
                continue; // gibt keinen Weg dahin
            }

            FindBuildingToGetGoodsFromResult potentialResult;
            potentialResult.building = buildingThere;
            potentialResult.route = route;

            // Ist das Zielgebäude ein Lagergebäude? Produktionsgebäuden können sich Waren auch von dort holen
            if (isStorgeBuildingThere) {
                Colony* colony = context->game->getColony(buildingThere); // TODO Es sollte getColony(buildingThere) == getColony(building) gelten, da Kolonie-übergreifend eh nix gehen darf.

                bool goods1CanBePickedUpFromStorage = false;
                bool goods2CanBePickedUpFromStorage = false;

                if ((goodsRequired1 != GoodsType::NO_GOODS) && colony->getGoods(goodsRequired1).inventory > 0) {
                    goods1CanBePickedUpFromStorage = true;
                }
                if ((goodsRequired2 != GoodsType::NO_GOODS) && colony->getGoods(goodsRequired2).inventory > 0) {
                    goods2CanBePickedUpFromStorage = true;
                }

                GoodsType goodsTypeWeChoose;

                // Nix passendes in der Kolonie?
                if (!goods1CanBePickedUpFromStorage && !goods2CanBePickedUpFromStorage) {
                    continue;
                }

                // Ein Lagerhaus bietet unter Umständen zwei Waren an, die wir brauchen. Falls ja, entscheiden wir
                // uns hier bereits, welche wir haben wollen. Wir wählen die, wo wir prozentual weniger auf Lager
                // haben.
                else if (goods1CanBePickedUpFromStorage && goods2CanBePickedUpFromStorage) {
                    if ((building->productionSlots.input.inventory / building->productionSlots.input.capacity) <
                        (building->productionSlots.input2.inventory / building->productionSlots.input2.capacity)) {

                        goodsTypeWeChoose = goodsRequired1;
                    } else {
                        goodsTypeWeChoose = goodsRequired2;
                    }
                }

                // Nur genaue eine Ware verfügbar?
                else if (goods1CanBePickedUpFromStorage) {
                    goodsTypeWeChoose = goodsRequired1;
                } else if (goods2CanBePickedUpFromStorage) {
                    goodsTypeWeChoose = goodsRequired2;
                }

                potentialResult.goodsSlot.goodsType = goodsTypeWeChoose;
                potentialResult.goodsSlot.inventory = colony->getGoods(goodsTypeWeChoose).inventory;
                potentialResult.goodsSlot.capacity = colony->getGoods(goodsTypeWeChoose).capacity;
                potentialResult.lastGoodsCollections = context->sdlTicks + 1; // Zeit in der Zukunft nehmen, damit diese Route als letztes verwendet wird
            }
            else {
                potentialResult.goodsSlot.goodsType = buildingThereConfig->getBuildingProduction()->output.goodsType;
                potentialResult.goodsSlot.inventory = buildingThere->productionSlots.output.inventory;
                potentialResult.goodsSlot.capacity = buildingThere->productionSlots.output.capacity;
                potentialResult.lastGoodsCollections = buildingThere->lastGoodsCollections;
            }

            // Juhuu! Dieses Gebäude kommt in Frage
            potentialResults.push_back(potentialResult);
        }
    }

    // Keine Ziele? Ok, dann sind wir fertig
    if (potentialResults.empty()) {
        return FindBuildingToGetGoodsFromResult();
    }

    // Liste sortieren und besten (siehe Methodenkommentar) Treffer wählen:
    struct FindBuildingToGetGoodsFromResultComparator {
        bool operator() (const FindBuildingToGetGoodsFromResult& result1,
                         const FindBuildingToGetGoodsFromResult& result2) const {

            // höherer Lagerbestand hat Vorrang
            double inventoryRatio1 = result1.goodsSlot.inventory / (double) result1.goodsSlot.capacity;
            double inventoryRatio2 = result2.goodsSlot.inventory / (double) result2.goodsSlot.capacity;

            if (inventoryRatio1 > inventoryRatio2) {
                return true;
            } else if (inventoryRatio1 < inventoryRatio2) {
                return false;
            }

            // Lagerbestand ist gleich: Dann den nehmen, der länger nicht abgeholt wurde
            return result1.lastGoodsCollections < result2.lastGoodsCollections;
        }
    };

    potentialResults.sort(FindBuildingToGetGoodsFromResultComparator());
    FindBuildingToGetGoodsFromResult bestResult = potentialResults.front();

#ifdef DEBUG_ECONOMICS
    const MapCoords& mapCoords = building->getMapCoords();

    std::cout << "potentialResults for (" << mapCoords.x() << ", " << mapCoords.y() << "):" << std::endl;

    int i = 1;
    for (auto iter = potentialResults.cbegin(); iter != potentialResults.cend(); iter++, i++) {
        std::cout << i << ": " << *iter << std::endl;
    }
    std::cout << "-----------------------------" << std::endl;
#endif

    return bestResult;
}
