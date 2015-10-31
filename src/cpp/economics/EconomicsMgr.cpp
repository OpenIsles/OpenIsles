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
            (double) ticksSinceLastUpdate / (double) TICKS_PER_MINUTE * mapObjectType->buildingProduction.input.rate;

        // nur verbrauchen, was auch da is
        if (inputConsumed > building->productionSlots.input.inventory) {
            inputConsumed = building->productionSlots.input.inventory;
        }
        ticksInputConsumed =
            (unsigned int) (inputConsumed * (double) TICKS_PER_MINUTE / mapObjectType->buildingProduction.input.rate);

        if (mapObjectType->buildingProduction.input2.isUsed()) {
            input2Consumed =
                (double) ticksSinceLastUpdate / (double) TICKS_PER_MINUTE * mapObjectType->buildingProduction.input2.rate;

            // nur verbrauchen, was auch da is
            if (input2Consumed > building->productionSlots.input2.inventory) {
                input2Consumed = building->productionSlots.input2.inventory;
            }
            ticksInput2Consumed =
                (unsigned int) (input2Consumed * (double) TICKS_PER_MINUTE / mapObjectType->buildingProduction.input2.rate);
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
        inputConsumed = (double) ticksWeReallyProduced / (double) TICKS_PER_MINUTE * mapObjectType->buildingProduction.input.rate;
        building->productionSlots.input.decreaseInventory(inputConsumed);

        if (mapObjectType->buildingProduction.input2.isUsed()) {
            input2Consumed =
                (double) ticksWeReallyProduced / (double) TICKS_PER_MINUTE * mapObjectType->buildingProduction.input2.rate;
            building->productionSlots.input2.decreaseInventory(input2Consumed);
        }
    }
    outputProduced = (double) ticksWeReallyProduced / (double) TICKS_PER_MINUTE * mapObjectType->buildingProduction.output.rate;
    building->productionSlots.output.increaseInventory(outputProduced);
}
