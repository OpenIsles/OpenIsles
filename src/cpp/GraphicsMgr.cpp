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
    
	structures[CHAPEL] = new Graphic("data/img/objects/chapel.png", 2, 2);
	structures[WEAPONSMITH] = new Graphic("data/img/objects/weaponsmith.png", 2, 2);
	structures[SIGNALFIRE] = new Graphic("data/img/objects/signalfire.png", 1, 1);
	structures[HERBARY] = new Graphic("data/img/objects/herbary.png", 3, 3);
	structures[BRICKYARD] = new Graphic("data/img/objects/brickyard.png", 4, 2);
	structures[BRICKYARD2] = new Graphic("data/img/objects/brickyard2.png", 2, 4);
    structures[OFFICE] = new Graphic("data/img/objects/office.png", 3, 3);
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
    
    
    otherGraphics = new Graphic*[OtherGraphic::MAX_GRAPHIC];
    memset(otherGraphics, 0, OtherGraphic::MAX_GRAPHIC * sizeof(Graphic*));
    
    otherGraphics[OtherGraphic::COINS] = new Graphic("data/img/coin.png");
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
	}
	delete[] goodsIcons;
    
    for (int i = 0; i < OtherGraphic::COINS; i++) {
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
