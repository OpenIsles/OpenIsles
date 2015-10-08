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
#include "utils/CatchmentAreaIterator.h"


EconomicsMgr::EconomicsMgr(const Context* const context) : ContextAware(context) {
}

EconomicsMgr::~EconomicsMgr() {
}

void EconomicsMgr::updateProduction(Building* building) {
    const MapObjectType* mapObjectType = building->getMapObjectType();

    // Produziert eh nix bzw. Lager schon voll? Dann nix zu tun.
    if (!mapObjectType->buildingProduction.output.isUsed() ||
        building->productionSlots.output.isInventoryFull()) {

        return;
    }

    unsigned int ticksSinceLastUpdate = building->getTicksSinceLastUpdate(*context);
    unsigned int ticksInputConsumed = 0, ticksInput2Consumed = 0; // Zeiten, in denen wirklich verbraucht wurde
    double inputConsumed, input2Consumed, outputProduced;   // verbrauchte/produzierte Güter

    // Haben wir Eingabegüter, dann wird nur produziert, wie diese verfügbar sind
    if (mapObjectType->buildingProduction.input.isUsed()) {
        inputConsumed =
            (double) ticksSinceLastUpdate / TICKS_PER_MINUTE * mapObjectType->buildingProduction.input.rate;

        // nur verbrauchen, was auch da is
        if (inputConsumed > building->productionSlots.input.inventory) {
            inputConsumed = building->productionSlots.input.inventory;
        }
        ticksInputConsumed =
            (unsigned int) (inputConsumed * TICKS_PER_MINUTE / mapObjectType->buildingProduction.input.rate);

        if (mapObjectType->buildingProduction.input2.isUsed()) {
            input2Consumed =
                (double) ticksSinceLastUpdate / TICKS_PER_MINUTE * mapObjectType->buildingProduction.input2.rate;

            // nur verbrauchen, was auch da is
            if (input2Consumed > building->productionSlots.input2.inventory) {
                input2Consumed = building->productionSlots.input2.inventory;
            }
            ticksInput2Consumed =
                (unsigned int) (input2Consumed * TICKS_PER_MINUTE / mapObjectType->buildingProduction.input2.rate);
        }
    }

    // Minimum-Ticks ermitteln, in denen wirklich produziert wurde
    unsigned int ticksWeReallyProduced = ticksSinceLastUpdate;
    if (mapObjectType->buildingProduction.input.isUsed()) {
        ticksWeReallyProduced = std::min(ticksWeReallyProduced, ticksInputConsumed);

        if (mapObjectType->buildingProduction.input2.isUsed()) {
            ticksWeReallyProduced = std::min(ticksWeReallyProduced, ticksInput2Consumed);
        }
    }

    // Jetzt die Produktion durchführen
    if (mapObjectType->buildingProduction.input.isUsed()) {
        inputConsumed = (double) ticksWeReallyProduced / TICKS_PER_MINUTE * mapObjectType->buildingProduction.input.rate;
        building->productionSlots.input.decreaseInventory(inputConsumed);

        if (mapObjectType->buildingProduction.input2.isUsed()) {
            input2Consumed =
                (double) ticksWeReallyProduced / TICKS_PER_MINUTE * mapObjectType->buildingProduction.input2.rate;
            building->productionSlots.input2.decreaseInventory(input2Consumed);
        }
    }
    outputProduced = (double) ticksWeReallyProduced / TICKS_PER_MINUTE * mapObjectType->buildingProduction.output.rate;
    building->productionSlots.output.increaseInventory(outputProduced);
}

FindBuildingToGetGoodsFromResult EconomicsMgr::findBuildingToGetGoodsFrom(Building* building) {
    const MapObjectType* mapObjectType = building->getMapObjectType();
    if (!mapObjectType->catchmentArea) {
        return FindBuildingToGetGoodsFromResult(); // kein Einzugsbereich
    }

    /*
     * Anno 1602 macht es so: Ein Gebäude, was zwei Waren braucht, schickt den Träger hintereinander los für die Waren,
     * Nur, wenn die erste Ware auf Lager ist, wird der Träger mit dem Auftrag, die zweite Waren zu holen, losgeschickt.
     * Ist die erste Ware nicht verfügbar, wird die zweite Ware auch niemals geholt, selbst wie diese da wäre.
     *
     * Wir verbessern diesen Mechanismus: Wir versuchen grundsätzlich so viele Waren wie möglich abzuholen. Stehen
     * mehrere zur Verfügung, so wählen wir die beste Alternative, nämlich die für die Ware, die drigender gebraucht
     * wird. Somit würde die zweite Ware geholt werden, selbst wenn die erste noch nicht zur Verfügung steht.
     */

    // Welche Ware brauchen wir überhaupt?
    const Good* goodRequired1 = nullptr;
    const Good* goodRequired2 = nullptr;

    bool isStorageBuilding = building->isStorageBuilding();
    if (!isStorageBuilding) {
        goodRequired1 = mapObjectType->buildingProduction.input.good;
        goodRequired2 = mapObjectType->buildingProduction.input2.good;

        // Wir brauchen nur, wenn die Lager nicht voll sind
        if (goodRequired1 != nullptr) {
            if (building->productionSlots.input.getRemainingCapacity() < 1) {
                goodRequired1 = nullptr;
            }
        }
        if (goodRequired2 != nullptr) {
            if (building->productionSlots.input2.getRemainingCapacity() < 1) {
                goodRequired2 = nullptr;
            }
        }

        if (goodRequired1 == nullptr && goodRequired2 == nullptr) {
            return FindBuildingToGetGoodsFromResult(); // Wir brauchen (aktuell?) keine Waren
        }
    }

    // Meinen Einzugsbereich durchsehen und Liste aller Gebäude innerhalb anfertigen, die als Ziel in Frage kommen
    std::list<FindBuildingToGetGoodsFromResult> potentialResults;

    std::set<Building*> buildingChecked; // Gebäude in dieses Set aufnehmen, wenn wir es schon behandelt haben.
                                         // Da wir kachelweise arbeiten, erhalten wir dasselbe Gebäude mehrfach.

    Map* map = context->game->getMap();
    CatchmentAreaIterator catchmentAreaIterator(*building, false);
    AStar aStar(context, &catchmentAreaIterator, true, isStorageBuilding, false);

    catchmentAreaIterator.iterate([&](const MapCoords& mapCoords) {
        // Gebäude da?
        MapTile* mapTile = map->getMapTileAt(mapCoords);
        if (mapTile == nullptr) {
            return;
        }

        Building* buildingThere = dynamic_cast<Building*>(mapTile->mapObjectFixed);
        if (buildingThere == nullptr) {
            return;
        }

        // Gebäude in Set einfügen bzw. gucken, ob wir das Gebäude schon haben
        std::pair<std::set<Building*>::iterator, bool> setInsertionResult = buildingChecked.insert(buildingThere);
        if (!setInsertionResult.second) {
            return; // Dieses Gebäude hatten wir schon mal
        }

        // Von Lagergebäude zu Lagergebäude wird nix transportiert
        bool isStorgeBuildingThere = buildingThere->isStorageBuilding();
        if (isStorageBuilding && isStorgeBuildingThere) {
            return;
        }

        // Gebäude, die gar nix produzieren, bringen uns nix, z.B. öffentliche Gebäude.
        const MapObjectType* buildingThereType = buildingThere->getMapObjectType();
        if (!isStorgeBuildingThere && !buildingThereType->buildingProduction.output.isUsed()) {
            return;
        }

        // Liefert das Gebäude was passendes?
        if (!isStorageBuilding && !isStorgeBuildingThere && (
                buildingThereType->buildingProduction.output.good != goodRequired1 &&
                buildingThereType->buildingProduction.output.good != goodRequired2)) {

            return; // produziert nix passendes
        }

        // Von diesem Slot holt schon ein anderer Träger ab. Verhindern, dass zwei Träger zum selben Ziel
        // unterwegs sind.
        if (!isStorgeBuildingThere && buildingThere->productionSlots.output.markedForPickup) {
            return;
        }

        // Waren zum Abholen da?
        if (!isStorgeBuildingThere && buildingThere->productionSlots.output.inventory < 1) {
            return;
        }

        // Bin ich ein Lagergebäude?
        if (isStorageBuilding) {
            Colony* colony = context->game->getColony(building);

            GoodsSlot* buildingThereOutputProductionSlot = &buildingThere->productionSlots.output;

            // Wir holen nur ab, wenn die Lager der Siedlung nicht schon voll sind
            if (colony->getGoods(buildingThereOutputProductionSlot->good).getRemainingCapacity() < 1) {
                return;
            }

            // Gebäude, die Rohstoffe herstellen, werden grundsätzlich nur vom Marktkarren abgeholt,
            // wenn die Lager voll sind
            if (buildingThereOutputProductionSlot->isRawMaterial() &&
                !buildingThereOutputProductionSlot->isInventoryFull()) {

                return;
            }
        }

        // Checken, ob eine Route dahin existiert
        const MapCoords& mapCoordsSource = building->getMapCoords();
        const MapCoords& mapCoordsDestination = buildingThere->getMapCoords();

        Route route = aStar.getRoute(mapCoordsSource, mapCoordsDestination);
        if (!route.routeExists()) {
            return; // gibt keinen Weg dahin
        }

        FindBuildingToGetGoodsFromResult potentialResult;
        potentialResult.building = buildingThere;
        potentialResult.route = route;

        // Ist das Zielgebäude ein Lagergebäude? Produktionsgebäuden können sich Waren auch von dort holen
        if (isStorgeBuildingThere) {
            Colony* colony = context->game->getColony(buildingThere); // TODO Es sollte getColony(buildingThere) == getColony(building) gelten, da Kolonie-übergreifend eh nix gehen darf.

            bool goods1CanBePickedUpFromStorage = false;
            bool goods2CanBePickedUpFromStorage = false;

            if ((goodRequired1 != nullptr) && colony->getGoods(goodRequired1).inventory > 0) {
                goods1CanBePickedUpFromStorage = true;
            }
            if ((goodRequired2 != nullptr) && colony->getGoods(goodRequired2).inventory > 0) {
                goods2CanBePickedUpFromStorage = true;
            }

            const Good* goodWeChoose;

            // Nix passendes in der Kolonie?
            if (!goods1CanBePickedUpFromStorage && !goods2CanBePickedUpFromStorage) {
                return;
            }

            // Ein Lagerhaus bietet unter Umständen zwei Waren an, die wir brauchen. Falls ja, entscheiden wir
            // uns hier bereits, welche wir haben wollen. Wir wählen die, wo wir prozentual weniger auf Lager
            // haben.
            else if (goods1CanBePickedUpFromStorage && goods2CanBePickedUpFromStorage) {
                if ((building->productionSlots.input.inventory / building->productionSlots.input.capacity) <
                    (building->productionSlots.input2.inventory / building->productionSlots.input2.capacity)) {

                       goodWeChoose = goodRequired1;
                } else {
                    goodWeChoose = goodRequired2;
                }
            }

            // Nur genaue eine Ware verfügbar?
            else if (goods1CanBePickedUpFromStorage) {
                goodWeChoose = goodRequired1;
            } else if (goods2CanBePickedUpFromStorage) {
                goodWeChoose = goodRequired2;
            }

            potentialResult.goodsSlot.good = goodWeChoose;
            potentialResult.goodsSlot.inventory = colony->getGoods(goodWeChoose).inventory;
            potentialResult.goodsSlot.capacity = colony->getGoods(goodWeChoose).capacity;
            potentialResult.lastGoodsCollections = context->game->getTicks() + 1; // Zeit in der Zukunft nehmen, damit diese Route als letztes verwendet wird
        }
        else {
            potentialResult.goodsSlot.good = buildingThereType->buildingProduction.output.good;
            potentialResult.goodsSlot.inventory = buildingThere->productionSlots.output.inventory;
            potentialResult.goodsSlot.capacity = buildingThere->productionSlots.output.capacity;
            potentialResult.lastGoodsCollections = buildingThere->lastGoodsCollections;
        }

        // Juhuu! Dieses Gebäude kommt in Frage
        potentialResults.push_back(potentialResult);
    });

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

    return bestResult;
}
