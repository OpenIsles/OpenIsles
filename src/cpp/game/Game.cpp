#include "config/BuildingConfigMgr.h"
#include "game/Colony.h"
#include "game/Game.h"
#include "gui/FontMgr.h"

static SDL_Color colorWhite = {255, 255, 255, 255};
static SDL_Color colorBlack = {0, 0, 0, 255};

// Aus main.cpp importiert
extern BuildingConfigMgr* buildingConfigMgr;
extern FontMgr* fontMgr;
extern Game* game;
extern GraphicsMgr* graphicsMgr;
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

void Game::renderResourcesBar() {
    Player* currentPlayer = game->getCurrentPlayer();
    StructureType addingStructure = game->getAddingStructure();
    const BuildingCosts* buildingCosts = (addingStructure != StructureType::NO_STRUCTURE) ? 
        buildingConfigMgr->getConfig(addingStructure)->getBuildingCosts() : nullptr;
    
    // Münzenguthaben
    graphicsMgr->getOtherGraphic(OtherGraphic::COINS)->drawAt(15, 8);
    
    std::string outputString = std::to_string(currentPlayer->coins);
    if (buildingCosts != nullptr) {
        outputString += " ("; 
        outputString += std::to_string(buildingCosts->coins);
        outputString += ")";
    }
    fontMgr->renderText(renderer, outputString, 42, 10,
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
        
        
        graphicsMgr->getGraphicForGoodsIcon(goodsType)->drawAt(x, 5);
        
        int goodsInventory = colony->getGoods(goodsType).inventory;
        std::string outputString = std::to_string(goodsInventory);
        if (buildingCosts != nullptr) {
            outputString += " ("; 
            outputString += std::to_string(
                (goodsType == GoodsType::TOOLS) ? buildingCosts->tools :
                (goodsType == GoodsType::WOOD) ? buildingCosts->wood :
                (goodsType == GoodsType::BRICKS) ? buildingCosts->bricks : 0);
            outputString += ")";
        }
        
        fontMgr->renderText(renderer, outputString, x + 35, 10,
                            &colorWhite, &colorBlack, "DroidSans-Bold.ttf", 18, RENDERTEXT_HALIGN_LEFT);
    }
}