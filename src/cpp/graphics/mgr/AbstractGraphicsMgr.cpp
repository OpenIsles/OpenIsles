#include <cstring>
#include "graphics/mgr/AbstractGraphicsMgr.h"


AbstractGraphicsMgr::~AbstractGraphicsMgr() {
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

void AbstractGraphicsMgr::loadGraphics() {
    loadTiles();

    // anisotropic filtering für die Gebäude aktivieren, damit beim Skalieren das Mask-Overlay ordentlich is
    renderer->setHintRenderScaleQuality("2");

	structures = new IMapObjectGraphic*[256];
    memset(structures, 0, 256 * sizeof(IMapObjectGraphic*));

	structures[StructureType::CHAPEL] = loadMapObjectGraphic("data/img/objects/chapel.png", 2, 1);
	structures[StructureType::PIONEERS_HOUSE1] = loadMapObjectGraphic("data/img/objects/pioneers-house1.png", 2, 2);
    structures[StructureType::PIONEERS_HOUSE2] = loadMapObjectGraphic("data/img/objects/pioneers-house2.png", 2, 2);
    structures[StructureType::PIONEERS_HOUSE3] = loadMapObjectGraphic("data/img/objects/pioneers-house3.png", 2, 2);
    structures[StructureType::PIONEERS_HOUSE4] = loadMapObjectGraphic("data/img/objects/pioneers-house4.png", 2, 2);
    structures[StructureType::PIONEERS_HOUSE5] = loadMapObjectGraphic("data/img/objects/pioneers-house5.png", 2, 2);
	structures[StructureType::STONEMASON] = loadMapObjectGraphic("data/img/objects/stonemason.png", 2, 2);
    structures[StructureType::OFFICE1] = loadMapObjectGraphic("data/img/objects/office1.png", 3, 2);
    structures[StructureType::MARKETPLACE] = loadMapObjectGraphic("data/img/objects/marketplace.png", 4, 3);
    structures[StructureType::FORESTERS] = loadMapObjectGraphic("data/img/objects/foresters.png", 2, 2);
    structures[StructureType::SHEEP_FARM] = loadMapObjectGraphic("data/img/objects/sheep-farm.png", 2, 2);
    structures[StructureType::WEAVING_MILL1] = loadMapObjectGraphic("data/img/objects/weaving-mill1.png", 2, 2);
    structures[StructureType::CATTLE_FARM] = loadMapObjectGraphic("data/img/objects/cattle-farm.png", 2, 2);
    structures[StructureType::BUTCHERS] = loadMapObjectGraphic("data/img/objects/butchers.png", 2, 2);
    structures[StructureType::TOOLSMITHS] = loadMapObjectGraphic("data/img/objects/toolsmiths.png", 2, 2);
    structures[StructureType::HUNTERS_HUT] = loadMapObjectGraphic("data/img/objects/hunters-hut.png", 1, 1);

    structures[StructureType::STREET] = loadMapObjectGraphic("data/img/objects/street-straight90.png", 1, 1); // damit was drinsteht. Fürs Malen werden die nachfolgenden Grafiken verwendet
	structures[StructureType::STREET_STRAIGHT_0] = loadMapObjectGraphic("data/img/objects/street-straight0.png", 1, 1);
	structures[StructureType::STREET_STRAIGHT_90] = loadMapObjectGraphic("data/img/objects/street-straight90.png", 1, 1);
	structures[StructureType::STREET_CURVE_0] = loadMapObjectGraphic("data/img/objects/street-curve0.png", 1, 1);
	structures[StructureType::STREET_CURVE_90] = loadMapObjectGraphic("data/img/objects/street-curve90.png", 1, 1);
	structures[StructureType::STREET_CURVE_180] = loadMapObjectGraphic("data/img/objects/street-curve180.png", 1, 1);
	structures[StructureType::STREET_CURVE_270] = loadMapObjectGraphic("data/img/objects/street-curve270.png", 1, 1);
    structures[StructureType::STREET_TEE_0] = loadMapObjectGraphic("data/img/objects/street-tee0.png", 1, 1);
    structures[StructureType::STREET_TEE_90] = loadMapObjectGraphic("data/img/objects/street-tee90.png", 1, 1);
    structures[StructureType::STREET_TEE_180] = loadMapObjectGraphic("data/img/objects/street-tee180.png", 1, 1);
    structures[StructureType::STREET_TEE_270] = loadMapObjectGraphic("data/img/objects/street-tee270.png", 1, 1);
    structures[StructureType::STREET_CROSS] = loadMapObjectGraphic("data/img/objects/street-cross.png", 1, 1);


    goodsIcons = new IPlainGraphic*[GoodsType::MAX_GOOD];
    memset(goodsIcons, 0, GoodsType::MAX_GOOD * sizeof(IPlainGraphic*));

    goodsIcons[GoodsType::TOOLS] = loadPlainGraphic("data/img/goods/icon/tools.png");
    goodsIcons[GoodsType::WOOD] = loadPlainGraphic("data/img/goods/icon/wood.png");
    goodsIcons[GoodsType::BRICKS] = loadPlainGraphic("data/img/goods/icon/bricks.png");

    goodsMarketplaceIcons = new IPlainGraphic*[GoodsType::MAX_GOOD];
    memset(goodsMarketplaceIcons, 0, GoodsType::MAX_GOOD * sizeof(IPlainGraphic*));

    goodsMarketplaceIcons[GoodsType::WOOL] = loadPlainGraphic("data/img/goods/marketplace-icon/wool.png");
    goodsMarketplaceIcons[GoodsType::CATTLE] = loadPlainGraphic("data/img/goods/marketplace-icon/cattle.png");
    goodsMarketplaceIcons[GoodsType::FOOD] = loadPlainGraphic("data/img/goods/marketplace-icon/food.png");
    goodsMarketplaceIcons[GoodsType::CLOTH] = loadPlainGraphic("data/img/goods/marketplace-icon/cloth.png");
    goodsMarketplaceIcons[GoodsType::TOOLS] = loadPlainGraphic("data/img/goods/marketplace-icon/tools.png");
    goodsMarketplaceIcons[GoodsType::WOOD] = loadPlainGraphic("data/img/goods/marketplace-icon/wood.png");
    goodsMarketplaceIcons[GoodsType::BRICKS] = loadPlainGraphic("data/img/goods/marketplace-icon/bricks.png");


    otherGraphics = new IPlainGraphic*[OtherGraphic::MAX_GRAPHIC];
    memset(otherGraphics, 0, OtherGraphic::MAX_GRAPHIC * sizeof(IPlainGraphic*));

    otherGraphics[OtherGraphic::COINS] = loadPlainGraphic("data/img/coin.png");
    otherGraphics[OtherGraphic::PANEL] = loadPlainGraphic("data/img/gui/panel.png");
    otherGraphics[OtherGraphic::STATUSBAR] = loadPlainGraphic("data/img/gui/statusbar.png");
    otherGraphics[OtherGraphic::ADD_BUILDING_GRID] = loadPlainGraphic("data/img/gui/add-building/add-building-grid.png");
    otherGraphics[OtherGraphic::ADD_BUILDING_CHAPEL] = loadPlainGraphic("data/img/gui/add-building/chapel.png");
    otherGraphics[OtherGraphic::ADD_BUILDING_MARKETPLACE] = loadPlainGraphic("data/img/gui/add-building/marketplace.png");
    otherGraphics[OtherGraphic::ADD_BUILDING_OFFICE1] = loadPlainGraphic("data/img/gui/add-building/office1.png");
    otherGraphics[OtherGraphic::ADD_BUILDING_STREET] = loadPlainGraphic("data/img/gui/add-building/street.png");
    otherGraphics[OtherGraphic::ADD_BUILDING_SHEEP_FARM] = loadPlainGraphic("data/img/gui/add-building/sheep-farm.png");
    otherGraphics[OtherGraphic::ADD_BUILDING_WEAVING_MILL1] = loadPlainGraphic("data/img/gui/add-building/weaving-mill1.png");
    otherGraphics[OtherGraphic::ADD_BUILDING_CATTLE_FARM] = loadPlainGraphic("data/img/gui/add-building/cattle-farm.png");
    otherGraphics[OtherGraphic::ADD_BUILDING_BUTCHERS] = loadPlainGraphic("data/img/gui/add-building/butchers.png");
    otherGraphics[OtherGraphic::ADD_BUILDING_TOOLSMITHS] = loadPlainGraphic("data/img/gui/add-building/toolsmiths.png");
    otherGraphics[OtherGraphic::ADD_BUILDING_HOUSE] = loadPlainGraphic("data/img/gui/add-building/house.png");
    otherGraphics[OtherGraphic::ADD_BUILDING_STONEMASON] = loadPlainGraphic("data/img/gui/add-building/stonemason.png");
    otherGraphics[OtherGraphic::ADD_BUILDING_FORESTERS] = loadPlainGraphic("data/img/gui/add-building/foresters.png");
    otherGraphics[OtherGraphic::ADD_BUILDING_HUNTERS_HUT] = loadPlainGraphic("data/img/gui/add-building/hunters-hut.png");
    otherGraphics[OtherGraphic::ADD_BUILDING_DUMMY] = loadPlainGraphic("data/img/gui/add-building/dummy.png");
    otherGraphics[OtherGraphic::ADD_BUILDING_GROUP_CRAFTSMAN] = loadPlainGraphic("data/img/gui/button-add-building-craftsman.png");
    otherGraphics[OtherGraphic::ADD_BUILDING_GROUP_CRAFTSMAN_PRESSED] = loadPlainGraphic("data/img/gui/button-add-building-craftsman-pressed.png");
    otherGraphics[OtherGraphic::ADD_BUILDING_GROUP_FARM] = loadPlainGraphic("data/img/gui/button-add-building-farm.png");
    otherGraphics[OtherGraphic::ADD_BUILDING_GROUP_FARM_PRESSED] = loadPlainGraphic("data/img/gui/button-add-building-farm-pressed.png");
    otherGraphics[OtherGraphic::ADD_BUILDING_GROUP_PORT] = loadPlainGraphic("data/img/gui/button-add-building-port.png");
    otherGraphics[OtherGraphic::ADD_BUILDING_GROUP_PORT_PRESSED] = loadPlainGraphic("data/img/gui/button-add-building-port-pressed.png");
    otherGraphics[OtherGraphic::ADD_BUILDING_GROUP_PUBLIC] = loadPlainGraphic("data/img/gui/button-add-building-public.png");
    otherGraphics[OtherGraphic::ADD_BUILDING_GROUP_PUBLIC_PRESSED] = loadPlainGraphic("data/img/gui/button-add-building-public-pressed.png");
    otherGraphics[OtherGraphic::PRODUCTION_ARROW] = loadPlainGraphic("data/img/gui/production-arrow.png");
    otherGraphics[OtherGraphic::PRODUCTION_PLUS] = loadPlainGraphic("data/img/gui/production-plus.png");
    otherGraphics[OtherGraphic::COAT_OF_ARMS_POPULATION_RED] = loadPlainGraphic("data/img/gui/coat-of-arms/population/red.png");
    otherGraphics[OtherGraphic::COAT_OF_ARMS_POPULATION_BLUE] = loadPlainGraphic("data/img/gui/coat-of-arms/population/blue.png");
    otherGraphics[OtherGraphic::COAT_OF_ARMS_POPULATION_YELLOW] = loadPlainGraphic("data/img/gui/coat-of-arms/population/yellow.png");
    otherGraphics[OtherGraphic::COAT_OF_ARMS_POPULATION_WHITE] = loadPlainGraphic("data/img/gui/coat-of-arms/population/white.png");
    otherGraphics[OtherGraphic::MUSIC] = loadPlainGraphic("data/img/gui/button-music.png");
    otherGraphics[OtherGraphic::MUSIC_PRESSED] = loadPlainGraphic("data/img/gui/button-music-pressed.png");


    animations = new IAnimation*[AnimationType::MAX_ANIMATION];
    memset(animations, 0, AnimationType::MAX_ANIMATION * sizeof(IAnimation*));

    animations[AnimationType::CARRIER] = loadAnimation("data/img/objects/carrier.png", 1, 1, 31, IAnimation::DEFAULT_FPS);
    animations[AnimationType::CART_WITHOUT_CARGO] = loadAnimation("data/img/objects/cart-without-cargo.png", 1, 1, 32, IAnimation::DEFAULT_FPS);
    animations[AnimationType::CART_WITH_CARGO] = loadAnimation("data/img/objects/cart-with-cargo.png", 1, 1, 32, IAnimation::DEFAULT_FPS);
}

void AbstractGraphicsMgr::loadTiles() {
    // nearest pixel sampling für die Kacheln, damit die fließend ineinander übergehen
    renderer->setHintRenderScaleQuality("0");

    tiles = new IMapObjectGraphic*[128];
    memset(tiles, 0, 128 * sizeof(IMapObjectGraphic*));

    for (unsigned char tileIndex = 0; tileIndex < 128; tileIndex++) {
        const MapTileConfig* mapTileConfig = configMgr->getMapTileConfig(tileIndex);
        if (mapTileConfig == nullptr) {
            continue;
        }

        tiles[tileIndex] = loadMapObjectGraphic(mapTileConfig->graphicsFile, 1, 1);
    }
}
