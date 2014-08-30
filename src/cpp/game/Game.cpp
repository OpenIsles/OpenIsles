#include "map/Building.h"
#include "map/Map.h"
#include "game/Colony.h"
#include "game/Game.h"
#include "game/Player.h"
#include "gui/FontMgr.h"

static SDL_Color colorWhite = {255, 255, 255, 255};
static SDL_Color colorBlack = {0, 0, 0, 255};

// Aus main.cpp importiert
extern FontMgr* fontMgr;
extern Game* game;
extern GraphicsMgr* graphicsMgr;
extern Map* map;
extern SDL_Renderer* renderer;
extern int mouseCurrentMapX, mouseCurrentMapY;


Game::Game() {
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
}

Colony* Game::foundNewColony(Player* player, Isle* isle) {
    Colony* colony = new Colony();    
    
    colonies.insert(std::pair<std::pair<Player*, Isle*>, Colony*>(
        std::pair<Player*, Isle*>(player, isle), colony));
    
    return colony;
}

void Game::renderResourcesBar() {
    // Münzenguthaben
    Player* currentPlayer = game->getCurrentPlayer();
    
    graphicsMgr->getOtherGraphic(OtherGraphic::COINS)->drawAt(15, 8);
    fontMgr->renderText(renderer, std::to_string(currentPlayer->getCoins()), 42, 10,
                        &colorWhite, &colorBlack, "DroidSans-Bold.ttf", 18, RENDERTEXT_HALIGN_LEFT);
        
    
    // Kolonie, wo der Cursor grade is
    MapTile* mapTileAtCursor = map->getMapTileAt(mouseCurrentMapX, mouseCurrentMapY);
    if (mapTileAtCursor == nullptr || mapTileAtCursor->player != currentPlayer) {
        return;
    }
    
    auto iter = colonies.find(std::pair<Player*, Isle*>(mapTileAtCursor->player, mapTileAtCursor->isle));
    if (iter == colonies.end()) {
        return;
    }
    Colony* colony = iter->second;
    
    GoodsType goodsToDraw[] = { GoodsType::TOOLS, GoodsType::WOOD, GoodsType::BRICKS };
    int x = 290;
    for (unsigned int i = 0; i < sizeof(goodsToDraw); i++, x += 110) {
        GoodsType goodsType = goodsToDraw[i];
        
        int goodsInventory = colony->getGoodsInventory(goodsType);
        graphicsMgr->getGraphicForGoodsIcon(goodsType)->drawAt(x, 5);
        fontMgr->renderText(renderer, std::to_string(goodsInventory), x + 35, 10,
                            &colorWhite, &colorBlack, "DroidSans-Bold.ttf", 18, RENDERTEXT_HALIGN_LEFT);
    }
}