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

	loadStaticGraphicSetWith4Views("structures/chapel", "data/img/objects/chapel.png", 2, 1);
	loadStaticGraphicSetWith4Views("structures/pioneers-house1", "data/img/objects/pioneers-house1.png", 2, 2);
    loadStaticGraphicSetWith4Views("structures/pioneers-house2", "data/img/objects/pioneers-house2.png", 2, 2);
    loadStaticGraphicSetWith4Views("structures/pioneers-house3", "data/img/objects/pioneers-house3.png", 2, 2);
    loadStaticGraphicSetWith4Views("structures/pioneers-house4", "data/img/objects/pioneers-house4.png", 2, 2);
    loadStaticGraphicSetWith4Views("structures/pioneers-house5", "data/img/objects/pioneers-house5.png", 2, 2);
	loadStaticGraphicSetWith4Views("structures/stonemason", "data/img/objects/stonemason.png", 2, 2);
    loadStaticGraphicSetWith4Views("structures/office1", "data/img/objects/office1.png", 3, 2);
    loadStaticGraphicSetWith4Views("structures/marketplace", "data/img/objects/marketplace.png", 4, 3);
    loadStaticGraphicSetWith4Views("structures/foresters", "data/img/objects/foresters.png", 2, 2);
    loadStaticGraphicSetWith4Views("structures/sheep-farm", "data/img/objects/sheep-farm.png", 2, 2);
    loadStaticGraphicSetWith4Views("structures/weaving-mill1", "data/img/objects/weaving-mill1.png", 2, 2);
    loadStaticGraphicSetWith4Views("structures/cattle-farm", "data/img/objects/cattle-farm.png", 2, 2);
    loadStaticGraphicSetWith4Views("structures/butchers", "data/img/objects/butchers.png", 2, 2);
    loadStaticGraphicSetWith4Views("structures/toolsmiths", "data/img/objects/toolsmiths.png", 2, 2);
    loadStaticGraphicSetWith4Views("structures/hunters-hut", "data/img/objects/hunters-hut.png", 1, 1);

    loadStreets();

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

    GraphicSet* graphicSet = new GraphicSet();
    graphicSet->addStatic(new Animation(sdlGraphic));

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

void AbstractGraphicsMgr::loadStreets() {
    IGraphic* streetsGraphic = loadGraphic("data/img/objects/streets.png");

    const unsigned char streetTilesCount = 11;
    const char** streetGraphicSetNames = new const char*[streetTilesCount] {
        "structures/street-straight0",
        "structures/street-straight90",
        "structures/street-curve0",
        "structures/street-curve90",
        "structures/street-curve180",
        "structures/street-curve270",
        "structures/street-tee0",
        "structures/street-tee90",
        "structures/street-tee180",
        "structures/street-tee270",
        "structures/street-cross"
    };
    Rect tileRect(0, 0, TILE_WIDTH, TILE_HEIGHT);
    for (int i = 0; i < streetTilesCount; i++, tileRect.x += TILE_WIDTH) {
        IGraphic* sdlFrameGraphic = loadGraphic(*streetsGraphic, tileRect, 1, 1);
        IGraphic* sdlFrameGraphic2 = loadGraphic(*streetsGraphic, tileRect, 1, 1); // TODO passende Grafik nehmen
        IGraphic* sdlFrameGraphic3 = loadGraphic(*streetsGraphic, tileRect, 1, 1); // TODO passende Grafik nehmen
        IGraphic* sdlFrameGraphic4 = loadGraphic(*streetsGraphic, tileRect, 1, 1); // TODO passende Grafik nehmen

        GraphicSet* graphicSet = new GraphicSet();
        graphicSet->addByView("south", new Animation(sdlFrameGraphic));
        graphicSet->addByView("east", new Animation(sdlFrameGraphic2)); // TODO passende Grafik nehmen
        graphicSet->addByView("north", new Animation(sdlFrameGraphic3)); // TODO passende Grafik nehmen
        graphicSet->addByView("west", new Animation(sdlFrameGraphic4)); // TODO passende Grafik nehmen

        graphicSets[streetGraphicSetNames[i]] = graphicSet;
    }

    delete streetsGraphic;
}

void AbstractGraphicsMgr::loadStaticGraphicSetWith4Views(
    const std::string& graphicSetName, const char* graphicFilename, unsigned char mapWidth, unsigned char mapHeight) {

    IGraphic* graphic = loadGraphic(graphicFilename);
    int tileWidth = graphic->getWidth() / 4;

    GraphicSet* graphicSet = new GraphicSet();
    IGraphic* tileGraphic;

    Rect tileRect(0, 0, tileWidth, graphic->getHeight());
    tileGraphic = loadGraphic(*graphic, tileRect, mapWidth, mapHeight);
    graphicSet->addByView("south", new Animation(tileGraphic));

    tileRect.x += tileWidth;
    tileGraphic = loadGraphic(*graphic, tileRect, mapHeight, mapWidth);
    graphicSet->addByView("east", new Animation(tileGraphic));

    tileRect.x += tileWidth;
    tileGraphic = loadGraphic(*graphic, tileRect, mapWidth, mapHeight);
    graphicSet->addByView("north", new Animation(tileGraphic));

    tileRect.x += tileWidth;
    tileGraphic = loadGraphic(*graphic, tileRect, mapHeight, mapWidth);
    graphicSet->addByView("west", new Animation(tileGraphic));

    graphicSets[graphicSetName] = graphicSet;
    delete graphic;
}
