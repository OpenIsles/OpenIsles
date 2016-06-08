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

        for (rapidxml::xml_node<>* subNode = graphicSetNode->first_node();
             subNode != nullptr;
             subNode = subNode->next_sibling()) {

            const char* nodeName = subNode->name();

            // statische Grafik
            if (std::strcmp(nodeName, "static") == 0) {
                const char* imageFile = subNode->first_attribute("image-file", 10, true)->value();

                loadStaticGraphicSet(graphicSet, imageFile);
            }

            // 4x (je View) nebeneinander
            else if (std::strcmp(nodeName, "static-views-horizontal") == 0) {
                const char* imageFile = subNode->first_attribute("image-file", 10, true)->value();

                loadStaticGraphicSetWith4Views(graphicSet, imageFile);
            }

            // verschiedene Wachstumzustände (nebeneinander) 4x (je View) untereinander
            else if (std::strcmp(nodeName, "harvestable") == 0) {
                const char* imageFile = subNode->first_attribute("image-file", 10, true)->value();
                int statesCount = std::atoi(subNode->first_attribute("states-count", 12, true)->value());

                loadHarvestablesGraphicSet(graphicSet, imageFile, statesCount);
            }

            // Animation (Frames nach rechts) mit 8 Ansichten (untereinander)
            else if (std::strcmp(nodeName, "animation") == 0) {
                const char* stateName = subNode->first_attribute("state", 5, true)->value();
                const char* imageFile = subNode->first_attribute("image-file", 10, true)->value();
                int framesCount = std::atoi(subNode->first_attribute("frames-count", 12, true)->value());

                int startFrame = 0;
                int endFrame = framesCount - 1;

                rapidxml::xml_attribute<>* framesAttribute = subNode->first_attribute("frames", 6, true);
                if (framesAttribute != nullptr) {
                    std::string framesAttributeValue = std::string(framesAttribute->value());

                    std::size_t c = framesAttributeValue.find("-");
                    if (c == std::string::npos) {
                        Log::error(_("Could not parse frames '%s'"), framesAttributeValue.c_str());
                        throw std::runtime_error("Could not parse frames");
                    }

                    startFrame = std::atoi(framesAttributeValue.substr(0, c).c_str());
                    endFrame = std::atoi(framesAttributeValue.substr(c + 1).c_str());
                }

                GraphicSetKeyState state = GraphicSetKeyState::NONE;
                if (std::strcmp(stateName, "walking") == 0) {
                    state = GraphicSetKeyState::WALKING;
                } else if (std::strcmp(stateName, "walking-with-goods") == 0) {
                    state = GraphicSetKeyState::WALKING_WITH_GOODS;
                } else if (std::strcmp(stateName, "eating") == 0) {
                    state = GraphicSetKeyState::EATING;
                } else if (std::strcmp(stateName, "eating-with-goods") == 0) {
                    state = GraphicSetKeyState::EATING_WITH_GOODS;
                }
                // TODO weitere Zustände bei Bedarf; nicht alle, weil wir nicht alle brauchen

                if (state == GraphicSetKeyState::NONE) {
                    Log::error(_("Illegal state '%s' for animation in graphicSet '%s'"), stateName, graphicSetName.c_str());
                    throw std::runtime_error("Illegal state for animation");
                }

                loadAnimationGraphicSet(graphicSet, imageFile, state, framesCount, startFrame, endFrame);
            }
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

void AbstractGraphicsMgr::loadAnimationGraphicSet(
    GraphicSet* graphicSet, const char* graphicFilename, const GraphicSetKeyState& state,
    int framesCount, int startFrame, int endFrame) {

    assert ((startFrame >= 0) && (startFrame <= endFrame) && (endFrame < framesCount));

    IGraphic* fullGraphic = loadGraphic(graphicFilename);

    int fullGraphicWidth = fullGraphic->getWidth();
    if (fullGraphicWidth % framesCount != 0) {
        Log::error(_("Could not divide the frames equally: '%s'"), graphicFilename);
        throw std::runtime_error("Could not divide the frames equally");
    }
    int frameWidth = fullGraphicWidth / framesCount;

    int fullGraphicHeight = fullGraphic->getHeight();
    if (fullGraphicHeight % 8 != 0) {
        Log::error(_("Could not divide the views equally: '%s'"), graphicFilename);
        throw std::runtime_error("Could not divide the views equally");
    }
    int frameHeight = fullGraphicHeight / 8;

    Rect frameRect(0, 0, frameWidth, frameHeight);

    forEachEighthDirection(view) {
        Animation* animation = new Animation(endFrame - startFrame + 1);

        frameRect.x = startFrame * frameWidth;
        for (int frameIndex = startFrame, i = 0; frameIndex <= endFrame; frameIndex++, i++, frameRect.x += frameWidth) {
            IGraphic* frameGraphic = loadGraphic(*fullGraphic, frameRect);
            animation->addFrame(i, frameGraphic);
        }

        graphicSet->addByStateAndView(state, view, animation);
        frameRect.y += frameHeight;
    }

    delete fullGraphic;
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
