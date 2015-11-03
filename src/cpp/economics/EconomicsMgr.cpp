#include "config/ConfigMgr.h"
#include "economics/EconomicsMgr.h"
#include "game/Game.h"


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

    // Input-Güter reichen?
    auto areAllInputGoodsHere = [&]() {
        if (mapObjectType->buildingProduction.input.isUsed()) {
            if (building->productionSlots.input.inventory < mapObjectType->inputAmountForProduction) {
                return false;
            }

            if (mapObjectType->buildingProduction.input2.isUsed()) {
                if (building->productionSlots.input2.inventory < mapObjectType->input2AmountForProduction) {
                    return false;
                }
            }
        }
        return true;
    };

    if (!areAllInputGoodsHere()) {
        return;
    }

    // Waren die Waren grade frisch angeliefert?
    auto setNextGoodsProductionTicks = [&]() {
        building->hasRecievedAlInputGoodsSinceLastUpdate = true;
        building->nextGoodsProductionTicks = context->game->getTicks() +
            (unsigned long) (mapObjectType->secondsToProduce * TICKS_PER_SECOND);
    };

    if (!building->hasRecievedAlInputGoodsSinceLastUpdate) {
        // Ja. Dann beginnt ab jetzt die Produktionszeit.
        setNextGoodsProductionTicks();
        return;
    }

    // Prüfen, ob es Zeit ist, was zu produzieren
    if (context->game->getTicks() < building->nextGoodsProductionTicks) {
        return;
    }

    // Ja? Ok, dann produzieren
    if (mapObjectType->buildingProduction.input.isUsed()) {
        building->productionSlots.input.decreaseInventory(mapObjectType->inputAmountForProduction);

        if (mapObjectType->buildingProduction.input2.isUsed()) {
            building->productionSlots.input2.decreaseInventory(mapObjectType->input2AmountForProduction);
        }
    }

    building->productionSlots.output.increaseInventory(1);

    // Sind noch genug Input-Güter da? Dann sofort das Helper-Flag setzen, damit sofort wieder die Uhr tickt
    if (areAllInputGoodsHere()) {
        setNextGoodsProductionTicks();
    } else {
        building->hasRecievedAlInputGoodsSinceLastUpdate = false;
    }
}
