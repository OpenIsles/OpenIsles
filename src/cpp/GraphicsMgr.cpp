#include <fstream>
#include <sstream>
#include <stdexcept>
#include "game/Colony.h"
#include "map/Structure.h"
#include "GraphicsMgr.h"

GraphicsMgr::GraphicsMgr() {
	loadTiles();
    

	structures = new Graphic*[256];
    memset(structures, 0, 256 * sizeof(Graphic*));
    
	structures[CHAPEL] = new Graphic("data/img/objects/chapel.png", 2, 1);
	structures[PIONEERS_HOUSE1] = new Graphic("data/img/objects/pioneers-house1.png", 2, 2);
	structures[SIGNALFIRE] = new Graphic("data/img/objects/signalfire.png", 1, 1);
	structures[HERBARY] = new Graphic("data/img/objects/herbary.png", 3, 3);
	structures[BRICKYARD] = new Graphic("data/img/objects/brickyard.png", 4, 2);
	structures[BRICKYARD2] = new Graphic("data/img/objects/brickyard2.png", 2, 4);
    structures[OFFICE1] = new Graphic("data/img/objects/office1.png", 3, 2);
    structures[MARKETPLACE] = new Graphic("data/img/objects/marketplace.png", 4, 3);
    structures[FORESTERS] = new Graphic("data/img/objects/foresters.png", 2, 2);

	structures[WAY_E] = new Graphic("data/img/objects/way/way-e.png", 1, 1);
	structures[WAY_N] = new Graphic("data/img/objects/way/way-n.png", 1, 1);
	structures[WAY_S] = new Graphic("data/img/objects/way/way-s.png", 1, 1);
	structures[WAY_W] = new Graphic("data/img/objects/way/way-w.png", 1, 1);
	structures[WAY_SW_NE] = new Graphic("data/img/objects/way/way-sw-ne.png", 1, 1);
	structures[WAY_NW_SE] = new Graphic("data/img/objects/way/way-nw-se.png", 1, 1);
    
    
    goodsIcons = new Graphic*[GoodsType::MAX_GOOD];
    memset(goodsIcons, 0, GoodsType::MAX_GOOD * sizeof(Graphic*));
    
    goodsIcons[GoodsType::TOOLS] = new Graphic("data/img/goods/icon/tools.png");
    goodsIcons[GoodsType::WOOD] = new Graphic("data/img/goods/icon/wood.png");
    goodsIcons[GoodsType::BRICKS] = new Graphic("data/img/goods/icon/bricks.png");

    goodsMarketplaceIcons = new Graphic*[GoodsType::MAX_GOOD];
    memset(goodsMarketplaceIcons, 0, GoodsType::MAX_GOOD * sizeof(Graphic*));

    goodsMarketplaceIcons[GoodsType::TOOLS] = new Graphic("data/img/goods/marketplace-icon/tools.png");
    goodsMarketplaceIcons[GoodsType::WOOD] = new Graphic("data/img/goods/marketplace-icon/wood.png");
    goodsMarketplaceIcons[GoodsType::BRICKS] = new Graphic("data/img/goods/marketplace-icon/bricks.png");


    otherGraphics = new Graphic*[OtherGraphic::MAX_GRAPHIC];
    memset(otherGraphics, 0, OtherGraphic::MAX_GRAPHIC * sizeof(Graphic*));
    
    otherGraphics[OtherGraphic::COINS] = new Graphic("data/img/coin.png");
    otherGraphics[OtherGraphic::PANEL] = new Graphic("data/img/gui/panel.png");
    otherGraphics[OtherGraphic::STATUSBAR] = new Graphic("data/img/gui/statusbar.png");
    otherGraphics[OtherGraphic::ADD_BUILDING_GRID] = new Graphic("data/img/gui/add-building/add-building-grid.png");
    otherGraphics[OtherGraphic::ADD_BUILDING_CHAPEL] = new Graphic("data/img/gui/add-building/chapel.png");
    otherGraphics[OtherGraphic::ADD_BUILDING_MARKETPLACE] = new Graphic("data/img/gui/add-building/marketplace.png");
    otherGraphics[OtherGraphic::ADD_BUILDING_OFFICE1] = new Graphic("data/img/gui/add-building/office1.png");
    otherGraphics[OtherGraphic::ADD_BUILDING_DUMMY] = new Graphic("data/img/gui/add-building/dummy.png");
    otherGraphics[OtherGraphic::PRODUCTION_ARROW] = new Graphic("data/img/gui/production-arrow.png");
    otherGraphics[OtherGraphic::PRODUCTION_PLUS] = new Graphic("data/img/gui/production-plus.png");
}

GraphicsMgr::~GraphicsMgr() {
	for (int i = 0; i < 128; i++) {
		if (tiles[i] != nullptr) {
			delete tiles[i];
		}
	}
	delete[] tiles;

	for (int i = 0; i < 256; i++) {
        if (structures[i] != nullptr) {
			delete structures[i];
		}
	}
	delete[] structures;
    
    for (int i = 0; i < GoodsType::MAX_GOOD; i++) {
        if (goodsIcons[i] != nullptr) {
			delete goodsIcons[i];
		}
        if (goodsMarketplaceIcons[i] != nullptr) {
            delete goodsMarketplaceIcons[i];
        }
	}
	delete[] goodsIcons;
    
    for (int i = 0; i < OtherGraphic::MAX_GRAPHIC; i++) {
        if (otherGraphics[i] != nullptr) {
			delete otherGraphics[i];
		}
	}
	delete[] otherGraphics;
}

void GraphicsMgr::loadTiles() {
	tiles = new Graphic*[128];
    memset(tiles, 0, 128 * sizeof(Graphic*));

	std::ifstream inputFileStreamTilesTxt("data/img/tiles/tiles.txt");
	std::string line;
	int lineNr = 0;
	while (std::getline(inputFileStreamTilesTxt, line)) {
		lineNr++;

		// Leerzeilen und Kommentarzeilen überspringen
		if (line.empty() || line.data()[0] == '#')
			continue;

		std::istringstream inputStringStream(line);
		int tileIndex, xOffset, yOffset;
		std::string tileFilename;
		if (!(inputStringStream >> tileIndex >> xOffset >> yOffset >> tileFilename)) {
			throw new std::runtime_error("Error in tiles.txt line " + lineNr);
		}

		std::string tileFilepath("data/img/tiles/" + tileFilename);
		tiles[tileIndex] = new Graphic(tileFilepath.data(), 1, 1);
	}
}
