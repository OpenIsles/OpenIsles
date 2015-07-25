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

#ifdef DEBUG
    fpsCounterEnabled = true;
#endif
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

Colony* Game::foundNewColony(const Player* player, const std::string& colonyName, const Isle* isle) {
    const std::unordered_map<std::string, Good>& allGoods = context->configMgr->getAllGoods();
    Colony* colony = new Colony(allGoods);
    colony->name = colonyName;
    
    colonies.insert(std::pair<std::pair<const Player*, const Isle*>, Colony*>(
        std::pair<const Player*, const Isle*>(player, isle), colony));
    
    return colony;
}

Colony* Game::getColony(const Player* player, const Isle* isle) const {
    auto iter = colonies.find(std::pair<const Player*, const Isle*>(player, isle));
    if (iter == colonies.end()) {
        return nullptr;
    }
    
    return iter->second;
}

Colony* Game::getColony(const MapObjectFixed* mapObject) const {
    const MapTile* mapTile = map->getMapTileAt(mapObject->getMapCoords());

    return getColony(mapTile->player, mapTile->isle);
}

Colony* Game::getColony(const MapCoords& mapCoords) const {
    const MapTile* mapTile = map->getMapTileAt(mapCoords);
    if (mapTile == nullptr) {
        return nullptr;
    }

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
    const MapCoords& mapCoords, const MapObjectType* mapObjectType, const FourthDirection& view, Player* player) {

    // Harvestable
    if (mapObjectType->type == MapObjectTypeClass::HARVESTABLE) {
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
    const MapCoords& mapCoords, const MapObjectType* mapObjectType, double age, const FourthDirection& view) {

    assert(mapObjectType->type == MapObjectTypeClass::HARVESTABLE);

    unsigned char maxAge = mapObjectType->maxAge;

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
    const MapCoords& mapCoords, const MapObjectType* mapObjectType, const FourthDirection& view, Player* player) {

    assert(mapObjectType->type == MapObjectTypeClass::STRUCTURE || mapObjectType->type == MapObjectTypeClass::BUILDING);

    unsigned char mapWidth, mapHeight;
    if (view == Direction::NORTH || view == Direction::SOUTH) {
        mapWidth = mapObjectType->mapWidth;
        mapHeight = mapObjectType->mapHeight;
    } else {
        mapWidth = mapObjectType->mapHeight;
        mapHeight = mapObjectType->mapWidth;
    }

    // Objekt anlegen
    Structure* structure = (Structure*) MapObjectFixed::instantiate(mapObjectType);
    structure->setMapCoords(mapCoords);
    structure->setMapWidth(mapWidth);
    structure->setMapHeight(mapHeight);
    structure->setMapObjectType(mapObjectType);
    structure->setPlayer(player);
    structure->setView(view);

    // Building? Defaults für Produktionsdaten setzen
    Building* building = dynamic_cast<Building*>(structure);
    if (building != nullptr) {
        building->productionSlots = ProductionSlots(mapObjectType->buildingProduction);
    }

    // Objekt in die Liste aufnehmen.
    map->addMapObject(structure);

    // Kontor oder Marktplatz? Einzugbereich in mapTiles aktualisieren und Lagerkapazität der Kolonie erhöhen
    // TODO Flag an die Config, dass das Gebäude zum baubaren Einzugsbereich zählt
    if (building != nullptr &&
        (mapObjectType->name == "office1" || mapObjectType->name == "office2" || mapObjectType->name == "marketplace")) {

        map->addOfficeCatchmentAreaToMap(*building);
#ifndef NO_SDL
        context->guiMgr->onOfficeCatchmentAreaChanged();
#endif
    }

    Colony* colony = context->game->getColony(structure); // Colony kann erst gefunden werden, wenn addOfficeCatchmentAreaToMap() aufgerufen wurde
    // TODO Lagerkapazitäterhöhung über Config lösen
    if (mapObjectType->name == "office1") {
        colony->increaseGoodsCapacity(30);
    } else if (mapObjectType->name == "office2") {
        colony->increaseGoodsCapacity(50);
    } else if (mapObjectType->name == "marketplace") {
        colony->increaseGoodsCapacity(10);
    }

    // Einwohner setzen
    if (building != nullptr) {
        if (building->isHouse()) {
            // TODO Start-Einwohnerzahl setzen
        } else {
            context->game->addInhabitantsToBuilding(building, mapObjectType->inhabitants);
        }
    }

    return structure;
}

Street* Game::addStreet(const MapCoords& mapCoords, const MapObjectType* mapObjectType, const FourthDirection& view,
                        Player* player, const StreetConnections& streetConnections) {

    assert(mapObjectType->type == MapObjectTypeClass::STREET);
    assert((mapObjectType->mapWidth == 1) && (mapObjectType->mapHeight == 1)); // Wir gehen davon aus, dass es nur 1x1-Straßen gibt

    // Objekt anlegen
    Street* street = new Street();
    street->setMapCoords(mapCoords);
    street->setMapWidth(1);
    street->setMapHeight(1);
    street->setMapObjectType(mapObjectType);
    street->setPlayer(player);
    street->setView(view);
    street->streetConnections = streetConnections;

    // Objekt in die Liste aufnehmen.
    map->addMapObject(street);

    return street;
}

void Game::addInhabitantsToBuilding(Building* building, char amount) {
    if ((int) building->inhabitants + amount < 0) {
        throw std::runtime_error("Cannot have negative inhabitants");
    }

    building->inhabitants += amount;

    Colony* colony = getColony(building);
    colony->population += amount;
}

void Game::setSelectedMapObject(const MapObject* selectedMapObject) {
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
