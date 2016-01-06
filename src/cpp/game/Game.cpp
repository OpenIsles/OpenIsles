#include <cassert>
#include <cstring>
#include "global.h"
#include "config/ConfigMgr.h"
#include "economics/EconomicsMgr.h"
#include "game/Colony.h"
#include "game/Game.h"
#include "game/GameIO.h"
#include "game/Player.h"
#include "gui/GuiMgr.h"
#include "map/Map.h"
#include "utils/RandomEngine.h"


Game::Game(const Context& context) : ContextAware(context) {
    speed = 1;
    ticks = 0;
    map = new Map(context);
    nextCycleTicks = TICKS_PER_CYCLE;

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
    const ConfigMgr* configMgr = context.configMgr;
    const std::unordered_map<std::string, Good>& allGoods = configMgr->getAllGoods();
    const std::set<PopulationTier>& allPopulationTiers = configMgr->getAllPopulationTiers();

    Colony* colony = new Colony(allGoods, allPopulationTiers);
    colony->name = colonyName;
    
    colonies.insert(std::pair<std::pair<const Player*, const Isle*>, Colony*>(
        std::pair<const Player*, const Isle*>(player, isle), colony));
    
    return colony;
}

Colony* Game::getColony(const Player* player, const Isle* isle) {
    auto iter = colonies.find(std::pair<const Player*, const Isle*>(player, isle));
    if (iter == colonies.end()) {
        return nullptr;
    }
    
    return iter->second;
}

Colony* Game::getColony(const MapCoords& mapCoords) {
    const MapTile* mapTile = map->getMapTileAt(mapCoords);
    if (mapTile == nullptr) {
        return nullptr;
    }

    return getColony(mapTile->player, mapTile->isle);
}

Harvestable* Game::addHarvestable(
    const MapCoords& mapCoords, const MapObjectType* mapObjectType, const FourthDirection& view) {

    assert(mapObjectType->type == MapObjectTypeClass::HARVESTABLE);

    // ein bisschen Zufall für das Startalter, damit die Felder nicht alle gleichzeitig wachsen
    std::uniform_real_distribution<double> randomInitAge(0.0, 0.25);
    double initAge = randomInitAge(*context.randomEngine);

    // Objekt anlegen
    Harvestable* harvestable = (Harvestable*) instantiateNewMapObject(mapObjectType);
    harvestable->setMapCoords(mapCoords);
    harvestable->setView(view);
    harvestable->setColony(getColony(mapCoords));
    harvestable->setAge(initAge);

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
    Structure* structure = (Structure*) instantiateNewMapObject(mapObjectType);
    structure->setMapCoords(mapCoords);
    structure->setMapWidth(mapWidth);
    structure->setMapHeight(mapHeight);
    structure->setPlayer(player);
    structure->setView(view);

    // Building?
    Building* building = dynamic_cast<Building*>(structure);
    if (building != nullptr) {
        // Defaults für Produktionsdaten setzen
        building->productionSlots = ProductionSlots(mapObjectType->buildingProduction);

        // Initialen Delay bis zum ersten Träger einstellen
        building->nextCarrierMinTicks = ticks + Building::CARRIER_INITIAL_DELAY;
    }

    // Objekt in die Liste aufnehmen.
    map->addMapObject(structure);

    // Kontor oder Marktplatz? Einzugbereich in mapTiles aktualisieren und Lagerkapazität der Kolonie erhöhen
    // TODO Flag an die Config, dass das Gebäude zum baubaren Einzugsbereich zählt
    if (building != nullptr &&
        (mapObjectType->name == "office1" || mapObjectType->name == "office2" || mapObjectType->name == "marketplace")) {

        map->addOfficeCatchmentAreaToMap(*building);
#ifndef NO_SDL
        context.guiMgr->onOfficeCatchmentAreaChanged();
#endif
    }

    // Colony kann erst gefunden werden, wenn addOfficeCatchmentAreaToMap() aufgerufen wurde
    Colony* colony = getColony(mapCoords);
    structure->setColony(colony);

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
            // Häuser beginnen immer mit genau einem Einwohner
            context.game->addInhabitantsToBuilding(building, 1);
        } else {
            context.game->addInhabitantsToBuilding(building, mapObjectType->inhabitants);
        }
    }

    return structure;
}

Street* Game::addStreet(const MapCoords& mapCoords, const MapObjectType* mapObjectType, const FourthDirection& view,
                        Player* player, const StreetConnections& streetConnections) {

    assert(mapObjectType->type == MapObjectTypeClass::STREET);
    assert((mapObjectType->mapWidth == 1) && (mapObjectType->mapHeight == 1)); // Wir gehen davon aus, dass es nur 1x1-Straßen gibt

    // Objekt anlegen
    Street* street = (Street*) instantiateNewMapObject(mapObjectType);
    street->setMapCoords(mapCoords);
    street->setMapWidth(1);
    street->setMapHeight(1);
    street->setPlayer(player);
    street->setView(view);
    street->setColony(getColony(mapCoords));
    street->streetConnections = streetConnections;

    // Objekt in die Liste aufnehmen.
    map->addMapObject(street);

    return street;
}

MapObject* Game::instantiateNewMapObject(const MapObjectType* mapObjectType) const {
    MapObject* mapObject = MapObject::instantiate(mapObjectType);
    mapObject->setLastUpdateTicks(ticks);
    return mapObject;
}

void Game::addInhabitantsToBuilding(Building* building, char amount) {
    if ((int) building->inhabitants + amount < 0) {
        Log::error(_("Cannot have negative inhabitants."));
        throw std::runtime_error("Cannot have negative inhabitants");
    }

    building->inhabitants += amount;

    Colony* colony = building->getColony();
    colony->population += amount;

    // Bei Häusern die Bevölkerungsgruppe anpassen
    if (building->isHouse()) {
        const PopulationTier* populationTier = building->getMapObjectType()->populationTier;
        colony->populationTiers[populationTier].population += amount;
    }

    context.economicsMgr->updatePlayerStatus();
}

void Game::setSelectedMapObject(const MapObject* selectedMapObject) {
    map->setSelectedMapObject(selectedMapObject);

#ifndef NO_SDL
    context.guiMgr->onSelectedMapObjectChanged(selectedMapObject);
#endif
}

void Game::loadGameFromTMX(const char* filename) {
    GameIO::loadGameFromTMX(this, context.configMgr, context.graphicsMgr, filename);

#ifndef NO_SDL
    context.guiMgr->onNewGame();
#endif
}

void Game::update(unsigned long millisecondsElapsed) {
    // Game-Ticks erhöhen
    ticks += millisecondsElapsed * speed;

    // Alle Map-Objekte updaten
    std::list<MapObject*> mapObjectsToDelete;
    const std::list<MapObject*>& mapObjects = map->getMapObjects();

    for (auto iter = mapObjects.rbegin(); iter != mapObjects.rend(); iter++) {
        MapObject* mapObject = *iter;

        bool dontDeleteMe = mapObject->update(context);
        if (!dontDeleteMe) {
            // Objekt zur Löschung vormerken. Wir dürfen nicht löschen, wenn wir noch iterieren
            mapObjectsToDelete.push_back(mapObject);
        }
    }

    for (auto iter = mapObjectsToDelete.rbegin(); iter != mapObjectsToDelete.rend(); iter++) {
        MapObject* mapObject = *iter;
        map->deleteMapObject(mapObject);
    }

    // bestimmte Spiellogiken finden einmal pro Zyklus statt
    if (ticks >= nextCycleTicks) {
        EconomicsMgr* economicsMgr = context.economicsMgr;

        // Finanzen
        economicsMgr->updateFinances();
        
        // Güterverbrauch und Bevölkerungszufriedenheit
        economicsMgr->doGoodsConsumptionAndUpdatePopulationSatisfaction();

        nextCycleTicks += TICKS_PER_CYCLE;
    }
}