#include "config/BuildingConfigMgr.h"
#include "economics/EconomicsMgr.h"
#include "game/Colony.h"
#include "game/Game.h"

// Aus main.cpp importiert
extern BuildingConfigMgr* buildingConfigMgr;
extern Game* game;


EconomicsMgr::EconomicsMgr() {
}

EconomicsMgr::~EconomicsMgr() {
}

void EconomicsMgr::update(Structure* structure) {
    // Erstmal nur Gebäude machen
    Building* building = dynamic_cast<Building*>(structure);
    if (building == nullptr) {
        return;
    }

    StructureType structureType = building->getStructureType();
    const BuildingConfig* buildingConfig = buildingConfigMgr->getConfig(structureType);

    // Produktion durchführen
    Uint32 ticksNow = SDL_GetTicks();
    Uint32 ticksPastSinceLastUpdate = ticksNow - building->getLastUpdateTime();

    // TODO Produktion abhängig machen von Input-Waren und deren Verfügbarkeit
    double goodsProduced = (double)ticksPastSinceLastUpdate / 60000 * buildingConfig->productionRate;
    building->productionSlots.output.increaseInventory(goodsProduced);

    building->setLastUpdateTime(ticksNow);
}
