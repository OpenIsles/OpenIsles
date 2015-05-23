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
    const std::unordered_map<std::string, Good>& allGoods = context->configMgr->getAllGoods();
    Colony* colony = new Colony(allGoods);
    
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

double Game::getSecondsSinceLastUpdate(const MapObject* mapObject) const {
    unsigned int ticksPastSinceLastUpdate;

    // Sicherung, falls ein minimal-negativer Wert entsteht: dann nehmen wir 0
    if (context->sdlTicks > mapObject->getLastUpdateTime()) {
        ticksPastSinceLastUpdate = context->sdlTicks - mapObject->getLastUpdateTime();
    } else {
        ticksPastSinceLastUpdate = 0;
    }
    const double oneSecondTicks = (double) 1000 / context->game->getSpeed();

    return (double) ticksPastSinceLastUpdate / oneSecondTicks;
}

MapObjectFixed* Game::addMapObjectFixed(
    const MapCoords& mapCoords, MapObjectType mapObjectType, const FourthDirection& view, Player* player) {

    // Harvestable
    if (mapObjectType < MapObjectType::START_STRUCTURES) {
        // ein bisschen Zufall für das Startalter, damit die Felder nicht alle gleichzeitig wachsen
        std::random_device randomDevice;
        std::default_random_engine randomEngine(randomDevice());
        std::uniform_real_distribution<double> randomInitAge(0.0, 0.25);
        double initAge = randomInitAge(randomEngine);

        return addHarvestable(mapCoords, mapObjectType, initAge, view);
    }

    // Structure/Building
    return addStructure(mapCoords, mapObjectType, view, player);
}

Harvestable* Game::addHarvestable(
    const MapCoords& mapCoords, MapObjectType mapObjectType, double age, const FourthDirection& view) {

    assert(mapObjectType < MapObjectType::START_STRUCTURES);

    const MapObjectConfig* mapObjectConfig = context->configMgr->getMapObjectConfig(mapObjectType);
    unsigned char maxAge = mapObjectConfig->maxAge;

    // Objekt anlegen
    Harvestable* harvestable = new Harvestable(maxAge);
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

    assert(mapObjectType >= MapObjectType::START_STRUCTURES);

    const MapObjectConfig* mapObjectConfig = context->configMgr->getMapObjectConfig(mapObjectType);
    unsigned char mapWidth, mapHeight;
    if (view == Direction::NORTH || view == Direction::SOUTH) {
        mapWidth = mapObjectConfig->mapWidth;
        mapHeight = mapObjectConfig->mapHeight;
    } else {
        mapWidth = mapObjectConfig->mapHeight;
        mapHeight = mapObjectConfig->mapWidth;
    }

    // Objekt anlegen
    Structure* structure = (mapObjectType >= START_BUILDINGS) ? new Building() : new Structure();
    structure->setMapCoords(mapCoords);
    structure->setMapWidth(mapWidth);
    structure->setMapHeight(mapHeight);
    structure->setMapObjectType(mapObjectType);
    structure->setPlayer(player);
    structure->setView(view);

    // Building? Defaults für Produktionsdaten setzen
    Building* building = dynamic_cast<Building*>(structure);
    if (building != nullptr) {
        building->productionSlots = ProductionSlots(mapObjectConfig->buildingProduction);
    }

    // Objekt in die Liste aufnehmen.
    map->addMapObject(structure);

    // Kontor oder Marktplatz? Einzugbereich in mapTiles aktualisieren und Lagerkapazität der Kolonie erhöhen
    if (building != nullptr && (mapObjectType == OFFICE1 || mapObjectType == OFFICE2 || mapObjectType == MARKETPLACE)) {
        map->addOfficeCatchmentAreaToMap(*building);
#ifndef NO_SDL
        context->guiMgr->onOfficeCatchmentAreaChanged();
#endif
    }

    Colony* colony = context->game->getColony(structure); // Colony kann erst gefunden werden, wenn addOfficeCatchmentAreaToMap() aufgerufen wurde
    // TODO Lagerkapazitäterhöhung über Config lösen
    if (mapObjectType == OFFICE1) {
        colony->increaseGoodsCapacity(30);
    } else if (mapObjectType == OFFICE2) {
        colony->increaseGoodsCapacity(50);
    } else if (mapObjectType == MARKETPLACE) {
        colony->increaseGoodsCapacity(10);
    }

    // Einwohner setzen
    if (building != nullptr) {
        if (building->isHouse()) {
            // TODO Start-Einwohnerzahl setzen
        } else {
            context->game->addInhabitantsToBuilding(building, mapObjectConfig->inhabitants);
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
