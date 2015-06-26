#include <cstring>
#include <iostream>
#include "graphics/graphic/Animation.h"
#include "graphics/graphic/GraphicSet.h"
#include "graphics/mgr/AbstractGraphicsMgr.h"
#include "utils/StringFormat.h"


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

	loadStaticGraphicSetWith4Views("building/chapel", "data/img/buildings/chapel.png");
	loadStaticGraphicSetWith4Views("building/pioneers-house1", "data/img/buildings/pioneers-house1.png");
    loadStaticGraphicSetWith4Views("building/pioneers-house2", "data/img/buildings/pioneers-house2.png");
    loadStaticGraphicSetWith4Views("building/pioneers-house3", "data/img/buildings/pioneers-house3.png");
    loadStaticGraphicSetWith4Views("building/pioneers-house4", "data/img/buildings/pioneers-house4.png");
    loadStaticGraphicSetWith4Views("building/pioneers-house5", "data/img/buildings/pioneers-house5.png");
    loadStaticGraphicSetWith4Views("building/settlers-house1", "data/img/buildings/settlers-house1.png");
    loadStaticGraphicSetWith4Views("building/settlers-house2", "data/img/buildings/settlers-house2.png");
    loadStaticGraphicSetWith4Views("building/settlers-house3", "data/img/buildings/settlers-house3.png");
    loadStaticGraphicSetWith4Views("building/settlers-house4", "data/img/buildings/settlers-house4.png");
    loadStaticGraphicSetWith4Views("building/settlers-house5", "data/img/buildings/settlers-house5.png");
	loadStaticGraphicSetWith4Views("building/stonemason", "data/img/buildings/stonemason.png");
    loadStaticGraphicSetWith4Views("building/office1", "data/img/buildings/office1.png");
    loadStaticGraphicSetWith4Views("building/office2", "data/img/buildings/office2.png");
    loadStaticGraphicSetWith4Views("building/marketplace", "data/img/buildings/marketplace.png");
    loadStaticGraphicSetWith4Views("building/foresters", "data/img/buildings/foresters.png");
    loadStaticGraphicSetWith4Views("building/sheep-farm", "data/img/buildings/sheep-farm.png");
    loadStaticGraphicSetWith4Views("building/weaving-mill1", "data/img/buildings/weaving-mill1.png");
    loadStaticGraphicSetWith4Views("building/cattle-farm", "data/img/buildings/cattle-farm.png");
    loadStaticGraphicSetWith4Views("building/butchers", "data/img/buildings/butchers.png");
    loadStaticGraphicSetWith4Views("building/toolsmiths", "data/img/buildings/toolsmiths.png");
    loadStaticGraphicSetWith4Views("building/hunters-hut", "data/img/buildings/hunters-hut.png");
    loadStaticGraphicSetWith4Views("building/cathedral", "data/img/buildings/cathedral.png");
    loadStaticGraphicSetWith4Views("building/tavern", "data/img/buildings/tavern.png");
    loadStaticGraphicSetWith4Views("building/sugarcane-plantation", "data/img/buildings/sugarcane-plantation.png");
    loadStaticGraphicSetWith4Views("building/tobacco-plantation", "data/img/buildings/tobacco-plantation.png");
    loadStaticGraphicSetWith4Views("building/distillery", "data/img/buildings/distillery.png");
    loadStaticGraphicSetWith4Views("building/tobacco-goods", "data/img/buildings/tobacco-goods.png");
    loadStaticGraphicSetWith4Views("building/spices-plantation", "data/img/buildings/spices-plantation.png");

    loadStreetTileset("cobbled-street");
    loadStreetTileset("farm-road");

    loadStaticGraphicSetWith4Views("structure/square1", "data/img/buildings/square1.png");
    loadStaticGraphicSetWith4Views("structure/square2", "data/img/buildings/square2.png");
    loadStaticGraphicSetWith4Views("structure/square3", "data/img/buildings/square3.png");

    const std::list<const Good*>& allGoods = configMgr->getAllGoodsOrdered();
    for (auto iter = allGoods.cbegin(); iter != allGoods.cend(); iter++) {
        const Good* good = *iter;

        loadStaticGraphicSet("goods-icon/" + good->name,
                             std::string("data/img/goods/icon/" + good->name + ".png").c_str());
        loadStaticGraphicSet("goods-marketplace-icon/" + good->name,
                             std::string("data/img/goods/marketplace-icon/" + good->name + ".png").c_str());
    }

    loadStaticGraphicSet("coin", "data/img/gui/coin.png");
    loadStaticGraphicSet("panel", "data/img/gui/panel.png");
    loadStaticGraphicSet("statusbar", "data/img/gui/statusbar.png");
    loadStaticGraphicSet("panel-header", "data/img/gui/panel-header.png");

    loadStaticGraphicSet("add-building-grid", "data/img/gui/add-building/add-building-grid.png");

    loadStaticGraphicSet("add-building-button/chapel", "data/img/gui/add-building/chapel.png");
    loadStaticGraphicSet("add-building-button/marketplace", "data/img/gui/add-building/marketplace.png");
    loadStaticGraphicSet("add-building-button/office1", "data/img/gui/add-building/office1.png");
    loadStaticGraphicSet("add-building-button/office2", "data/img/gui/add-building/office2.png");
    loadStaticGraphicSet("add-building-button/cobbled-street-straight0", "data/img/gui/add-building/cobbled-street.png"); // TODO "cobbled-street"
    loadStaticGraphicSet("add-building-button/farm-road-straight0", "data/img/gui/add-building/farm-road.png"); // TODO "farm-road"
    loadStaticGraphicSet("add-building-button/sheep-farm", "data/img/gui/add-building/sheep-farm.png");
    loadStaticGraphicSet("add-building-button/weaving-mill1", "data/img/gui/add-building/weaving-mill1.png");
    loadStaticGraphicSet("add-building-button/cattle-farm", "data/img/gui/add-building/cattle-farm.png");
    loadStaticGraphicSet("add-building-button/butchers", "data/img/gui/add-building/butchers.png");
    loadStaticGraphicSet("add-building-button/toolsmiths", "data/img/gui/add-building/toolsmiths.png");
    loadStaticGraphicSet("add-building-button/pioneers-house1", "data/img/gui/add-building/house.png"); // TODO "house"
    loadStaticGraphicSet("add-building-button/stonemason", "data/img/gui/add-building/stonemason.png");
    loadStaticGraphicSet("add-building-button/foresters", "data/img/gui/add-building/foresters.png");
    loadStaticGraphicSet("add-building-button/northern-forest1", "data/img/gui/add-building/forest.png"); // TODO "forest"
    loadStaticGraphicSet("add-building-button/hunters-hut", "data/img/gui/add-building/hunters-hut.png");
    loadStaticGraphicSet("add-building-button/cathedral", "data/img/gui/add-building/cathedral.png");
    loadStaticGraphicSet("add-building-button/tavern", "data/img/gui/add-building/tavern.png");
    loadStaticGraphicSet("add-building-button/sugarcane-plantation", "data/img/gui/add-building/sugarcane-plantation.png");
    loadStaticGraphicSet("add-building-button/sugarcane-field", "data/img/gui/add-building/sugarcane-field.png");
    loadStaticGraphicSet("add-building-button/tobacco-plantation", "data/img/gui/add-building/tobacco-plantation.png");
    loadStaticGraphicSet("add-building-button/tobacco-field", "data/img/gui/add-building/tobacco-field.png");
    loadStaticGraphicSet("add-building-button/spices-field", "data/img/gui/add-building/spices-field.png");
    loadStaticGraphicSet("add-building-button/distillery", "data/img/gui/add-building/distillery.png");
    loadStaticGraphicSet("add-building-button/tobacco-goods", "data/img/gui/add-building/tobacco-goods.png");
    loadStaticGraphicSet("add-building-button/spices-plantation", "data/img/gui/add-building/spices-plantation.png");
    loadStaticGraphicSet("add-building-button/square1", "data/img/gui/add-building/square1.png");
    loadStaticGraphicSet("add-building-button/square2", "data/img/gui/add-building/square2.png");
    loadStaticGraphicSet("add-building-button/square3", "data/img/gui/add-building/square3.png");
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

    loadStaticGraphicSet("coat-of-arms/large/red", "data/img/gui/coat-of-arms/large/red.png");
    loadStaticGraphicSet("coat-of-arms/large/blue", "data/img/gui/coat-of-arms/large/blue.png");
    loadStaticGraphicSet("coat-of-arms/large/yellow", "data/img/gui/coat-of-arms/large/yellow.png");
    loadStaticGraphicSet("coat-of-arms/large/white", "data/img/gui/coat-of-arms/large/white.png");

    loadStaticGraphicSet("button-music", "data/img/gui/button-music.png");
    loadStaticGraphicSet("button-music-pressed", "data/img/gui/button-music-pressed.png");

    loadStaticAnimationGraphicSetWith8Views("carrier", "data/img/animations/carrier.png", 31);
    loadStaticAnimationGraphicSetWith8Views("cart-without-cargo", "data/img/animations/cart-without-cargo.png", 32);
    loadStaticAnimationGraphicSetWith8Views("cart-with-cargo", "data/img/animations/cart-with-cargo.png", 32);

    loadHarvestablesGraphicSet("harvestable/northern-forest1", "data/img/harvestables/northern-forest1.png", 5);
    loadHarvestablesGraphicSet("harvestable/northern-forest2", "data/img/harvestables/northern-forest2.png", 5);
    loadHarvestablesGraphicSet("harvestable/sugarcane-field", "data/img/harvestables/sugarcane-field.png", 5);
    loadHarvestablesGraphicSet("harvestable/tobacco-field", "data/img/harvestables/tobacco-field.png", 7);
    loadHarvestablesGraphicSet("harvestable/spices-field", "data/img/harvestables/spices-field.png", 6);
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
            IGraphic* sdlTileGraphic = loadGraphic(*sdlFullGraphic, tileRect);

            graphicSet->addByView(fourDirectionsView, new Animation(sdlTileGraphic));
        }

        std::string graphicSetName = "tiles/" + mapTileConfig.tileName;
        graphicSets[graphicSetName] = graphicSet;
    }

    delete sdlFullGraphic;
}

void AbstractGraphicsMgr::loadStaticGraphicSet(const std::string& graphicSetName, const char* graphicFilename) {
    IGraphic* sdlGraphic = loadGraphic(graphicFilename);

    GraphicSet* graphicSet = new GraphicSet();
    graphicSet->addStatic(new Animation(sdlGraphic));

    graphicSets[graphicSetName] = graphicSet;
}

void AbstractGraphicsMgr::loadStaticAnimationGraphicSetWith8Views(
    const std::string& graphicSetName, const char* graphicFilename, unsigned int countFrames) {

    IGraphic* sdlFullGraphic = loadGraphic(graphicFilename);

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

            IGraphic* sdlFrameGraphic = loadGraphic(*sdlFullGraphic, frameRect);
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

    const std::string graphicFilename = "data/img/streets/" + streetTileset + ".png";
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
            IGraphic* sdlFrameGraphic = loadGraphic(*streetsGraphic, tileRect);
            graphicSet->addByView(view, new Animation(sdlFrameGraphic));
        }

        const std::string graphicSetName = "structure/" + streetTileset + "-" + streetTiles[i].angleName;
        graphicSets[graphicSetName] = graphicSet;
    }

    delete streetsGraphic;

    // wieder zurückstellen
    renderer->setHintRenderScaleQuality("2");
}

void AbstractGraphicsMgr::loadStaticGraphicSetWith4Views(
    const std::string& graphicSetName, const char* graphicFilename) {

    IGraphic* graphic = loadGraphic(graphicFilename);
    int tileWidth = graphic->getWidth() / 4;

    GraphicSet* graphicSet = new GraphicSet();
    Rect tileRect(0, 0, tileWidth, graphic->getHeight());

    forEachFourthDirection(view) {
        IGraphic* tileGraphic = loadGraphic(*graphic, tileRect);
        graphicSet->addByView(view, new Animation(tileGraphic));

        tileRect.x += tileWidth;
    }

    graphicSets[graphicSetName] = graphicSet;
    delete graphic;
}

void AbstractGraphicsMgr::loadHarvestablesGraphicSet(
    const std::string& graphicSetName, const char* graphicFilename, int tileStatesCount) {

    IGraphic* graphic = loadGraphic(graphicFilename);
    int tileWidth = graphic->getWidth() / tileStatesCount;
    int tileHeight = graphic->getHeight() / 4;

    GraphicSet* graphicSet = new GraphicSet();
    Rect tileRect(0, 0, tileWidth, tileHeight);

    forEachFourthDirection(view) {
        for (int stateIndex = 0; stateIndex < tileStatesCount; stateIndex++) {
            IGraphic* tileGraphic = loadGraphic(*graphic, tileRect);
            std::string state = "growth" + toString(stateIndex);
            graphicSet->addByStateAndView(state, view, new Animation(tileGraphic));

            tileRect.x += tileWidth;
        }

        tileRect.x = 0;
        tileRect.y += tileHeight;
    }

    graphicSets[graphicSetName] = graphicSet;
    delete graphic;
}
