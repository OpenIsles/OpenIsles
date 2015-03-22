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
    loadStaticGraphicSetWith4Views("structures/cathedral", "data/img/objects/cathedral.png", 6, 4);
    loadStaticGraphicSetWith4Views("structures/tavern", "data/img/objects/tavern.png", 2, 3);

    loadStreetTileset("cobbled-street");
    loadStreetTileset("farm-road");

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
    loadStaticGraphicSet("add-building-button/cobbled-street", "data/img/gui/add-building/cobbled-street.png");
    loadStaticGraphicSet("add-building-button/farm-road", "data/img/gui/add-building/farm-road.png");
    loadStaticGraphicSet("add-building-button/sheep-farm", "data/img/gui/add-building/sheep-farm.png");
    loadStaticGraphicSet("add-building-button/weaving-mill1", "data/img/gui/add-building/weaving-mill1.png");
    loadStaticGraphicSet("add-building-button/cattle-farm", "data/img/gui/add-building/cattle-farm.png");
    loadStaticGraphicSet("add-building-button/butchers", "data/img/gui/add-building/butchers.png");
    loadStaticGraphicSet("add-building-button/toolsmiths", "data/img/gui/add-building/toolsmiths.png");
    loadStaticGraphicSet("add-building-button/house", "data/img/gui/add-building/house.png");
    loadStaticGraphicSet("add-building-button/stonemason", "data/img/gui/add-building/stonemason.png");
    loadStaticGraphicSet("add-building-button/foresters", "data/img/gui/add-building/foresters.png");
    loadStaticGraphicSet("add-building-button/hunters-hut", "data/img/gui/add-building/hunters-hut.png");
    loadStaticGraphicSet("add-building-button/cathedral", "data/img/gui/add-building/cathedral.png");
    loadStaticGraphicSet("add-building-button/tavern", "data/img/gui/add-building/tavern.png");
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

    renderer->setHintRenderScaleQuality("0");
    loadStaticGraphicSet("plain-tile", "data/img/plain-tile.png");
    renderer->setHintRenderScaleQuality("2");

    loadStaticAnimationGraphicSetWith8Views("carrier", "data/img/objects/carrier.png", 1, 1, 31);
    loadStaticAnimationGraphicSetWith8Views("cart-without-cargo", "data/img/objects/cart-without-cargo.png", 1, 1, 32);
    loadStaticAnimationGraphicSetWith8Views("cart-with-cargo", "data/img/objects/cart-with-cargo.png", 1, 1, 32);
}

void AbstractGraphicsMgr::loadTiles() {
    // nearest pixel sampling für die Kacheln, damit die fließend ineinander übergehen
    renderer->setHintRenderScaleQuality("0");

    IGraphic* sdlFullGraphic = loadGraphic("data/img/tileset.png");

    for (auto mapEntry : configMgr->getMapTileConfigs()) {
        const MapTileConfig& mapTileConfig = mapEntry.second;

        GraphicSet* graphicSet = new GraphicSet();

        Rect tileRect(0, 0, TILE_WIDTH, TILE_HEIGHT + ELEVATION_HEIGHT);
        forEachFourthDirection(fourDirectionsView) {
            const std::pair<int, int>& xyOffsetInTileset =
                mapTileConfig.mapTileViewsOffsetXYInTileset.at(fourDirectionsView);

            tileRect.x = xyOffsetInTileset.first;
            tileRect.y = xyOffsetInTileset.second;
            IGraphic* sdlTileGraphic = loadGraphic(*sdlFullGraphic, tileRect, 1, 1);

            graphicSet->addByView(fourDirectionsView, new Animation(sdlTileGraphic));
        }

        std::string graphicSetName = "tiles/" + mapTileConfig.tileName;
        graphicSets[graphicSetName] = graphicSet;
    }

    delete sdlFullGraphic;
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

void AbstractGraphicsMgr::loadStaticAnimationGraphicSetWith8Views(
    const std::string& graphicSetName, const char* graphicFilename, unsigned char mapWidth, unsigned char mapHeight,
    unsigned int countFrames) {

    IGraphic* sdlFullGraphic = loadGraphic(graphicFilename, mapWidth, mapHeight);

    int fullGraphicWidth = sdlFullGraphic->getWidth();
    if (fullGraphicWidth % countFrames != 0) {
        std::cerr << "Could not divide the frames equally: '" << graphicFilename << "': " << std::endl;
        throw std::runtime_error("Could not divide the frames equally");
    }
    int frameWidth = fullGraphicWidth / countFrames;

    int fullGraphicHeight = sdlFullGraphic->getHeight();
    if (fullGraphicHeight % 8 != 0) {
        std::cerr << "Could not divide the views equally: '" << graphicFilename << "': " << std::endl;
        throw std::runtime_error("Could not divide the views equally");
    }
    int frameHeight = fullGraphicHeight / 8;

    GraphicSet* graphicSet = new GraphicSet();
    Rect frameRect(0, 0, frameWidth, frameHeight);

    forEachEighthDirection(view) {
        Animation* animation = new Animation(countFrames);
        for (int x = 0, frameIndex = 0; x < fullGraphicWidth; x += frameWidth, frameIndex++) {
            frameRect.x = x;

            IGraphic* sdlFrameGraphic = loadGraphic(
                *sdlFullGraphic, frameRect,
                (view % 2) ? mapHeight : mapWidth,
                (view % 2) ? mapWidth : mapHeight
            );
            animation->addFrame(frameIndex, sdlFrameGraphic);
        }

        graphicSet->addByView(view, animation);
        frameRect.y += frameHeight;
    }

    graphicSets[graphicSetName] = graphicSet;

    delete sdlFullGraphic;
}

void AbstractGraphicsMgr::loadStreetTileset(const std::string& streetTileset) {
    // nearest pixel sampling für die Straßen-Kacheln, damit die auch fließend ineinander übergehen, wenn rausgezoomt is
    renderer->setHintRenderScaleQuality("0");

    const std::string graphicFilename = "data/img/objects/" + streetTileset + ".png";
    IGraphic* streetsGraphic = loadGraphic(graphicFilename.c_str());

    static struct {
        const char* angleName;
        int tileOffsetPerView[4];
    } streetTiles[11]{
        { "straight0", { 0, 1, 0, 1 } },
        { "straight90", { 1, 0, 1, 0 } },
        { "curve0", { 2, 3, 4, 5 } },
        { "curve90", { 3, 4, 5, 2 } },
        { "curve180", { 4, 5, 2, 3 } },
        { "curve270", { 5, 2, 3, 4 } },
        { "tee0", { 6, 7, 8, 9 } },
        { "tee90", { 7, 8, 9, 6 } },
        { "tee180", { 8, 9, 6, 7 }} ,
        { "tee270", { 9, 6, 7, 8 } },
        { "cross", { 10, 10, 10, 10 } }
    };

    Rect tileRect(0, 0, TILE_WIDTH, TILE_HEIGHT);
    for (int i = 0; i < 11; i++) {
        GraphicSet* graphicSet = new GraphicSet();
        forEachFourthDirection(view) {
            tileRect.x = TILE_WIDTH * streetTiles[i].tileOffsetPerView[((unsigned char) view) / 2];
            IGraphic* sdlFrameGraphic = loadGraphic(*streetsGraphic, tileRect, 1, 1);
            graphicSet->addByView(view, new Animation(sdlFrameGraphic));
        }

        const std::string graphicSetName = "structures/" + streetTileset + "-" + streetTiles[i].angleName;
        graphicSets[graphicSetName] = graphicSet;
    }

    delete streetsGraphic;

    // wieder zurückstellen
    renderer->setHintRenderScaleQuality("2");
}

void AbstractGraphicsMgr::loadStaticGraphicSetWith4Views(
    const std::string& graphicSetName, const char* graphicFilename, unsigned char mapWidth, unsigned char mapHeight) {

    IGraphic* graphic = loadGraphic(graphicFilename);
    int tileWidth = graphic->getWidth() / 4;

    GraphicSet* graphicSet = new GraphicSet();
    Rect tileRect(0, 0, tileWidth, graphic->getHeight());

    forEachFourthDirection(view) {
        IGraphic* tileGraphic = loadGraphic(
            *graphic, tileRect,
            (view % 4) ? mapHeight : mapWidth,
            (view % 4) ? mapWidth : mapHeight
        );
        graphicSet->addByView(view, new Animation(tileGraphic));

        tileRect.x += tileWidth;
    }

    graphicSets[graphicSetName] = graphicSet;
    delete graphic;
}
