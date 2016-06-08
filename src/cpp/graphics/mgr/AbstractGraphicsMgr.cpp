#include <cstring>
#include "global.h"
#include "graphics/graphic/Animation.h"
#include "graphics/graphic/GraphicSet.h"
#include "graphics/graphic/GraphicSetKeyState.h"
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

    loadGraphicsFromXmlConfig("data/config/graphics.xml");

    loadStreetTileset("cobbled-street");
    loadStreetTileset("farm-road");

    const std::list<const Good*>& allGoods = configMgr->getAllGoodsOrdered();
    for (auto iter = allGoods.cbegin(); iter != allGoods.cend(); iter++) {
        const Good* good = *iter;

        if (good->invisible) {
            continue;
        }

        GraphicSet* graphicSet = new GraphicSet();
        graphicSets["goods-icon/" + good->name] = graphicSet;
        loadStaticGraphicSet(graphicSet, std::string("data/img/goods/icon/" + good->name + ".png").c_str());

        graphicSet = new GraphicSet();
        graphicSets["goods-marketplace-icon/" + good->name] = graphicSet;
        loadStaticGraphicSet(graphicSet, std::string("data/img/goods/marketplace-icon/" + good->name + ".png").c_str());
    }

    loadMapRotateGraphicSet();
    loadMapZoomGraphicSet();

    loadCarrierGraphicSet();
    loadCartGraphicSet();
    loadSheepGraphicSet();
    loadCattleGraphicSet();

#ifdef DEBUG_GUIMAP
    loadStaticGraphicSet("debug/grid-overlay-evelation0", "data/debug-grid-overlay-elevation0.png");
    loadStaticGraphicSet("debug/grid-overlay-screencoords", "data/debug-grid-overlay-screencoords.png");
#endif
#if defined(DEBUG_GUIMAP_COORDS) || defined(DEBUG_GUIMAP)
    loadStaticGraphicSet("debug/grid-overlay-evelation1", "data/debug-grid-overlay-elevation1.png");
#endif

    linkGraphicSetsToMapObjectTypes();
}

void AbstractGraphicsMgr::loadTiles() {
    // nearest pixel sampling für die Kacheln, damit die fließend ineinander übergehen
    renderer->setHintRenderScaleQuality("0");

    IGraphic* fullGraphic = loadGraphic("data/img/tileset.png"); // TODO konfigurierbar in tiles.xml machen

    for (auto mapEntry : configMgr->getMapTileConfigs()) {
        const MapTileConfig& mapTileConfig = mapEntry.second;

        GraphicSet* graphicSet = new GraphicSet();

        Rect tileRect(0, 0, 64, 64);
        forEachFourthDirection(fourDirectionsView) {
            const std::pair<int, int>& xyOffsetInTileset =
                mapTileConfig.mapTileViewsOffsetXYInTileset.at(fourDirectionsView);

            tileRect.x = xyOffsetInTileset.first;
            tileRect.y = xyOffsetInTileset.second;
            IGraphic* tileGraphic = loadGraphic(*fullGraphic, tileRect);

            graphicSet->addByView(fourDirectionsView, new Animation(tileGraphic));
        }

        std::string graphicSetName = "tiles/" + mapTileConfig.tileName;
        graphicSets[graphicSetName] = graphicSet;
    }

    delete fullGraphic;
}

void AbstractGraphicsMgr::loadGraphicsFromXmlConfig(const std::string& configFilePath) {
    // Datei öffnen
    rapidxml::file<> xmlFile(configFilePath.c_str());

    rapidxml::xml_document<>* xmlDocument = new rapidxml::xml_document<>();
    xmlDocument->parse<0>(xmlFile.data());

    rapidxml::xml_node<>* graphicsNode = xmlDocument->first_node("graphics", 8, true);

    for (rapidxml::xml_node<>* graphicSetNode = graphicsNode->first_node("graphic-set", 11, true);
         graphicSetNode != nullptr;
         graphicSetNode = graphicSetNode->next_sibling("graphic-set", 11, true)) {

        // GraphicSet anlegen
        std::string graphicSetName = std::string(graphicSetNode->first_attribute("name", 4, true)->value());

        GraphicSet* graphicSet = new GraphicSet();
        graphicSets[graphicSetName] = graphicSet;

        // statische Grafik
        rapidxml::xml_node<>* staticNode = graphicSetNode->first_node("static", 6, true);
        if (staticNode != nullptr) {
            const char* imageFile = staticNode->first_attribute("image-file", 10, true)->value();

            loadStaticGraphicSet(graphicSet, imageFile);
        }

        // 4x (je View) nebeneinander
        rapidxml::xml_node<>* staticViewsHorizontalNode = graphicSetNode->first_node("static-views-horizontal", 23, true);
        if (staticViewsHorizontalNode != nullptr) {
            const char* imageFile = staticViewsHorizontalNode->first_attribute("image-file", 10, true)->value();

            loadStaticGraphicSetWith4Views(graphicSet, imageFile);
        }

        // verschiedene Wachstumzustände (nebeneinander) 4x (je View) untereinander
        rapidxml::xml_node<>* harvestableNode = graphicSetNode->first_node("harvestable", 11, true);
        if (harvestableNode != nullptr) {
            const char* imageFile = harvestableNode->first_attribute("image-file", 10, true)->value();
            int statesCount = std::atoi(harvestableNode->first_attribute("states-count", 12, true)->value());

            loadHarvestablesGraphicSet(graphicSet, imageFile, statesCount);
        }


        // Jetzt sollte mindestens ein if-Block durchlaufen und was im GraphicSet sein
        if (graphicSet->isEmpty()) {
            Log::error(_("No graphics load for graphicSet '%s'"), graphicSetName.c_str());
            throw std::runtime_error("No graphics load for graphicSet");
        }
    }

    delete xmlDocument;
}

void AbstractGraphicsMgr::loadStaticGraphicSet(GraphicSet* graphicSet, const char* graphicFilename) {
    IGraphic* graphic = loadGraphic(graphicFilename);

    graphicSet->addStatic(new Animation(graphic));
}

void AbstractGraphicsMgr::loadStreetTileset(const std::string& streetTileset) {
    // nearest pixel sampling für die Straßen-Kacheln, damit die auch fließend ineinander übergehen, wenn rausgezoomt is
    renderer->setHintRenderScaleQuality("0");

    const std::string graphicFilename = "data/img/streets/" + streetTileset + ".png";
    IGraphic* streetsGraphic = loadGraphic(graphicFilename.c_str());

    static struct {
        GraphicSetKeyState state;
        int tileOffsetPerView[4];
    } streetTiles[11]{
        { GraphicSetKeyState::STRAIGHT0, { 0, 1, 0, 1 } },
        { GraphicSetKeyState::STRAIGHT90, { 1, 0, 1, 0 } },
        { GraphicSetKeyState::CURVE0, { 2, 3, 4, 5 } },
        { GraphicSetKeyState::CURVE90, { 3, 4, 5, 2 } },
        { GraphicSetKeyState::CURVE180, { 4, 5, 2, 3 } },
        { GraphicSetKeyState::CURVE270, { 5, 2, 3, 4 } },
        { GraphicSetKeyState::TEE0, { 6, 7, 8, 9 } },
        { GraphicSetKeyState::TEE90, { 7, 8, 9, 6 } },
        { GraphicSetKeyState::TEE180, { 8, 9, 6, 7 }} ,
        { GraphicSetKeyState::TEE270, { 9, 6, 7, 8 } },
        { GraphicSetKeyState::CROSS, { 10, 10, 10, 10 } }
    };

    GraphicSet* graphicSet = new GraphicSet();

    Rect tileRect(0, 0, TILE_WIDTH, TILE_HEIGHT);
    for (int i = 0; i < 11; i++) {
        const GraphicSetKeyState& state = streetTiles[i].state;

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

void AbstractGraphicsMgr::loadStaticGraphicSetWith4Views(GraphicSet* graphicSet, const char* graphicFilename) {
    IGraphic* graphic = loadGraphic(graphicFilename);
    int tileWidth = graphic->getWidth() / 4;

    Rect tileRect(0, 0, tileWidth, graphic->getHeight());

    forEachFourthDirection(view) {
        IGraphic* tileGraphic = loadGraphic(*graphic, tileRect);
        graphicSet->addByView(view, new Animation(tileGraphic));

        tileRect.x += tileWidth;
    }

    delete graphic;
}

void AbstractGraphicsMgr::loadHarvestablesGraphicSet(
    GraphicSet* graphicSet, const char* graphicFilename, int statesCount) {

    if (statesCount < 1 || statesCount > 7) {
        Log::error(_("Illegal statesCount %d for graphicFilename %s"), statesCount, graphicFilename);
        throw std::runtime_error("Illegal statesCount");
    }


    IGraphic* graphic = loadGraphic(graphicFilename);
    int tileWidth = graphic->getWidth() / statesCount;
    int tileHeight = graphic->getHeight() / 4;

    Rect tileRect(0, 0, tileWidth, tileHeight);

    forEachFourthDirection(view) {
        for (int stateIndex = 0; stateIndex < statesCount; stateIndex++) {
            IGraphic* tileGraphic = loadGraphic(*graphic, tileRect);

            GraphicSetKeyState state = (GraphicSetKeyState) (GraphicSetKeyState::GROWTH0 + stateIndex);
            assert (state >= GraphicSetKeyState::GROWTH0 && state <= GraphicSetKeyState::GROWTH6);

            graphicSet->addByStateAndView(state, view, new Animation(tileGraphic));

            tileRect.x += tileWidth;
        }

        tileRect.x = 0;
        tileRect.y += tileHeight;
    }

    delete graphic;
}

void AbstractGraphicsMgr::loadMapRotateGraphicSet() {
    IGraphic* graphic = loadGraphic("data/img/gui/map-rotate.png");
    int tileWidth = graphic->getWidth() / 16;
    int height = graphic->getHeight();

    GraphicSet* graphicSet = new GraphicSet();
    Rect tileRect(0, 0, tileWidth, height);

    forEachFourthDirection(view) {
        IGraphic* tileGraphic = loadGraphic(*graphic, tileRect);
        graphicSet->addByStateAndView(GraphicSetKeyState::LEFT, view, new Animation(tileGraphic));
        tileRect.x += tileWidth;

        tileGraphic = loadGraphic(*graphic, tileRect);
        graphicSet->addByStateAndView(GraphicSetKeyState::LEFT_PRESSED, view, new Animation(tileGraphic));
        tileRect.x += tileWidth;

        tileGraphic = loadGraphic(*graphic, tileRect);
        graphicSet->addByStateAndView(GraphicSetKeyState::RIGHT, view, new Animation(tileGraphic));
        tileRect.x += tileWidth;

        tileGraphic = loadGraphic(*graphic, tileRect);
        graphicSet->addByStateAndView(GraphicSetKeyState::RIGHT_PRESSED, view, new Animation(tileGraphic));
        tileRect.x += tileWidth;
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

    IGraphic* tileGraphic = loadGraphic(*graphic, tileRect);
    graphicSet->addByState(GraphicSetKeyState::PLUS, new Animation(tileGraphic));
    tileRect.x += tileWidth;

    tileGraphic = loadGraphic(*graphic, tileRect);
    graphicSet->addByState(GraphicSetKeyState::PLUS_PRESSED, new Animation(tileGraphic));
    tileRect.x += tileWidth;

    tileGraphic = loadGraphic(*graphic, tileRect);
    graphicSet->addByState(GraphicSetKeyState::MINUS, new Animation(tileGraphic));
    tileRect.x += tileWidth;

    tileGraphic = loadGraphic(*graphic, tileRect);
    graphicSet->addByState(GraphicSetKeyState::MINUS_PRESSED, new Animation(tileGraphic));

    graphicSets["map-zoom"] = graphicSet;
    delete graphic;
}

// TODO Duplicate Code loadCarrierGraphicSet() / loadCartGraphicSet() / loadSheepGraphicSets() / loadCattleGraphicSet() - über XML-Datei regeln
void AbstractGraphicsMgr::loadCarrierGraphicSet() {
    IGraphic* fullGraphic = loadGraphic("data/img/animations/carrier.png");

    int fullGraphicWidth = fullGraphic->getWidth();
    if (fullGraphicWidth % 31 != 0) {
        Log::error(_("Could not divide the frames equally: '%s'"), "data/img/animations/carrier.png");
        throw std::runtime_error("Could not divide the frames equally");
    }
    int frameWidth = fullGraphicWidth / 31;

    int fullGraphicHeight = fullGraphic->getHeight();
    if (fullGraphicHeight % 8 != 0) {
        Log::error(_("Could not divide the views equally: '%s'"), "data/img/animations/carrier.png");
        throw std::runtime_error("Could not divide the views equally");
    }
    int frameHeight = fullGraphicHeight / 8;

    GraphicSet* graphicSet = new GraphicSet();
    Rect frameRect(0, 0, frameWidth, frameHeight);

    forEachEighthDirection(view) {
        Animation* animation = new Animation(31);
        for (int x = 0, frameIndex = 0; x < fullGraphicWidth; x += frameWidth, frameIndex++) {
            frameRect.x = x;

            IGraphic* frameGraphic = loadGraphic(*fullGraphic, frameRect);
            animation->addFrame(frameIndex, frameGraphic);
        }

        graphicSet->addByStateAndView(GraphicSetKeyState::WALKING, view, animation);
        frameRect.y += frameHeight;
    }

    graphicSets["carrier"] = graphicSet;

    delete fullGraphic;
}

// TODO Duplicate Code loadCarrierGraphicSet() / loadCartGraphicSet() / loadSheepGraphicSets() / loadCattleGraphicSet() - über XML-Datei regeln
void AbstractGraphicsMgr::loadCartGraphicSet() {
    GraphicSet* graphicSet = new GraphicSet();

    for (int i = 0; i < 2; i++) {
        const char* graphicFilename = (i == 0) ?
            "data/img/animations/cart-without-cargo.png" : "data/img/animations/cart-with-cargo.png";
        IGraphic* fullGraphic = loadGraphic(graphicFilename);

        int fullGraphicWidth = fullGraphic->getWidth();
        if (fullGraphicWidth % 32 != 0) {
            Log::error(_("Could not divide the frames equally: '%s'"), graphicFilename);
            throw std::runtime_error("Could not divide the frames equally");
        }
        int frameWidth = fullGraphicWidth / 32;

        int fullGraphicHeight = fullGraphic->getHeight();
        if (fullGraphicHeight % 8 != 0) {
            Log::error(_("Could not divide the views equally: '%s'"), graphicFilename);
            throw std::runtime_error("Could not divide the views equally");
        }
        int frameHeight = fullGraphicHeight / 8;

        Rect frameRect(0, 0, frameWidth, frameHeight);

        forEachEighthDirection(view) {
            Animation* animation = new Animation(32);
            for (int x = 0, frameIndex = 0; x < fullGraphicWidth; x += frameWidth, frameIndex++) {
                frameRect.x = x;

                IGraphic* frameGraphic = loadGraphic(*fullGraphic, frameRect);
                animation->addFrame(frameIndex, frameGraphic);
            }

            GraphicSetKeyState state = (i == 0) ? GraphicSetKeyState::WALKING : GraphicSetKeyState::WALKING_WITH_GOODS;
            graphicSet->addByStateAndView(state, view, animation);
            frameRect.y += frameHeight;
        }

        delete fullGraphic;
    }

    graphicSets["cart"] = graphicSet;
}

// TODO Duplicate Code loadCarrierGraphicSet() / loadCartGraphicSet() / loadSheepGraphicSets() / loadCattleGraphicSet() - über XML-Datei regeln
void AbstractGraphicsMgr::loadSheepGraphicSet() {
    GraphicSet* graphicSet = new GraphicSet();

    for (int i = 0; i < 2; i++) {
        const std::string graphicFilename = "data/img/animations/sheep" + toString(i) + ".png";
        IGraphic* graphic = loadGraphic(graphicFilename.c_str());

        int frameWidth = graphic->getWidth() / 12;
        int frameHeight = graphic->getHeight() / 8;

        Rect frameRect(0, 0, frameWidth, frameHeight);

        forEachEighthDirection(view) {
            frameRect.x = 0;

            Animation* animationWalking = new Animation(8);
            for (int frameIndex = 0; frameIndex < 8; frameIndex++, frameRect.x += frameWidth) {
                IGraphic* frameGraphic = loadGraphic(*graphic, frameRect);
                animationWalking->addFrame(frameIndex, frameGraphic);
            }
            GraphicSetKeyState state = (i == 0) ? GraphicSetKeyState::WALKING : GraphicSetKeyState::WALKING_WITH_GOODS;
            graphicSet->addByStateAndView(state, view, animationWalking);

            Animation* animationEating = new Animation(4);
            for (int frameIndex = 0; frameIndex < 4; frameIndex++, frameRect.x += frameWidth) {
                IGraphic* frameGraphic = loadGraphic(*graphic, frameRect);
                animationEating->addFrame(frameIndex, frameGraphic);
            }
            state = (i == 0) ? GraphicSetKeyState::EATING : GraphicSetKeyState::EATING_WITH_GOODS;
            graphicSet->addByStateAndView(state, view, animationEating);

            // TODO später entfernen; nur dazu da, dass der bisherige Code funktioniert.
            IGraphic* firstFrameGraphic = loadGraphic(*graphic, Rect(0, frameRect.y, frameWidth, frameHeight));
            graphicSet->addByView(view, new Animation(firstFrameGraphic));

            frameRect.y += frameHeight;
        }

        delete graphic;
    }

    graphicSets["sheep"] = graphicSet;
}

// TODO Duplicate Code loadCarrierGraphicSet() / loadCartGraphicSet() / loadSheepGraphicSets() / loadCattleGraphicSet() - über XML-Datei regeln
void AbstractGraphicsMgr::loadCattleGraphicSet() {
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
        graphicSet->addByStateAndView(GraphicSetKeyState::WALKING, view, animationWalking);

        Animation* animationEating = new Animation(6);
        for (int frameIndex = 0; frameIndex < 6; frameIndex++, frameRect.x += frameWidth) {
            IGraphic* frameGraphic = loadGraphic(*graphic, frameRect);
            animationEating->addFrame(frameIndex, frameGraphic);
        }
        graphicSet->addByStateAndView(GraphicSetKeyState::EATING, view, animationEating);

        // TODO später entfernen; nur dazu da, dass der bisherige Code funktioniert.
        IGraphic* firstFrameGraphic = loadGraphic(*graphic, Rect(0, frameRect.y, frameWidth, frameHeight));
        graphicSet->addByView(view, new Animation(firstFrameGraphic));

        frameRect.y += frameHeight;
    }

    graphicSets["cattle"] = graphicSet;
    delete graphic;
}

void AbstractGraphicsMgr::linkGraphicSetsToMapObjectTypes() {
    const std::list<const MapObjectType*> allMapObjectTypes = configMgr->getAllMapObjectTypes();

    for (const MapObjectType* mapObjectType : allMapObjectTypes) {
        const std::string& graphicSetName = mapObjectType->graphicSetName;

        if (graphicSetName.empty()) {
            Log::error(_("No graphicSet defined for mapObjectType '%s'"), mapObjectType->name.c_str());
            throw std::runtime_error("No graphicSet defined for mapObjectType");
        }

        const GraphicSet* graphicSet = getGraphicSet(graphicSetName);
        if (graphicSet == nullptr) {
            Log::error(_("Could not find graphicSet: '%s'"), graphicSetName.c_str());
            throw std::runtime_error("Could not find graphicSet");
        }

        configMgr->initMapObjectTypeGraphicSet(mapObjectType, graphicSet);
    }
}
