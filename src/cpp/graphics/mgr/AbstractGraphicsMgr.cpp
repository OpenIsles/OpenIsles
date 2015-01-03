#include <cstring>
#include <iostream>
#include "graphics/graphic/Animation.h"
#include "graphics/graphic/GraphicSet.h"
#include "graphics/mgr/AbstractGraphicsMgr.h"


AbstractGraphicsMgr::~AbstractGraphicsMgr() {
    for (auto iter = graphicSets.cbegin(); iter != graphicSets.cend(); iter++) {
        const GraphicSet* graphicSet = iter->second;
        delete graphicSet;
    }

    graphicSets.clear();
}

void AbstractGraphicsMgr::loadGraphics() {
    loadTiles();

    // anisotropic filtering für die Gebäude aktivieren, damit beim Skalieren das Mask-Overlay ordentlich is
    renderer->setHintRenderScaleQuality("2");

	loadStaticGraphicSet("structures/chapel", "data/img/objects/chapel.png", 2, 1);
	loadStaticGraphicSet("structures/pioneers-house1", "data/img/objects/pioneers-house1.png", 2, 2);
    loadStaticGraphicSet("structures/pioneers-house2", "data/img/objects/pioneers-house2.png", 2, 2);
    loadStaticGraphicSet("structures/pioneers-house3", "data/img/objects/pioneers-house3.png", 2, 2);
    loadStaticGraphicSet("structures/pioneers-house4", "data/img/objects/pioneers-house4.png", 2, 2);
    loadStaticGraphicSet("structures/pioneers-house5", "data/img/objects/pioneers-house5.png", 2, 2);
	loadStaticGraphicSet("structures/stonemason", "data/img/objects/stonemason.png", 2, 2);
    loadStaticGraphicSet("structures/office1", "data/img/objects/office1.png", 3, 2);
    loadStaticGraphicSet("structures/marketplace", "data/img/objects/marketplace.png", 4, 3);
    loadStaticGraphicSet("structures/foresters", "data/img/objects/foresters.png", 2, 2);
    loadStaticGraphicSet("structures/sheep-farm", "data/img/objects/sheep-farm.png", 2, 2);
    loadStaticGraphicSet("structures/weaving-mill1", "data/img/objects/weaving-mill1.png", 2, 2);
    loadStaticGraphicSet("structures/cattle-farm", "data/img/objects/cattle-farm.png", 2, 2);
    loadStaticGraphicSet("structures/butchers", "data/img/objects/butchers.png", 2, 2);
    loadStaticGraphicSet("structures/toolsmiths", "data/img/objects/toolsmiths.png", 2, 2);
    loadStaticGraphicSet("structures/hunters-hut", "data/img/objects/hunters-hut.png", 1, 1);

    loadStaticGraphicSet("structures/street", "data/img/objects/street-straight90.png", 1, 1); // damit was drinsteht. Fürs Malen werden die nachfolgenden Grafiken verwendet
	loadStaticGraphicSet("structures/street-straight0", "data/img/objects/street-straight0.png", 1, 1);
	loadStaticGraphicSet("structures/street-straight90", "data/img/objects/street-straight90.png", 1, 1);
	loadStaticGraphicSet("structures/street-curve0", "data/img/objects/street-curve0.png", 1, 1);
	loadStaticGraphicSet("structures/street-curve90", "data/img/objects/street-curve90.png", 1, 1);
	loadStaticGraphicSet("structures/street-curve180", "data/img/objects/street-curve180.png", 1, 1);
	loadStaticGraphicSet("structures/street-curve270", "data/img/objects/street-curve270.png", 1, 1);
    loadStaticGraphicSet("structures/street-tee0", "data/img/objects/street-tee0.png", 1, 1);
    loadStaticGraphicSet("structures/street-tee90", "data/img/objects/street-tee90.png", 1, 1);
    loadStaticGraphicSet("structures/street-tee180", "data/img/objects/street-tee180.png", 1, 1);
    loadStaticGraphicSet("structures/street-tee270", "data/img/objects/street-tee270.png", 1, 1);
    loadStaticGraphicSet("structures/street-cross", "data/img/objects/street-cross.png", 1, 1);

    loadStaticGraphicSet("goods-icons/tools", "data/img/goods/icon/tools.png");
    loadStaticGraphicSet("goods-icons/wood", "data/img/goods/icon/wood.png");
    loadStaticGraphicSet("goods-icons/bricks", "data/img/goods/icon/bricks.png");

    loadStaticGraphicSet("goods-marketplace-icons/wool", "data/img/goods/marketplace-icon/wool.png");
    loadStaticGraphicSet("goods-marketplace-icons/cattle", "data/img/goods/marketplace-icon/cattle.png");
    loadStaticGraphicSet("goods-marketplace-icons/food", "data/img/goods/marketplace-icon/food.png");
    loadStaticGraphicSet("goods-marketplace-icons/cloth", "data/img/goods/marketplace-icon/cloth.png");
    loadStaticGraphicSet("goods-marketplace-icons/tools", "data/img/goods/marketplace-icon/tools.png");
    loadStaticGraphicSet("goods-marketplace-icons/wood", "data/img/goods/marketplace-icon/wood.png");
    loadStaticGraphicSet("goods-marketplace-icons/bricks", "data/img/goods/marketplace-icon/bricks.png");

    loadStaticGraphicSet("coin", "data/img/coin.png");
    loadStaticGraphicSet("panel", "data/img/gui/panel.png");
    loadStaticGraphicSet("statusbar", "data/img/gui/statusbar.png");

    loadStaticGraphicSet("add-building-grid", "data/img/gui/add-building/add-building-grid.png");

    loadStaticGraphicSet("add-building-button/chapel", "data/img/gui/add-building/chapel.png");
    loadStaticGraphicSet("add-building-button/marketplace", "data/img/gui/add-building/marketplace.png");
    loadStaticGraphicSet("add-building-button/office1", "data/img/gui/add-building/office1.png");
    loadStaticGraphicSet("add-building-button/street", "data/img/gui/add-building/street.png");
    loadStaticGraphicSet("add-building-button/sheep-farm", "data/img/gui/add-building/sheep-farm.png");
    loadStaticGraphicSet("add-building-button/weaving-mill1", "data/img/gui/add-building/weaving-mill1.png");
    loadStaticGraphicSet("add-building-button/cattle-farm", "data/img/gui/add-building/cattle-farm.png");
    loadStaticGraphicSet("add-building-button/butchers", "data/img/gui/add-building/butchers.png");
    loadStaticGraphicSet("add-building-button/toolsmiths", "data/img/gui/add-building/toolsmiths.png");
    loadStaticGraphicSet("add-building-button/house", "data/img/gui/add-building/house.png");
    loadStaticGraphicSet("add-building-button/stonemason", "data/img/gui/add-building/stonemason.png");
    loadStaticGraphicSet("add-building-button/foresters", "data/img/gui/add-building/foresters.png");
    loadStaticGraphicSet("add-building-button/hunters-hut", "data/img/gui/add-building/hunters-hut.png");
    loadStaticGraphicSet("add-building-button/dummy", "data/img/gui/add-building/dummy.png");

    loadStaticGraphicSet("add-building-group/craftsman", "data/img/gui/button-add-building-craftsman.png");
    loadStaticGraphicSet("add-building-group/craftsman-pressed", "data/img/gui/button-add-building-craftsman-pressed.png");
    loadStaticGraphicSet("add-building-group/farm", "data/img/gui/button-add-building-farm.png");
    loadStaticGraphicSet("add-building-group/farm-pressed", "data/img/gui/button-add-building-farm-pressed.png");
    loadStaticGraphicSet("add-building-group/port", "data/img/gui/button-add-building-port.png");
    loadStaticGraphicSet("add-building-group/port-pressed", "data/img/gui/button-add-building-port-pressed.png");
    loadStaticGraphicSet("add-building-group/public", "data/img/gui/button-add-building-public.png");
    loadStaticGraphicSet("add-building-group/public-pressed", "data/img/gui/button-add-building-public-pressed.png");

    loadStaticGraphicSet("production-arrow", "data/img/gui/production-arrow.png");
    loadStaticGraphicSet("production-plus", "data/img/gui/production-plus.png");

    loadStaticGraphicSet("coat-of-arms/population/red", "data/img/gui/coat-of-arms/population/red.png");
    loadStaticGraphicSet("coat-of-arms/population/blue", "data/img/gui/coat-of-arms/population/blue.png");
    loadStaticGraphicSet("coat-of-arms/population/yellow", "data/img/gui/coat-of-arms/population/yellow.png");
    loadStaticGraphicSet("coat-of-arms/population/white", "data/img/gui/coat-of-arms/population/white.png");

    loadStaticGraphicSet("button-music", "data/img/gui/button-music.png");
    loadStaticGraphicSet("button-music-pressed", "data/img/gui/button-music-pressed.png");

    loadStaticAnimationGraphicSet("carrier", "data/img/objects/carrier.png", 1, 1, 31);
    loadStaticAnimationGraphicSet("cart-without-cargo", "data/img/objects/cart-without-cargo.png", 1, 1, 32);
    loadStaticAnimationGraphicSet("cart-with-cargo", "data/img/objects/cart-with-cargo.png", 1, 1, 32);
}

void AbstractGraphicsMgr::loadTiles() {
    // nearest pixel sampling für die Kacheln, damit die fließend ineinander übergehen
    renderer->setHintRenderScaleQuality("0");

    for (unsigned char tileIndex = 0; tileIndex < 128; tileIndex++) {
        const MapTileConfig* mapTileConfig = configMgr->getMapTileConfig(tileIndex);
        if (mapTileConfig == nullptr) {
            continue;
        }

        std::string graphicName = getGraphicSetNameForTile(tileIndex);
        loadStaticGraphicSet(graphicName, mapTileConfig->graphicsFile, 1, 1);
    }
}

void AbstractGraphicsMgr::loadStaticGraphicSet(const std::string& graphicSetName, const char* graphicFilename) {
    loadStaticGraphicSet(graphicSetName, graphicFilename, 0, 0);
}

void AbstractGraphicsMgr::loadStaticGraphicSet(
    const std::string& graphicSetName, const char* graphicFilename, unsigned char mapWidth, unsigned char mapHeight) {

    IGraphic* sdlGraphic = loadGraphic(graphicFilename, mapWidth, mapHeight);

    Animation* animation = new Animation(1);
    animation->addFrame(0, sdlGraphic);

    GraphicSet* graphicSet = new GraphicSet();
    graphicSet->addStatic(animation);

    graphicSets[graphicSetName] = graphicSet;
}

void AbstractGraphicsMgr::loadStaticAnimationGraphicSet(
    const std::string& graphicSetName, const char* graphicFilename, unsigned char mapWidth, unsigned char mapHeight,
    unsigned int countFrames) {

    IGraphic* sdlFullGraphic = loadGraphic(graphicFilename, mapWidth, mapHeight);

    int fullGraphicWidth = sdlFullGraphic->getWidth();
    if (fullGraphicWidth % countFrames != 0) {
        std::cerr << "Could not divide the frames equally: '" << graphicFilename << "': " << std::endl;
        throw new std::runtime_error("Could not divide the frames equally");
    }
    int frameWidth = fullGraphicWidth / countFrames;


    Animation* animation = new Animation(countFrames);
    Rect frameRect(0, 0, frameWidth, sdlFullGraphic->getHeight());
    for (int x = 0, frameIndex = 0; x < fullGraphicWidth; x += frameWidth, frameIndex++) {
        frameRect.x = x;

        IGraphic* sdlFrameGraphic = loadGraphic(*sdlFullGraphic, frameRect, mapWidth, mapHeight);
        animation->addFrame(frameIndex, sdlFrameGraphic);
    }

    GraphicSet* graphicSet = new GraphicSet();
    graphicSet->addStatic(animation);

    graphicSets[graphicSetName] = graphicSet;
}