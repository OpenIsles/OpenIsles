#include <cassert>
#include <cstring>
#include "config/ConfigMgr.h"
#include "economics/EconomicsMgr.h"
#include "game/Colony.h"
#include "game/Game.h"
#include "game/GameIO.h"
#include "game/Player.h"
#include "gui/GuiMgr.h"
#include "map/Map.h"


Game::Game(const Context* const context) : ContextAware(context) {
    speed = 1;
    map = new Map(context);
}

Game::~Game() {
    for (auto iter = players.cbegin(); iter != players.cend(); iter++) {
		Player* player = *iter;
		delete player;
	}
	players.clear();
    
    for (auto iter = colonies.cbegin(); iter != colonies.cend(); iter++) {
		Colony* colony = iter->second;
		delete colony;
	}
	colonies.clear();

    delete map;
}

Colony* Game::foundNewColony(const Player* player, Isle* isle) {
    Colony* colony = new Colony();    
    
    colonies.insert(std::pair<std::pair<const Player*, Isle*>, Colony*>(
        std::pair<const Player*, Isle*>(player, isle), colony));
    
    return colony;
}

Colony* Game::getColony(const Player* player, Isle* isle) const {
    auto iter = colonies.find(std::pair<const Player*, Isle*>(player, isle));
    if (iter == colonies.end()) {
        return nullptr;
    }
    
    return iter->second;
}

Colony* Game::getColony(const MapObjectFixed* mapObject) const {
    MapTile* mapTile = map->getMapTileAt(mapObject->getMapCoords());

    return getColony(mapTile->player, mapTile->isle);
}

Harvestable* Game::addHarvestable(
    const MapCoords& mapCoords, MapObjectType mapObjectType, double age, const FourthDirection& view) {

    assert(mapObjectType < MapObjectType::START_STRUCTURES); // TODO addHarvestable() und addStructure() zusammenlegen?

    // Objekt anlegen
    Harvestable* harvestable = new Harvestable();
    harvestable->setMapCoords(mapCoords);
    harvestable->setMapObjectType(mapObjectType);
    harvestable->setView(view);
    harvestable->setAge(age);

    // Objekt in die Liste aufnehmen.
    map->addMapObject(harvestable);

    return harvestable;
}

Structure* Game::addStructure(
    const MapCoords& mapCoords, MapObjectType mapObjectType, const FourthDirection& view, Player* player) {

    assert(mapObjectType >= MapObjectType::START_STRUCTURES); // TODO addHarvestable() und addStructure() zusammenlegen?

    const std::string graphicSetName = context->graphicsMgr->getGraphicSetNameForMapObject(mapObjectType);
    const GraphicSet* graphicSet = context->graphicsMgr->getGraphicSet(graphicSetName);
    const IGraphic* graphic = graphicSet->getByView(view)->getGraphic();

    // Objekt anlegen
    Structure* structure = (mapObjectType >= START_BUILDINGS) ? new Building() : new Structure();
    structure->setMapCoords(mapCoords);
    structure->setMapWidth(graphic->getMapWidth());
    structure->setMapHeight(graphic->getMapHeight());
    structure->setMapObjectType(mapObjectType);
    structure->setPlayer(player);
    structure->setView(view);

    // Building? Defaults für Produktionsdaten setzen
    Building* building = dynamic_cast<Building*>(structure);
    const BuildingConfig* buildingConfig = nullptr;
    if (building != nullptr) {
        buildingConfig = context->configMgr->getBuildingConfig(mapObjectType);
        building->productionSlots = ProductionSlots(buildingConfig->buildingProduction);
    }

    // Objekt in die Liste aufnehmen.
    map->addMapObject(structure);

    // Kontor oder Marktplatz? Einzugbereich in mapTiles aktualisieren und Lagerkapazität der Kolonie erhöhen
    if (building != nullptr && (mapObjectType == OFFICE1 || mapObjectType == MARKETPLACE)) {
        map->addOfficeCatchmentAreaToMap(*building);
#ifndef NO_SDL
        context->guiMgr->onOfficeCatchmentAreaChanged();
#endif
    }

    Colony* colony = context->game->getColony(structure); // Colony kann erst gefunden werden, wenn addOfficeCatchmentAreaToMap() aufgerufen wurde
    if (mapObjectType == OFFICE1 || mapObjectType == MARKETPLACE) {
        colony->increaseGoodsCapacity(10);
    }

    // Einwohner setzen
    if (building != nullptr) {
        if (building->isHouse()) {
            // TODO Start-Einwohnerzahl setzen
        } else {
            context->game->addInhabitantsToBuilding(building, buildingConfig->inhabitants);
        }
    }

    return structure;
}

void Game::addInhabitantsToBuilding(Building* building, char amount) {
    if ((int) building->inhabitants + amount < 0) {
        throw std::runtime_error("Cannot have negative inhabitants");
    }

    building->inhabitants += amount;

    Colony* colony = getColony(building);
    colony->population += amount;
}

void Game::setSelectedMapObject(MapObject* selectedMapObject) {
    map->setSelectedMapObject(selectedMapObject);

#ifndef NO_SDL
    context->guiMgr->onSelectedMapObjectChanged(selectedMapObject);
#endif
}

void Game::loadGameFromTMX(const char* filename) {
    GameIO::loadGameFromTMX(this, context->configMgr, context->graphicsMgr, filename);

#ifndef NO_SDL
    context->guiMgr->onNewGame();
#endif
}
