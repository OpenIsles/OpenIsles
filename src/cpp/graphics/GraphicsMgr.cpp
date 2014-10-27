#include <fstream>
#include <sstream>
#include <stdexcept>
#include "game/Colony.h"

GraphicsMgr::GraphicsMgr() {
	loadTiles();

    // anisotropic filtering für die Gebäude aktivieren, damit beim Skalieren das Mask-Overlay ordentlich is
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");

	structures = new MapObjectGraphic*[256];
    memset(structures, 0, 256 * sizeof(MapObjectGraphic*));
    
	structures[StructureType::CHAPEL] = new MapObjectGraphic("data/img/objects/chapel.png", 2, 1);
	structures[StructureType::PIONEERS_HOUSE1] = new MapObjectGraphic("data/img/objects/pioneers-house1.png", 2, 2);
	structures[StructureType::SIGNALFIRE] = new MapObjectGraphic("data/img/objects/signalfire.png", 1, 1);
	structures[StructureType::HERBARY] = new MapObjectGraphic("data/img/objects/herbary.png", 3, 3);
	structures[StructureType::BRICKYARD] = new MapObjectGraphic("data/img/objects/brickyard.png", 4, 2);
	structures[StructureType::BRICKYARD2] = new MapObjectGraphic("data/img/objects/brickyard2.png", 2, 4);
    structures[StructureType::OFFICE1] = new MapObjectGraphic("data/img/objects/office1.png", 3, 2);
    structures[StructureType::MARKETPLACE] = new MapObjectGraphic("data/img/objects/marketplace.png", 4, 3);
    structures[StructureType::FORESTERS] = new MapObjectGraphic("data/img/objects/foresters.png", 2, 2);
    structures[StructureType::SHEEP_FARM] = new MapObjectGraphic("data/img/objects/sheep-farm.png", 2, 2);
    structures[StructureType::WEAVING_MILL1] = new MapObjectGraphic("data/img/objects/weaving-mill1.png", 2, 2);
    structures[StructureType::CATTLE_FARM] = new MapObjectGraphic("data/img/objects/cattle-farm.png", 2, 2);
    structures[StructureType::BUTCHERS] = new MapObjectGraphic("data/img/objects/butchers.png", 2, 2);
    structures[StructureType::TOOLSMITHS] = new MapObjectGraphic("data/img/objects/toolsmiths.png", 2, 2);

    structures[StructureType::STREET] = new MapObjectGraphic("data/img/objects/street-straight90.png", 1, 1); // damit was drinsteht. Fürs Malen werden die nachfolgenden Grafiken verwendet
	structures[StructureType::STREET_STRAIGHT_0] = new MapObjectGraphic("data/img/objects/street-straight0.png", 1, 1);
	structures[StructureType::STREET_STRAIGHT_90] = new MapObjectGraphic("data/img/objects/street-straight90.png", 1, 1);
	structures[StructureType::STREET_CURVE_0] = new MapObjectGraphic("data/img/objects/street-curve0.png", 1, 1);
	structures[StructureType::STREET_CURVE_90] = new MapObjectGraphic("data/img/objects/street-curve90.png", 1, 1);
	structures[StructureType::STREET_CURVE_180] = new MapObjectGraphic("data/img/objects/street-curve180.png", 1, 1);
	structures[StructureType::STREET_CURVE_270] = new MapObjectGraphic("data/img/objects/street-curve270.png", 1, 1);
    structures[StructureType::STREET_TEE_0] = new MapObjectGraphic("data/img/objects/street-tee0.png", 1, 1);
    structures[StructureType::STREET_TEE_90] = new MapObjectGraphic("data/img/objects/street-tee90.png", 1, 1);
    structures[StructureType::STREET_TEE_180] = new MapObjectGraphic("data/img/objects/street-tee180.png", 1, 1);
    structures[StructureType::STREET_TEE_270] = new MapObjectGraphic("data/img/objects/street-tee270.png", 1, 1);
    structures[StructureType::STREET_CROSS] = new MapObjectGraphic("data/img/objects/street-cross.png", 1, 1);

    
    goodsIcons = new PlainGraphic*[GoodsType::MAX_GOOD];
    memset(goodsIcons, 0, GoodsType::MAX_GOOD * sizeof(PlainGraphic*));
    
    goodsIcons[GoodsType::TOOLS] = new PlainGraphic("data/img/goods/icon/tools.png");
    goodsIcons[GoodsType::WOOD] = new PlainGraphic("data/img/goods/icon/wood.png");
    goodsIcons[GoodsType::BRICKS] = new PlainGraphic("data/img/goods/icon/bricks.png");

    goodsMarketplaceIcons = new PlainGraphic*[GoodsType::MAX_GOOD];
    memset(goodsMarketplaceIcons, 0, GoodsType::MAX_GOOD * sizeof(PlainGraphic*));

    goodsMarketplaceIcons[GoodsType::WOOL] = new PlainGraphic("data/img/goods/marketplace-icon/wool.png");
    goodsMarketplaceIcons[GoodsType::CATTLE] = new PlainGraphic("data/img/goods/marketplace-icon/cattle.png");
    goodsMarketplaceIcons[GoodsType::FOOD] = new PlainGraphic("data/img/goods/marketplace-icon/food.png");
    goodsMarketplaceIcons[GoodsType::CLOTH] = new PlainGraphic("data/img/goods/marketplace-icon/cloth.png");
    goodsMarketplaceIcons[GoodsType::TOOLS] = new PlainGraphic("data/img/goods/marketplace-icon/tools.png");
    goodsMarketplaceIcons[GoodsType::WOOD] = new PlainGraphic("data/img/goods/marketplace-icon/wood.png");
    goodsMarketplaceIcons[GoodsType::BRICKS] = new PlainGraphic("data/img/goods/marketplace-icon/bricks.png");


    otherGraphics = new PlainGraphic*[OtherGraphic::MAX_GRAPHIC];
    memset(otherGraphics, 0, OtherGraphic::MAX_GRAPHIC * sizeof(PlainGraphic*));
    
    otherGraphics[OtherGraphic::COINS] = new PlainGraphic("data/img/coin.png");
    otherGraphics[OtherGraphic::PANEL] = new PlainGraphic("data/img/gui/panel.png");
    otherGraphics[OtherGraphic::STATUSBAR] = new PlainGraphic("data/img/gui/statusbar.png");
    otherGraphics[OtherGraphic::ADD_BUILDING_GRID] = new PlainGraphic("data/img/gui/add-building/add-building-grid.png");
    otherGraphics[OtherGraphic::ADD_BUILDING_CHAPEL] = new PlainGraphic("data/img/gui/add-building/chapel.png");
    otherGraphics[OtherGraphic::ADD_BUILDING_MARKETPLACE] = new PlainGraphic("data/img/gui/add-building/marketplace.png");
    otherGraphics[OtherGraphic::ADD_BUILDING_OFFICE1] = new PlainGraphic("data/img/gui/add-building/office1.png");
    otherGraphics[OtherGraphic::ADD_BUILDING_STREET] = new PlainGraphic("data/img/gui/add-building/street.png");
    otherGraphics[OtherGraphic::ADD_BUILDING_SHEEP_FARM] = new PlainGraphic("data/img/gui/add-building/sheep-farm.png");
    otherGraphics[OtherGraphic::ADD_BUILDING_WEAVING_MILL1] = new PlainGraphic("data/img/gui/add-building/weaving-mill1.png");
    otherGraphics[OtherGraphic::ADD_BUILDING_CATTLE_FARM] = new PlainGraphic("data/img/gui/add-building/cattle-farm.png");
    otherGraphics[OtherGraphic::ADD_BUILDING_BUTCHERS] = new PlainGraphic("data/img/gui/add-building/butchers.png");
    otherGraphics[OtherGraphic::ADD_BUILDING_TOOLSMITHS] = new PlainGraphic("data/img/gui/add-building/toolsmiths.png");
    otherGraphics[OtherGraphic::ADD_BUILDING_DUMMY] = new PlainGraphic("data/img/gui/add-building/dummy.png");
    otherGraphics[OtherGraphic::PRODUCTION_ARROW] = new PlainGraphic("data/img/gui/production-arrow.png");
    otherGraphics[OtherGraphic::PRODUCTION_PLUS] = new PlainGraphic("data/img/gui/production-plus.png");


    animations = new Animation*[AnimationType::MAX_ANIMATION];
    memset(animations, 0, AnimationType::MAX_ANIMATION * sizeof(Animation*));

    animations[AnimationType::CARRIER] = new Animation("data/img/objects/carrier.png", 1, 1, 31, Animation::DEFAULT_FPS);
    animations[AnimationType::CART_WITHOUT_CARGO] = new Animation("data/img/objects/cart-without-cargo.png", 1, 1, 32, Animation::DEFAULT_FPS);
    animations[AnimationType::CART_WITH_CARGO] = new Animation("data/img/objects/cart-with-cargo.png", 1, 1, 32, Animation::DEFAULT_FPS);
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
    delete[] goodsMarketplaceIcons;
    
    for (int i = 0; i < OtherGraphic::MAX_GRAPHIC; i++) {
        if (otherGraphics[i] != nullptr) {
			delete otherGraphics[i];
		}
	}
	delete[] otherGraphics;

    for (int i = 0; i < AnimationType::MAX_ANIMATION; i++) {
        if (animations[i] != nullptr) {
            delete animations[i];
        }
    }
    delete[] animations;
}

void GraphicsMgr::loadTiles() {
    // nearest pixel sampling für die Kacheln, damit die fließend ineinander übergehen
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

	tiles = new MapObjectGraphic*[128];
    memset(tiles, 0, 128 * sizeof(MapObjectGraphic*));

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
		tiles[tileIndex] = new MapObjectGraphic(tileFilepath.data(), 1, 1);
	}
}
