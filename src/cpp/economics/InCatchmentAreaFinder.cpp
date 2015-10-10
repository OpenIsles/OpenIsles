#include "economics/InCatchmentAreaFinder.h"
#include "game/Game.h"

/**
 * @brief Hilfsstruktur, die ein potentielles Ergebnis enthält.
 */
struct PotentialInCatchmentAreaFinderResult : public InCatchmentAreaFinderResult {

    /**
     * @brief GoodsSlot. Enthält die Info, welchen Warentyp wir dort holen sollen,
     * wie hoch der Lagerbestand und die Lagerkapazität ist (um auswerten zu können, wie "gut" dieses Ergebnis ist)
     */
    GoodsSlot goodsSlot;

    /**
     * @brief [Spielzeitpunkt](@ref gameTicks), wann zuletzt
     * - eine Abholung der Waren bei diesem Gebäude bzw
     * - eine Ernte auf dieser Map-Kachel
     * stattgefunden hat
     */
    unsigned long lastGoodsCollections;
};



InCatchmentAreaFinder::InCatchmentAreaFinder(
    const Context* const context, CatchmentAreaIterator* catchmentAreaIterator) :
        ContextAware(context), catchmentAreaIterator(catchmentAreaIterator) {}


InCatchmentAreaFinderResult InCatchmentAreaFinder::findBuildingWithGoodsToStore() {
    return findBuildingWithGoods(true, nullptr);
}

InCatchmentAreaFinderResult InCatchmentAreaFinder::findBuildingWithGoods(std::list<const Good*> goods) {
    return findBuildingWithGoods(false, &goods);
}

InCatchmentAreaFinderResult InCatchmentAreaFinder::findBuildingWithGoods(
    bool isStorageBuilding, const std::list<const Good*>* goods) {

    /*
     * Anno 1602 macht es so: Ein Gebäude, was zwei Waren braucht, schickt den Träger hintereinander los für die Waren,
     * Nur, wenn die erste Ware auf Lager ist, wird der Träger mit dem Auftrag, die zweite Waren zu holen, losgeschickt.
     * Ist die erste Ware nicht verfügbar, wird die zweite Ware auch niemals geholt, selbst wie diese da wäre.
     *
     * Wir verbessern diesen Mechanismus: Wir versuchen grundsätzlich so viele Waren wie möglich abzuholen. Stehen
     * mehrere zur Verfügung, so wählen wir die beste Alternative, nämlich die für die Ware, die drigender gebraucht
     * wird. Somit würde die zweite Ware geholt werden, selbst wenn die erste noch nicht zur Verfügung steht.
     */


    // Liste aller Gebäude innerhalb anfertigen, die als Ziel in Frage kommen
    std::list<PotentialInCatchmentAreaFinderResult> potentialResults;

    std::set<Building*> buildingChecked; // Gebäude in dieses Set aufnehmen, wenn wir es schon behandelt haben.
                                         // Da wir kachelweise arbeiten, erhalten wir dasselbe Gebäude mehrfach.

    Map* map = context->game->getMap();
    AStar aStar(context, catchmentAreaIterator, true, isStorageBuilding, false);

    catchmentAreaIterator->iterate([&](const MapCoords& mapCoords) {
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
        auto setInsertionResult = buildingChecked.insert(buildingThere);
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
        if (!isStorageBuilding && !isStorgeBuildingThere) {
            bool foundAnyGood = std::any_of(goods->cbegin(), goods->cend(), [&](const Good* good) {
                return (buildingThereType->buildingProduction.output.good == good);
            });

            if (!foundAnyGood) {
                return; // produziert nix passendes
            }
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

        // Suchen wir für ein Lagergebäude?
        if (isStorageBuilding) {
            const Colony* colony = catchmentAreaIterator->getColony();

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
        const MapCoords& mapCoordsSource = catchmentAreaIterator->getMapCoordsCentered();
        const MapCoords& mapCoordsDestination = buildingThere->getMapCoords();

        Route route = aStar.getRoute(mapCoordsSource, mapCoordsDestination);
        if (!route.routeExists()) {
            return; // gibt keinen Weg dahin
        }

        PotentialInCatchmentAreaFinderResult potentialResult;
        potentialResult.foundSomething = true;
        potentialResult.mapObject = buildingThere;
        potentialResult.route = route;

        // Ist das Zielgebäude ein Lagergebäude? Produktionsgebäuden können sich Waren auch von dort holen
        if (isStorgeBuildingThere) {
            assert(goods != nullptr); // "Lagergebäude sucht Lagergebäude" -> haben wir oben schon abgehakt.

            Colony* colony = buildingThere->getColony(); // TODO Es sollte buildingThere->getColony() == building->getColony() gelten, da Kolonie-übergreifend eh nix gehen darf.

            const Good* goodWeChoose = nullptr;
            for (const Good* good : *goods) {
                if (colony->getGoods(good).inventory > 0) {
                    goodWeChoose = good;
                    break;
                }
            }

            // Nix passendes in der Kolonie?
            if (goodWeChoose == nullptr) {
                return;
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
        return InCatchmentAreaFinderResult();
    }

    // Liste sortieren und "besten" Treffer wählen:
    auto resultComparator = [&](const PotentialInCatchmentAreaFinderResult& result1,
                                const PotentialInCatchmentAreaFinderResult& result2) {

        // Zuerst die Priorität der Güter berücksichtigen
        if ((goods != nullptr) && (result1.goodsSlot.good != result2.goodsSlot.good)) {
            int prio1 = -1;
            int prio2 = -1;
            int prio = 0;
            for (const Good* good : *goods) {
                if (result1.goodsSlot.good == good) {
                    prio1 = prio;
                }
                if (result2.goodsSlot.good == good) {
                    prio2 = prio;
                }

                prio++;
            }
            assert((prio1 != -1) && (prio2 != -1) && (prio1 != prio2));

            return (prio1 < prio2);
        }

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
    };

    potentialResults.sort(resultComparator);
    PotentialInCatchmentAreaFinderResult bestResult = potentialResults.front();
    bestResult.good = bestResult.goodsSlot.good;

    return bestResult;
}
