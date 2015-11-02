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

    // TODO Verzögerung berücksichtigen, wenn Input-Güter grade erst reingekommen sind. 
    // Es darf danach nicht sofort Output da sein, sondern erst dann beginnt die Produktionszeit

    // Prüfen, ob es Zeit ist, was zu produzieren
    unsigned long ticksNextProduction = building->lastGoodsProduction +
        (unsigned long) (mapObjectType->secondsToProduce * TICKS_PER_SECOND);

    if (context->game->getTicks() < ticksNextProduction) {
        return;
    }

    // Input-Güter reichen?
    if (mapObjectType->buildingProduction.input.isUsed()) {
        if (building->productionSlots.input.inventory < mapObjectType->inputAmountForProduction) {
            return;
        }

        if (mapObjectType->buildingProduction.input2.isUsed()) {
            if (building->productionSlots.input2.inventory < mapObjectType->input2AmountForProduction) {
                return;
            }
        }
    }

    // Ja? Ok, dann produzieren
    if (mapObjectType->buildingProduction.input.isUsed()) {
        building->productionSlots.input.decreaseInventory(mapObjectType->inputAmountForProduction);

        if (mapObjectType->buildingProduction.input2.isUsed()) {
            building->productionSlots.input2.decreaseInventory(mapObjectType->input2AmountForProduction);
        }
    }

    building->productionSlots.output.increaseInventory(1);
    building->lastGoodsProduction = context->game->getTicks();
}
