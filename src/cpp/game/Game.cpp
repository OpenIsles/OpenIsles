#include "config/ConfigMgr.h"
#include "game/Colony.h"
#include "game/Game.h"
#include "gui/FontMgr.h"
#include "gui/GuiMgr.h"
#include "utils/StringFormat.h"

// Aus main.cpp importiert
extern ConfigMgr* configMgr;
extern FontMgr* fontMgr;
extern Game* game;
extern GraphicsMgr* graphicsMgr;
extern GuiMgr* guiMgr;


Game::Game() {
    speed = 1;
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