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

Colony* Game::foundNewColony(Player* player, Isle* isle) {
    Colony* colony = new Colony();    
    
    colonies.insert(std::pair<std::pair<Player*, Isle*>, Colony*>(
        std::pair<Player*, Isle*>(player, isle), colony));
    
    return colony;
}

Colony* Game::getColony(Player* player, Isle* isle) {
    auto iter = colonies.find(std::pair<Player*, Isle*>(player, isle));
    if (iter == colonies.end()) {
        return nullptr;
    }
    
    return iter->second;
}

Colony* Game::getColony(const MapObject* mapObject) {
    int mapX, mapY;
    mapObject->getMapCoords(mapX, mapY);
    MapTile* mapTile = map->getMapTileAt(mapX, mapY);

    return getColony(mapTile->player, mapTile->isle);
}

void Game::addStructure(int mapX, int mapY, StructureType structureType, Player* player) {
    const std::string graphicSetName = context->graphicsMgr->getGraphicSetNameForStructure(structureType);
    const GraphicSet* graphicSet = context->graphicsMgr->getGraphicSet(graphicSetName);
    const IGraphic* graphic = graphicSet->getStatic()->getGraphic();

    // Objekt anlegen
    Structure* structure = (structureType >= START_BUILDINGS) ? new Building() : new Structure();
    structure->setMapCoords(mapX, mapY, graphic->getMapWidth(), graphic->getMapHeight());
    structure->setStructureType(structureType);
    structure->setPlayer(player);

    // Building? Defaults für Produktionsdaten setzen
    Building* building = dynamic_cast<Building*>(structure);
    const BuildingConfig* buildingConfig = nullptr;
    if (building != nullptr) {
        buildingConfig = context->configMgr->getBuildingConfig(structureType);
        building->productionSlots = ProductionSlots(buildingConfig->buildingProduction);
    }

    // Objekt in die Liste aufnehmen.
    map->addMapObject(structure);

    // Kontor oder Marktplatz? Einzugbereich in mapTiles aktualisieren und Lagerkapazität der Kolonie erhöhen
    if (building != nullptr && (structureType == OFFICE1 || structureType == MARKETPLACE)) {
        map->addOfficeCatchmentAreaToMap(*building);
#ifndef NO_SDL
        context->guiMgr->onOfficeCatchmentAreaChanged();
#endif
    }

    Colony* colony = context->game->getColony(structure); // Colony kann erst gefunden werden, wenn addOfficeCatchmentAreaToMap() aufgerufen wurde
    if (structureType == OFFICE1 || structureType == MARKETPLACE) {
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
}

void Game::addInhabitantsToBuilding(Building* building, char amount) {
    if ((int) building->inhabitants + amount < 0) {
        throw new std::runtime_error("Cannot have negative inhabitants");
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
    GameIO::loadGameFromTMX(this, filename);

#ifndef NO_SDL
    context->guiMgr->onNewGame();
#endif
}
