#include <cstring>
#include "global.h"
#include "graphics/graphic/Animation.h"
#include "graphics/graphic/GraphicSet.h"
#include "graphics/mgr/AbstractGraphicsMgr.h"
#include "map/Street.h"
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

    loadStaticGraphicSetWith4Views("mapobjects/chapel", "data/img/buildings/chapel.png");
    loadStaticGraphicSetWith4Views("mapobjects/pioneers-house1", "data/img/buildings/pioneers-house1.png");
    loadStaticGraphicSetWith4Views("mapobjects/pioneers-house2", "data/img/buildings/pioneers-house2.png");
    loadStaticGraphicSetWith4Views("mapobjects/pioneers-house3", "data/img/buildings/pioneers-house3.png");
    loadStaticGraphicSetWith4Views("mapobjects/pioneers-house4", "data/img/buildings/pioneers-house4.png");
    loadStaticGraphicSetWith4Views("mapobjects/pioneers-house5", "data/img/buildings/pioneers-house5.png");
    loadStaticGraphicSetWith4Views("mapobjects/settlers-house1", "data/img/buildings/settlers-house1.png");
    loadStaticGraphicSetWith4Views("mapobjects/settlers-house2", "data/img/buildings/settlers-house2.png");
    loadStaticGraphicSetWith4Views("mapobjects/settlers-house3", "data/img/buildings/settlers-house3.png");
    loadStaticGraphicSetWith4Views("mapobjects/settlers-house4", "data/img/buildings/settlers-house4.png");
    loadStaticGraphicSetWith4Views("mapobjects/settlers-house5", "data/img/buildings/settlers-house5.png");
    loadStaticGraphicSetWith4Views("mapobjects/burghers-house1", "data/img/buildings/burghers-house1.png");
    loadStaticGraphicSetWith4Views("mapobjects/merchants-house1", "data/img/buildings/merchants-house1.png");
    loadStaticGraphicSetWith4Views("mapobjects/aristocrats-house1", "data/img/buildings/aristocrats-house1.png");
    loadStaticGraphicSetWith4Views("mapobjects/stonemason", "data/img/buildings/stonemason.png");
    loadStaticGraphicSetWith4Views("mapobjects/office1", "data/img/buildings/office1.png");
    loadStaticGraphicSetWith4Views("mapobjects/office2", "data/img/buildings/office2.png");
    loadStaticGraphicSetWith4Views("mapobjects/marketplace", "data/img/buildings/marketplace.png");
    loadStaticGraphicSetWith4Views("mapobjects/foresters", "data/img/buildings/foresters.png");
    loadStaticGraphicSetWith4Views("mapobjects/sheep-farm", "data/img/buildings/sheep-farm.png");
    loadStaticGraphicSetWith4Views("mapobjects/weaving-mill1", "data/img/buildings/weaving-mill1.png");
    loadStaticGraphicSetWith4Views("mapobjects/cattle-farm", "data/img/buildings/cattle-farm.png");
    loadStaticGraphicSetWith4Views("mapobjects/butchers", "data/img/buildings/butchers.png");
    loadStaticGraphicSetWith4Views("mapobjects/toolsmiths", "data/img/buildings/toolsmiths.png");
    loadStaticGraphicSetWith4Views("mapobjects/hunters-hut", "data/img/buildings/hunters-hut.png");
    loadStaticGraphicSetWith4Views("mapobjects/cathedral", "data/img/buildings/cathedral.png");
    loadStaticGraphicSetWith4Views("mapobjects/tavern", "data/img/buildings/tavern.png");
    loadStaticGraphicSetWith4Views("mapobjects/sugarcane-plantation", "data/img/buildings/sugarcane-plantation.png");
    loadStaticGraphicSetWith4Views("mapobjects/tobacco-plantation", "data/img/buildings/tobacco-plantation.png");
    loadStaticGraphicSetWith4Views("mapobjects/distillery", "data/img/buildings/distillery.png");
    loadStaticGraphicSetWith4Views("mapobjects/tobacco-goods", "data/img/buildings/tobacco-goods.png");
    loadStaticGraphicSetWith4Views("mapobjects/spices-plantation", "data/img/buildings/spices-plantation.png");
    loadStaticGraphicSetWith4Views("mapobjects/school", "data/img/buildings/school.png");

    loadStreetTileset("cobbled-street");
    loadStreetTileset("farm-road");

    loadStaticGraphicSetWith4Views("mapobjects/square1", "data/img/buildings/square1.png");
    loadStaticGraphicSetWith4Views("mapobjects/square2", "data/img/buildings/square2.png");
    loadStaticGraphicSetWith4Views("mapobjects/square3", "data/img/buildings/square3.png");
    loadStaticGraphicSetWith4Views("mapobjects/pier", "data/img/buildings/pier.png");

    const std::list<const Good*>& allGoods = configMgr->getAllGoodsOrdered();
    for (auto iter = allGoods.cbegin(); iter != allGoods.cend(); iter++) {
        const Good* good = *iter;

        if (good->invisible) {
            continue;
        }

        loadStaticGraphicSet("goods-icon/" + good->name,
                             std::string("data/img/goods/icon/" + good->name + ".png").c_str());
        loadStaticGraphicSet("goods-marketplace-icon/" + good->name,
                             std::string("data/img/goods/marketplace-icon/" + good->name + ".png").c_str());
    }

    loadStaticGraphicSet("coin/coin", "data/img/gui/coin/coin.png");
    loadStaticGraphicSet("coin/coin-green-arrow", "data/img/gui/coin/coin-green-arrow.png");
    loadStaticGraphicSet("coin/coin-red-arrow", "data/img/gui/coin/coin-red-arrow.png");
    loadStaticGraphicSet("population-man", "data/img/gui/population-man.png");
    loadStaticGraphicSet("panel", "data/img/gui/panel.png");
    loadStaticGraphicSet("statusbar", "data/img/gui/statusbar.png");
    loadStaticGraphicSet("panel-header", "data/img/gui/panel-header.png");
    loadStaticGraphicSet("minimap", "data/img/gui/minimap.png");
    loadMapRotateGraphicSet();
    loadMapZoomGraphicSet();

    loadStaticGraphicSet("panel-buttons/build", "data/img/gui/panel-buttons/build.png");
    loadStaticGraphicSet("panel-buttons/build-pressed", "data/img/gui/panel-buttons/build-pressed.png");
    loadStaticGraphicSet("panel-buttons/dummy", "data/img/gui/panel-buttons/dummy.png");
    loadStaticGraphicSet("panel-buttons/dummy-pressed", "data/img/gui/panel-buttons/dummy-pressed.png");
    loadStaticGraphicSet("panel-buttons/player-status", "data/img/gui/panel-buttons/player-status.png");
    loadStaticGraphicSet("panel-buttons/player-status-pressed", "data/img/gui/panel-buttons/player-status-pressed.png");

    loadStaticGraphicSet("add-building-grid", "data/img/gui/add-building/add-building-grid.png");

    loadStaticGraphicSet("add-building-button/chapel", "data/img/gui/add-building/chapel.png");
    loadStaticGraphicSet("add-building-button/marketplace", "data/img/gui/add-building/marketplace.png");
    loadStaticGraphicSet("add-building-button/office1", "data/img/gui/add-building/office1.png");
    loadStaticGraphicSet("add-building-button/office2", "data/img/gui/add-building/office2.png");
    loadStaticGraphicSet("add-building-button/cobbled-street", "data/img/gui/add-building/cobbled-street.png");
    loadStaticGraphicSet("add-building-button/farm-road", "data/img/gui/add-building/farm-road.png");
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
    loadStaticGraphicSet("add-building-button/school", "data/img/gui/add-building/school.png");
    loadStaticGraphicSet("add-building-button/square1", "data/img/gui/add-building/square1.png");
    loadStaticGraphicSet("add-building-button/square2", "data/img/gui/add-building/square2.png");
    loadStaticGraphicSet("add-building-button/square3", "data/img/gui/add-building/square3.png");
    loadStaticGraphicSet("add-building-button/pier", "data/img/gui/add-building/pier.png");

    // TODO generate-build-buttons.php entfernen, wenn die neuen Blender-Buttons komplett sind
    loadStaticGraphicSet("add-building-group/craftsman", "data/img/gui/button-add-building-craftsman.png"); // TODO build-menu-Grafik verwenden
    loadStaticGraphicSet("add-building-group/craftsman-pressed", "data/img/gui/button-add-building-craftsman-pressed.png"); // TODO build-menu-Grafik verwenden
    loadStaticGraphicSet("add-building-group/farm", "data/img/gui/button-add-building-farm.png"); // TODO build-menu-Grafik verwenden
    loadStaticGraphicSet("add-building-group/farm-pressed", "data/img/gui/button-add-building-farm-pressed.png"); // TODO build-menu-Grafik verwenden
    loadStaticGraphicSet("build-menu/port", "data/img/gui/build-menu/port.png");
    loadStaticGraphicSet("build-menu/port-pressed", "data/img/gui/build-menu/port-pressed.png");
    loadStaticGraphicSet("build-menu/public", "data/img/gui/build-menu/public.png");
    loadStaticGraphicSet("build-menu/public-pressed", "data/img/gui/build-menu/public-pressed.png");

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

    loadStaticGraphicSet("population-satisfaction/good", "data/img/gui/population-satisfaction/good.png");
    loadStaticGraphicSet("population-satisfaction/neutral", "data/img/gui/population-satisfaction/neutral.png");
    loadStaticGraphicSet("population-satisfaction/bad", "data/img/gui/population-satisfaction/bad.png");
    loadStaticGraphicSet("population-satisfaction/worst", "data/img/gui/population-satisfaction/worst.png");

//    loadStaticGraphicSet("public-building-icon/bathhouse", "data/img/gui/public-building-icon/bathhouse.png");
    loadStaticGraphicSet("public-building-icon/chapel", "data/img/gui/public-building-icon/chapel.png");
//    loadStaticGraphicSet("public-building-icon/church", "data/img/gui/public-building-icon/church.png");
//    loadStaticGraphicSet("public-building-icon/high-school", "data/img/gui/public-building-icon/high-school.png");
    loadStaticGraphicSet("public-building-icon/marketplace", "data/img/gui/public-building-icon/marketplace.png");
    loadStaticGraphicSet("public-building-icon/school", "data/img/gui/public-building-icon/school.png");
    loadStaticGraphicSet("public-building-icon/tavern", "data/img/gui/public-building-icon/tavern.png");
//    loadStaticGraphicSet("public-building-icon/theater", "data/img/gui/public-building-icon/theater.png");

    loadStaticAnimationGraphicSetWith8Views("carrier", "data/img/animations/carrier.png", 31);
    loadStaticAnimationGraphicSetWith8Views("cart-without-cargo", "data/img/animations/cart-without-cargo.png", 32);
    loadStaticAnimationGraphicSetWith8Views("cart-with-cargo", "data/img/animations/cart-with-cargo.png", 32);
    loadSheepGraphicSets();
    loadCattleGraphicSets();

    loadHarvestablesGraphicSet("mapobjects/northern-forest1", "data/img/harvestables/northern-forest1.png", 5);
    loadHarvestablesGraphicSet("mapobjects/northern-forest2", "data/img/harvestables/northern-forest2.png", 5);
    loadHarvestablesGraphicSet("mapobjects/sugarcane-field", "data/img/harvestables/sugarcane-field.png", 5);
    loadHarvestablesGraphicSet("mapobjects/tobacco-field", "data/img/harvestables/tobacco-field.png", 7);
    loadHarvestablesGraphicSet("mapobjects/spices-field", "data/img/harvestables/spices-field.png", 6);

#ifdef DEBUG_GUIMAP
    loadStaticGraphicSet("debug/grid-overlay-evelation0", "data/debug-grid-overlay-elevation0.png");
    loadStaticGraphicSet("debug/grid-overlay-screencoords", "data/debug-grid-overlay-screencoords.png");
#endif
#if defined(DEBUG_GUIMAP_COORDS) || defined(DEBUG_GUIMAP)
    loadStaticGraphicSet("debug/grid-overlay-evelation1", "data/debug-grid-overlay-elevation1.png");
#endif
}

void AbstractGraphicsMgr::loadTiles() {
    // nearest pixel sampling für die Kacheln, damit die fließend ineinander übergehen
    renderer->setHintRenderScaleQuality("0");

    IGraphic* sdlFullGraphic = loadGraphic("data/img/tileset.png");

    for (auto mapEntry : configMgr->getMapTileConfigs()) {
        const MapTileConfig& mapTileConfig = mapEntry.second;

        GraphicSet* graphicSet = new GraphicSet();

        Rect tileRect(0, 0, 64, 64);
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
        Log::error(_("Could not divide the frames equally: '%s'"), graphicFilename);
        throw std::runtime_error("Could not divide the frames equally");
    }
    int frameWidth = fullGraphicWidth / countFrames;

    int fullGraphicHeight = sdlFullGraphic->getHeight();
    if (fullGraphicHeight % 8 != 0) {
        Log::error(_("Could not divide the views equally: '%s'"), graphicFilename);
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
        const std::string state;
        int tileOffsetPerView[4];
    } streetTiles[11]{
        { Street::STATE_NAME_EW, { 0, 1, 0, 1 } },
        { Street::STATE_NAME_NS, { 1, 0, 1, 0 } },
        { Street::STATE_NAME_NE, { 2, 3, 4, 5 } },
        { Street::STATE_NAME_ES, { 3, 4, 5, 2 } },
        { Street::STATE_NAME_SW, { 4, 5, 2, 3 } },
        { Street::STATE_NAME_NW, { 5, 2, 3, 4 } },
        { Street::STATE_NAME_NSW, { 6, 7, 8, 9 } },
        { Street::STATE_NAME_NEW, { 7, 8, 9, 6 } },
        { Street::STATE_NAME_NES, { 8, 9, 6, 7 }} ,
        { Street::STATE_NAME_ESW, { 9, 6, 7, 8 } },
        { Street::STATE_NAME_NESW, { 10, 10, 10, 10 } }
    };

    GraphicSet* graphicSet = new GraphicSet();

    Rect tileRect(0, 0, TILE_WIDTH, TILE_HEIGHT);
    for (int i = 0; i < 11; i++) {
        const std::string& state = streetTiles[i].state;

        forEachFourthDirection(view) {
            tileRect.x = TILE_WIDTH * streetTiles[i].tileOffsetPerView[((unsigned char) view) / 2];
            IGraphic* sdlFrameGraphic = loadGraphic(*streetsGraphic, tileRect);
            graphicSet->addByStateAndView(state, view, new Animation(sdlFrameGraphic));
        }
    }
    graphicSets["mapobjects/" + streetTileset] = graphicSet;

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

void AbstractGraphicsMgr::loadMapRotateGraphicSet() {
    IGraphic* graphic = loadGraphic("data/img/gui/map-rotate.png");
    int tileWidth = graphic->getWidth() / 16;
    int height = graphic->getHeight();

    GraphicSet* graphicSet = new GraphicSet();
    Rect tileRect(0, 0, tileWidth, height);

    forEachFourthDirection(view) {
        for (const char* stateName : {"left", "right"}) {
            for (const char* stateSuffix : {"", "_pressed"}) {
                std::string state = std::string(stateName) + stateSuffix;

                IGraphic* tileGraphic = loadGraphic(*graphic, tileRect);
                graphicSet->addByStateAndView(state, view, new Animation(tileGraphic));

                tileRect.x += tileWidth;
            }
        }
    }

    graphicSets["map-rotate"] = graphicSet;
    delete graphic;
}

void AbstractGraphicsMgr::loadMapZoomGraphicSet() {
    IGraphic* graphic = loadGraphic("data/img/gui/map-zoom.png");
    int tileWidth = graphic->getWidth() / 4;
    int height = graphic->getHeight();

    GraphicSet* graphicSet = new GraphicSet();
    Rect tileRect(0, 0, tileWidth, height);

    for (const char* stateName : {"plus", "minus"}) {
        for (const char* stateSuffix : {"", "_pressed"}) {
            std::string state = std::string(stateName) + stateSuffix;

            IGraphic* tileGraphic = loadGraphic(*graphic, tileRect);
            graphicSet->addByState(state, new Animation(tileGraphic));

            tileRect.x += tileWidth;
        }
    }

    graphicSets["map-zoom"] = graphicSet;
    delete graphic;
}

// TODO Duplicate Code loadSheepGraphicSets() / loadCattleGraphicSets() - über XML-Datei regeln
void AbstractGraphicsMgr::loadSheepGraphicSets() {
    for (int i = 0; i < 2; i++) {
        const std::string graphicFilename = "data/img/animations/sheep" + toString(i) + ".png";
        IGraphic* graphic = loadGraphic(graphicFilename.c_str());

        int frameWidth = graphic->getWidth() / 12;
        int frameHeight = graphic->getHeight() / 8;

        GraphicSet* graphicSet = new GraphicSet();
        Rect frameRect(0, 0, frameWidth, frameHeight);

        forEachEighthDirection(view) {
            frameRect.x = 0;

            Animation* animationWalking = new Animation(8);
            for (int frameIndex = 0; frameIndex < 8; frameIndex++, frameRect.x += frameWidth) {
                IGraphic* frameGraphic = loadGraphic(*graphic, frameRect);
                animationWalking->addFrame(frameIndex, frameGraphic);
            }
            graphicSet->addByStateAndView("walking", view, animationWalking);

            Animation* animationEating = new Animation(4);
            for (int frameIndex = 0; frameIndex < 4; frameIndex++, frameRect.x += frameWidth) {
                IGraphic* frameGraphic = loadGraphic(*graphic, frameRect);
                animationEating->addFrame(frameIndex, frameGraphic);
            }
            graphicSet->addByStateAndView("eating", view, animationEating);

            // TODO später entfernen; nur dazu da, dass der bisherige Code funktioniert.
            IGraphic* firstFrameGraphic = loadGraphic(*graphic, Rect(0, frameRect.y, frameWidth, frameHeight));
            graphicSet->addByView(view, new Animation(firstFrameGraphic));

            frameRect.y += frameHeight;
        }

        graphicSets["sheep" + toString(i)] = graphicSet;
        delete graphic;
    }
}

// TODO Duplicate Code loadSheepGraphicSets() / loadCattleGraphicSets() - über XML-Datei regeln
void AbstractGraphicsMgr::loadCattleGraphicSets() {
    IGraphic* graphic = loadGraphic("data/img/animations/cattle.png");

    int frameWidth = graphic->getWidth() / 14;
    int frameHeight = graphic->getHeight() / 8;

    GraphicSet* graphicSet = new GraphicSet();
    Rect frameRect(0, 0, frameWidth, frameHeight);

    forEachEighthDirection(view) {
        frameRect.x = 0;

        Animation* animationWalking = new Animation(8);
        for (int frameIndex = 0; frameIndex < 8; frameIndex++, frameRect.x += frameWidth) {
            IGraphic* frameGraphic = loadGraphic(*graphic, frameRect);
            animationWalking->addFrame(frameIndex, frameGraphic);
        }
        graphicSet->addByStateAndView("walking", view, animationWalking);

        Animation* animationEating = new Animation(6);
        for (int frameIndex = 0; frameIndex < 6; frameIndex++, frameRect.x += frameWidth) {
            IGraphic* frameGraphic = loadGraphic(*graphic, frameRect);
            animationEating->addFrame(frameIndex, frameGraphic);
        }
        graphicSet->addByStateAndView("eating", view, animationEating);

        // TODO später entfernen; nur dazu da, dass der bisherige Code funktioniert.
        IGraphic* firstFrameGraphic = loadGraphic(*graphic, Rect(0, frameRect.y, frameWidth, frameHeight));
        graphicSet->addByView(view, new Animation(firstFrameGraphic));

        frameRect.y += frameHeight;
    }

    graphicSets["cattle"] = graphicSet;
    delete graphic;
}