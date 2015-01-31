#include <SDL.h>
#include <string>
#include "config/ConfigMgr.h"
#include "game/Colony.h"
#include "game/Game.h"
#include "graphics/graphic/IGraphic.h"
#include "graphics/renderer/sdl/SDLRenderer.h"
#include "gui/GuiMgr.h"
#include "gui/components/GuiMap.h"
#include "map/coords/MapCoords.h"
#include "map/coords/ScreenCoords.h"
#include "map/Map.h"
#include "utils/Consts.h"
#include "utils/StringFormat.h"

#ifdef DEBUG_A_STAR
#include "graphics/mgr/IFontMgr.h"
#endif


GuiMap::GuiMap(const Context* const context) : GuiBase(context) {
    setCoords(Consts::mapClipRect.x, Consts::mapClipRect.y, Consts::mapClipRect.w, Consts::mapClipRect.h);
}

GuiMap::~GuiMap() {
    if (mapObjectAlreadyDrawnThere != nullptr) {
        delete mapObjectAlreadyDrawnThere;
        mapObjectAlreadyDrawnThere = nullptr;
    }
}

void GuiMap::renderElement(IRenderer* renderer) {
    Map* map = context->game->getMap();
    const MapObject* selectedMapObject = map->getSelectedMapObject();
    const Building* selectedBuilding = dynamic_cast<const Building*>(selectedMapObject);

    int screenOffsetX = map->getScreenOffsetX();
    int screenOffsetY = map->getScreenOffsetY();
    int screenZoom = map->getScreenZoom();

    /*
     * Optimierung: Das Loopen über ALLE Kacheln ist teuer, weil wir jedes Mal die screenCoords ermitteln müssen,
     * bevor das Clipping greifen kann. Wir ermitteln die mapCoords in den Ecken der Karten-Zeichenfläche, um Start-
     * und End-Map-Koordinaten zu ermitteln. Damit gehen wir zwar immer noch über mehr Kacheln, als auf dem Bildschirm
     * sind, aber besser als nix :-)
     */
    // TODO Duplicate-Code refactorn, Rect (top, left, right, bottom) von Punkten einführen

    MapCoords mapCoordsTopLeft = MapCoordUtils::screenToMapCoords(ScreenCoords(
        screenOffsetX, screenOffsetY));
    MapCoords mapCoordsTopRight = MapCoordUtils::screenToMapCoords(ScreenCoords(
        (Consts::mapClipRect.w * screenZoom) + screenOffsetX, screenOffsetY));
    MapCoords mapCoordsBottomLeft = MapCoordUtils::screenToMapCoords(ScreenCoords(
        screenOffsetX, (Consts::mapClipRect.h * screenZoom) + screenOffsetY));
    MapCoords mapCoordsBottomRight = MapCoordUtils::screenToMapCoords(ScreenCoords(
        (Consts::mapClipRect.w * screenZoom) + screenOffsetX, (Consts::mapClipRect.h * screenZoom) + screenOffsetY));

    int mapXStart = std::max(mapCoordsTopLeft.x(), 0);
    int mapYStart = std::max(mapCoordsTopRight.y(), 0);
    int mapXEnd = std::min(mapCoordsBottomRight.x() + 1, (int) (map->getWidth() - 1)); // am Ende jeweils +1 rechnen, um der Elevation
    int mapYEnd = std::min(mapCoordsBottomLeft.y() + 1, (int) (map->getHeight() - 1)); // entgegen zu wirken. Ohne das würde unten was fehlen.

    // Nur die Kartenfläche vollmalen
    Rect sdlMapClipRect(Consts::mapClipRect);
    sdlMapClipRect.y = renderer->getWindowHeight() - (sdlMapClipRect.y + sdlMapClipRect.h); // SDL misst das Rechteck von UNTEN, kp, warum und ob das ein Bug is
    renderer->setClipRect(&sdlMapClipRect);

    // Kacheln rendern
    for (int mapY = mapYStart; mapY <= mapYEnd; mapY++) {
        for (int mapX = mapXStart; mapX <= mapXEnd; mapX++) {
            const MapCoords mapCoords = MapCoords(mapX, mapY);
            const MapTile* mapTile = map->getMapTileAt(mapCoords);
            const Animation* tileAnimation = mapTile->getTileAnimationForView(0); // TODO später Drehung der View
            const IGraphic* tileGraphic = tileAnimation->getGraphic();

            Rect rectDestination = MapCoordUtils::mapToDrawCoords(mapCoords, *map, 0, *tileGraphic);

            // Clipping
            if (rectDestination.x >= Consts::mapClipRect.x + Consts::mapClipRect.w ||
                rectDestination.y >= Consts::mapClipRect.y + Consts::mapClipRect.h ||
                rectDestination.x + rectDestination.w < Consts::mapClipRect.x ||
                rectDestination.y + rectDestination.h < Consts::mapClipRect.y) {

                continue;
            }

            int drawingFlags = 0;
            if (selectedMapObject != nullptr) {
                bool insideCatchmentArea =
                    (selectedBuilding != nullptr && selectedBuilding->isInsideCatchmentArea(context->configMgr, mapCoords));

                if (!insideCatchmentArea) {
                    drawingFlags |= IGraphic::DRAWING_FLAG_DARKENED;
                }
            }
            tileGraphic->draw(nullptr, &rectDestination, drawingFlags, context->sdlTicks);
        }
    }

    // Postionieren wir grade ein neues Gebäude?
    Structure* structureBeingAdded = nullptr;
    if (context->guiMgr->getPanelState().selectedPanelButton == PanelButton::ADD_BUILDING) {
        MapCoords mapCoords = MapCoordUtils::getMapCoordsUnderMouse(map, context->mouseCurrentX, context->mouseCurrentY);

        StructureType structureType = context->guiMgr->getPanelState().addingStructure;
        const FourDirectionsView& view = context->guiMgr->getPanelState().addingStructureView;
        unsigned char allowedToPlaceStructure = isAllowedToPlaceStructure(mapCoords, structureType, view);

        // Auf dem Ozean malen wir gar nix.
        // Is was im Weg, malen wir auch nicht. // TODO wir müssen eine flache Kachel malen, sonst sieht man ja gar nix
        if (!(allowedToPlaceStructure & (PLACING_STRUCTURE_OUTSIDE_OF_ISLE | PLACING_STRUCTURE_SOMETHING_IN_THE_WAY))) {
            const std::string& viewName = view.getViewName();
            const std::string graphicSetName = context->graphicsMgr->getGraphicSetNameForStructure(structureType);
            const GraphicSet* graphicSet = context->graphicsMgr->getGraphicSet(graphicSetName);
            const IGraphic* graphic = graphicSet->getByView(viewName)->getGraphic();

            int drawingFlags = IGraphic::DRAWING_FLAG_MASKED;
            if (allowedToPlaceStructure & PLACING_STRUCTURE_ROOM_NOT_UNLOCK) {
                drawingFlags |= IGraphic::DRAWING_FLAG_RED;
            }
            if (allowedToPlaceStructure & PLACING_STRUCTURE_NO_RESOURCES) {
                drawingFlags |= IGraphic::DRAWING_FLAG_BLINK;
            }

            // Zu zeichnendes Gebäude erstellen
            structureBeingAdded = new Structure();
            structureBeingAdded->setStructureType(structureType);
            structureBeingAdded->setMapCoords(mapCoords);
            structureBeingAdded->setMapWidth(graphic->getMapWidth());
            structureBeingAdded->setMapHeight(graphic->getMapHeight());
            structureBeingAdded->setDrawingFlags(drawingFlags);
            structureBeingAdded->setView(context->guiMgr->getPanelState().addingStructureView);
        }
    }

    // Objekte rendern
    memset(mapObjectAlreadyDrawnThere->data, 0,
        mapObjectAlreadyDrawnThere->width * mapObjectAlreadyDrawnThere->height * sizeof(char));

    // TODO Start und End noch ein wenig weiter ausweiten?
    for (int mapY = mapYStart; mapY <= mapYEnd; mapY++) {
        for (int mapX = mapXStart; mapX <= mapXEnd; mapX++) {
            const MapCoords mapCoords = MapCoords(mapX, mapY);
            const MapObjectFixed* mapObject = map->getMapObjectAt(mapCoords);
            if (mapObject == nullptr) {
                // Positionieren wir hier ein neues Gebäude?
                if (structureBeingAdded != nullptr) {
                    const MapCoords& mcoords = structureBeingAdded->getMapCoords();
                    int mw = structureBeingAdded->getMapWidth();
                    int mh = structureBeingAdded->getMapHeight();

                    if (mapX >= mcoords.x() && mapY >= mcoords.y() &&
                        mapX < mcoords.x() + mw && mapY < mcoords.y() + mh) {

                        mapObject = structureBeingAdded;
                    }
                }

                if (mapObject == nullptr) {
                    continue; // nix zum Zeichen an dieser Stelle
                }
            }

            if (mapObjectAlreadyDrawnThere->getData(mapX, mapY, 1) == 1) {
                continue; // hier is schon bemalt, nix zu tun
            }

            // @see docs/drawing-order-x-tiles.xcf für Variablen

            // Ausrechnen, welchen Schnibbel der Grafik wir anzeigen müssen
            const MapCoords& moMapCoords = mapObject->getMapCoords();
            int moMapWidth = mapObject->getMapWidth();
            int moMapHeight = mapObject->getMapHeight();

            int tileOffsetXInMapObject = mapX - moMapCoords.x(); // (0 ... moMapWidth-1)
            int tileOffsetYInMapObject = mapY - moMapCoords.y(); // (0 ... moMapHeight-1)

            const Structure* structure = dynamic_cast<const Structure*>(mapObject); // TODO nullptr sollte nicht passieren; später checken, wenn wir Bäume und sowas haben
            int drawingFlags = structure->getDrawingFlags();

            StructureType structureType = structure->getStructureType();

            // Sonderfall: Straße
            if (structureType == StructureType::STREET) {
                structureType = getConcreteStreetStructureType(mapCoords, structureType);
            }

            const std::string& viewName = structure->getView().getViewName();  // TODO view+rotation
            const std::string graphicSetName = context->graphicsMgr->getGraphicSetNameForStructure(structureType);
            const GraphicSet* mapObjectGraphicSet = context->graphicsMgr->getGraphicSet(graphicSetName);
            const IGraphic* mapObjectGraphic = mapObjectGraphicSet->getByView(viewName)->getGraphic();

            int xInMapObject =
                ((moMapHeight - 1) - tileOffsetYInMapObject + tileOffsetXInMapObject) * IGraphicsMgr::TILE_WIDTH_HALF;
            int yInMapObject = mapObjectGraphic->getHeight() -
                ((moMapHeight - 1) - tileOffsetYInMapObject + (moMapWidth - 1) - tileOffsetXInMapObject + 2) *
                    IGraphicsMgr::TILE_HEIGHT_HALF;

            Rect rectSource(xInMapObject, 0, IGraphicsMgr::TILE_WIDTH, mapObjectGraphic->getHeight());

            ScreenCoords screenCoords = MapCoordUtils::mapToScreenCoords(mapCoords);
            Rect rectDestination(screenCoords.x(), screenCoords.y(), rectSource.w / screenZoom, rectSource.h / screenZoom);

            rectDestination.x -= screenOffsetX;
            rectDestination.y -= screenOffsetY;

            rectDestination.y -= yInMapObject;

            const int elevation = 1; // TODO für Gebäude wie Anlegestelle, Fischerhütte etc. muss auf 0 gesetzt werden
            rectDestination.y -= elevation * IGraphicsMgr::ELEVATION_HEIGHT;

            rectDestination.x /= screenZoom;
            rectDestination.y /= screenZoom;

            if (selectedMapObject != nullptr) {
                bool insideCatchmentArea =
                    (selectedBuilding != nullptr && selectedBuilding->isInsideCatchmentArea(context->configMgr, *structure));

                if (!insideCatchmentArea) {
                    drawingFlags |= IGraphic::DRAWING_FLAG_DARKENED;
                }
            }
            mapObjectGraphic->draw(&rectSource, &rectDestination, drawingFlags, context->sdlTicks);

            // In mapObjectAlreadyDrawnThere die Kacheln-Spalte als erledigt markieren
            do {
                mapObjectAlreadyDrawnThere->setData(
                    moMapCoords.x() + tileOffsetXInMapObject, moMapCoords.y() + tileOffsetYInMapObject, 1);

                tileOffsetXInMapObject++;
                tileOffsetYInMapObject++;
            } while(tileOffsetXInMapObject < moMapWidth && tileOffsetYInMapObject < moMapHeight);
        }
    }

    // Träger zeichnen
    // TODO jetzt müssen wir doch wieder alle Map-Objekte durchloopen, weil die Träger nicht auf MapTile sind. Das sollte viel hübscher gemacht werden
    const std::list<MapObject*>& mapObjects = map->getMapObjects();
    for (auto iter = mapObjects.cbegin(); iter != mapObjects.cend(); iter++) {
        MapObject* mapObject = *iter;
        Building* building = dynamic_cast<Building*>(mapObject);
        if (building == nullptr) {
            continue;
        }

        Carrier* carrier = building->carrier;
        if (carrier == nullptr) {
            continue;
        }

        const DoubleMapCoords& mapCoords = carrier->getMapCoords();
        const Animation* animation = carrier->getAnimation();
        const IGraphic* animationCurrentFrame = animation->getFrame((unsigned int) carrier->animationFrame);

        Rect rect = MapCoordUtils::mapToDrawCoords(mapCoords, *map, 1, *animationCurrentFrame);

        animationCurrentFrame->drawScaledAt(rect.x, rect.y, (double) 1 / (double) screenZoom);
    }

    // structureBeingAdded gesetzt?
    if (structureBeingAdded != nullptr) {
        // Einzugsbereich jetzt malen, damit er oben drauf is
        drawCatchmentArea(renderer, structureBeingAdded);
        delete structureBeingAdded;
    }

#ifdef DEBUG_A_STAR
    Route* routeToDraw = AStar::debugAStar_route;

    // Wenn Gebäude ausgewählt -> Trägerroute anzeigen
    if (selectedBuilding != nullptr) {
        routeToDraw = (selectedBuilding->carrier != nullptr) ? selectedBuilding->carrier->route : nullptr;
    }

    // A*-Route zeichnen (nur bei Maximalzoom, dann sparen wir uns Berechnungen und der Code wird einfacher)
    if (screenZoom == 1 && routeToDraw != nullptr) {
        renderer->setDrawBlendMode(IRenderer::BLENDMODE_BLEND);

        int lastPointX = -1;
        int lastPointY = -1;

        int i = 1;
        for (auto iter = routeToDraw->cbegin(); iter != routeToDraw->cend(); iter++) {
            const MapCoords& mapCoords = *iter;

            ScreenCoords screenCoords = MapCoordUtils::mapToScreenCoordsCenter(mapCoords);

            screenCoords.subX(screenOffsetX);
            screenCoords.subY(screenOffsetY);

            screenCoords.subY(IGraphicsMgr::ELEVATION_HEIGHT);

            // Verbindungslinie zuerst
            if (lastPointX != -1) {
                renderer->setDrawColor(Color(255, 0, 0, 128));
                renderer->drawLine(lastPointX, lastPointY, screenCoords.x(), screenCoords.y());
            }
            lastPointX = screenCoords.x();
            lastPointY = screenCoords.y();

            // dann Rechteck mit Zahl drin
            Rect rect(screenCoords.x() - 8, screenCoords.y() - 8, 16, 16);
            renderer->setDrawColor(Color(0, 0, 255, 128));
            renderer->fillRect(rect);

            static Color colorWhite = Color(255, 255, 255, 255);
            context->fontMgr->renderText(renderer, toString(i++), screenCoords.x(), screenCoords.y(), &colorWhite, nullptr,
                "DroidSans.ttf", 9, RENDERTEXT_HALIGN_CENTER | RENDERTEXT_VALIGN_MIDDLE);
        }
    }
#endif

    // Clipping wieder zurücksetzen, bevor der nächste mit Malen drankommt
    renderer->setClipRect(nullptr);

    // TODO nicht die Konstante benutzen für die Größe/Position der Karte, sondern die GUI-Properties,
    // damit die GUI-Komponente auch mal verschoben werden kann; ohne alle Stellen zu ändern;
    // nur der Konstruktor sollte die Konstante benutzen
}

void GuiMap::onEventElement(SDL_Event& event) {
    int startClickX, startClickY;
    context->guiMgr->getStartClickCoords(startClickX, startClickY);

    // Maustaste in der Karte geklickt
    if (event.type == SDL_MOUSEBUTTONUP &&
        event.button.button == SDL_BUTTON_LEFT &&
        hitTest(event.button.x, event.button.y)) {

        if (abs(event.button.x - startClickX) < 3 && abs(event.button.y - startClickY) < 3) {
            onClickInMap(event.button.x, event.button.y);
        }
    }
}

void GuiMap::onClickInMap(int mouseX, int mouseY) {
    Map* map = context->game->getMap();
    int screenZoom = map->getScreenZoom();

    // Grade beim Platzieren eines neuen Gebäudes?
    if (context->guiMgr->getPanelState().selectedPanelButton == PanelButton::ADD_BUILDING) {
        MapCoords mapCoords = MapCoordUtils::getMapCoordsUnderMouse(map, context->mouseCurrentX, context->mouseCurrentY);

        onClickInMapWhileAddingStructure(mapCoords);
        return;
    }

    // Gucken, ob ein Gebäude geklickt wurde.
    // Objekte dabei rückwärts iterieren. Somit kommen "oben liegende" Objekte zuerst dran.
    // TODO Überarbeiten, da wir keine Reihenfolge mehr auf den Objekten haben!
    const std::list<MapObject*>& mapObjects = map->getMapObjects();
    for (auto iter = mapObjects.crbegin(); iter != mapObjects.crend(); iter++) {
        MapObject* mapObject = *iter;

        // TODO hier später weitere Typen handeln oder cleverer in Objekt-Methoden arbeiten
        Building* building = dynamic_cast<Building*>(mapObject);
        if (building == nullptr) {
            // Da wir nur die Buildings durchgehen und nicht alle Structures, haben wir den positiven Nebeneffekt,
            // dass wir z.B. durch eine Mauer durchklicken und ein verstecktes Gebäude dahinter anklicken können.
            continue;
        }

        Rect rect = MapCoordUtils::getDrawCoordsForBuilding(*map, context->graphicsMgr, building);

        // Außerhalb der Boundary-Box des Objekt geklickt?
        // TODO für Gebäude mit elevation=0 ggf. anpassen
        if (mouseX < rect.x) {
            continue;
        }
        if (mouseX >= rect.x + rect.w) {
            continue;
        }
        if (mouseY < rect.y) {
            continue;
        }
        if (mouseY >= rect.y + rect.h) {
            continue;
        }

        // Pixelfarbwert holen
        uint8_t r, g, b, a;
        int x = (mouseX - rect.x) * screenZoom;
        int y = (mouseY - rect.y) * screenZoom;

        const std::string& viewName = building->getView().getViewName();
        const std::string graphicSetName = context->graphicsMgr->getGraphicSetNameForStructure(building->getStructureType());
        const GraphicSet* graphicSet = context->graphicsMgr->getGraphicSet(graphicSetName);
        graphicSet->getByView(viewName)->getGraphic()->getPixel(x, y, &r, &g, &b, &a);

        // Checken, ob Pixel un-transparent genug ist, um es als Treffer zu nehmen
        if (a > 127) {
            context->game->setSelectedMapObject(building);
            return;
        }
    }

    // TODO später ggf. weitere Events
    context->game->setSelectedMapObject(nullptr);
}

void GuiMap::onClickInMapWhileAddingStructure(const MapCoords& mapCoords) {
    const FourDirectionsView& view = context->guiMgr->getPanelState().addingStructureView;
    StructureType structureType = context->guiMgr->getPanelState().addingStructure;
    if (isAllowedToPlaceStructure(mapCoords, structureType, view) != PLACING_STRUCTURE_ALLOWED) {
        // Dürfen wir hier/jetzt nicht setzen, ignorieren wir den Klick
        return;
    }

    // Gebäude platzieren und Modus verlassen
    Player* currentPlayer = context->game->getCurrentPlayer();

    // Sonderfall Haus: Wir wählen zufällig eins aus
    if (structureType == StructureType::PIONEERS_HOUSE1) {
        std::random_device randomDevice;
        std::default_random_engine randomEngine(randomDevice());
        std::uniform_int_distribution<int> randomPioneerHouse(
            StructureType::PIONEERS_HOUSE1, StructureType::PIONEERS_HOUSE5);

        structureType = (StructureType) randomPioneerHouse(randomEngine);
    }

    context->game->addStructure(mapCoords, structureType, view, currentPlayer);


    // Resourcen bezahlen
    Colony* colony = context->game->getColony(
        currentPlayer, context->game->getMap()->getMapTileAt(mapCoords)->isle);
    const BuildingCosts* buildingCosts = context->configMgr->getBuildingConfig(structureType)->getBuildingCosts();
    currentPlayer->coins -= buildingCosts->coins;
    colony->subtractBuildingCosts(buildingCosts);
}

unsigned char GuiMap::isAllowedToPlaceStructure(
    const MapCoords& mapCoords, StructureType structureType, const FourDirectionsView& view) const {

    MapTile* mapTile = context->game->getMap()->getMapTileAt(mapCoords);
    if (mapTile == nullptr) {
        return PLACING_STRUCTURE_OUTSIDE_OF_ISLE;
    }

    Isle* isle = mapTile->isle;
    if (isle == nullptr) {
        return PLACING_STRUCTURE_OUTSIDE_OF_ISLE;
    }

    unsigned char result = PLACING_STRUCTURE_ALLOWED;

    // Resourcen checken
    Player* currentPlayer = context->game->getCurrentPlayer();
    Colony* colony = context->game->getColony(currentPlayer, isle);

    if (colony != nullptr) {
        const BuildingCosts* buildingCosts = context->configMgr->getBuildingConfig(structureType)->getBuildingCosts();
        if ((buildingCosts->coins > currentPlayer->coins) ||
            (buildingCosts->tools > colony->getGoods(GoodsType::TOOLS).inventory) ||
            (buildingCosts->wood > colony->getGoods(GoodsType::WOOD).inventory) ||
            (buildingCosts->bricks > colony->getGoods(GoodsType::BRICKS).inventory)) {
            result |= PLACING_STRUCTURE_NO_RESOURCES;
        }
    }

    // Checken, ob alles frei is, um das Gebäude zu setzen
    const std::string& viewName = context->guiMgr->getPanelState().addingStructureView.getViewName();
    const std::string graphicSetName = context->graphicsMgr->getGraphicSetNameForStructure(structureType);
    const GraphicSet* graphicSet = context->graphicsMgr->getGraphicSet(graphicSetName);
    const IGraphic* graphic = graphicSet->getByView(viewName)->getGraphic();
    for (int y = mapCoords.y(); y < mapCoords.y() + graphic->getMapHeight(); y++) {
        for (int x = mapCoords.x(); x < mapCoords.x() + graphic->getMapWidth(); x++) {
            MapTile* mapTile = context->game->getMap()->getMapTileAt(MapCoords(x, y));

            if (mapTile->mapObject != nullptr) {                  // Da steht was im Weg
                result |= PLACING_STRUCTURE_SOMETHING_IN_THE_WAY;
                return result;
            }

            const MapTileConfig* mapTileConfig = mapTile->getMapTileConfig();

            if (mapTile->player == nullptr ||                     // Gebiet nicht erschlossen, ..,
                mapTile->player != context->game->getCurrentPlayer() ||    // ... nicht unseres...
                !mapTileConfig->isWalkableAndBuildable) {         // ... oder Gelände passt nicht

                result |= PLACING_STRUCTURE_ROOM_NOT_UNLOCK;
                return result;
            }
        }
    }

    return result;
}

void GuiMap::drawCatchmentArea(IRenderer* const renderer, Structure* structure) {
    Map* map = context->game->getMap();
    int screenOffsetX = map->getScreenOffsetX();
    int screenOffsetY = map->getScreenOffsetY();
    int screenZoom = map->getScreenZoom();

    renderer->setDrawColor(Color(0xc8, 0xaf, 0x37, 255));

    const BuildingConfig* buildingConfig = context->configMgr->getBuildingConfig(structure->getStructureType());
    const RectangleData<char>* catchmentArea = buildingConfig->getCatchmentArea();
    if (catchmentArea != nullptr) {
        for (int y = 0; y < catchmentArea->height; y++) {
            for (int x = 0; x < catchmentArea->width; x++) {
                const MapCoords& mapCoords = structure->getMapCoords();
                int mapWidth = structure->getMapWidth();
                int mapHeight = structure->getMapHeight();

                int mapX = mapCoords.x() + (x - (catchmentArea->width - mapWidth) / 2);
                int mapY = mapCoords.y() + (y - (catchmentArea->height - mapHeight) / 2);

                ScreenCoords screenCoords = MapCoordUtils::mapToScreenCoords(MapCoords(mapX, mapY));

                int drawX = (screenCoords.x() - screenOffsetX) / screenZoom;
                int drawY = (screenCoords.y() - screenOffsetY - IGraphicsMgr::ELEVATION_HEIGHT) / screenZoom;

                // An der Kachel jede der 4 Seiten untersuchen, ob wir eine Linie malen müssen.
                // TODO die String-'1'er ersetzen durch echte 1en.

                // Oben rechts
                if (catchmentArea->getData(x, y - 1, '0') == '0' && catchmentArea->getData(x, y, '0') == '1') {
                    renderer->drawLine(
                        drawX + IGraphicsMgr::TILE_WIDTH_HALF / screenZoom, drawY,
                        drawX + IGraphicsMgr::TILE_WIDTH / screenZoom,
                        drawY + IGraphicsMgr::TILE_HEIGHT_HALF / screenZoom);
                }

                // Oben links
                if (catchmentArea->getData(x - 1, y, '0') == '0' && catchmentArea->getData(x, y, '0') == '1') {
                    renderer->drawLine(
                        drawX, drawY + IGraphicsMgr::TILE_HEIGHT_HALF / screenZoom,
                        drawX + IGraphicsMgr::TILE_WIDTH_HALF / screenZoom, drawY);
                }

                // Unten rechts
                if (catchmentArea->getData(x, y, '0') == '1' && catchmentArea->getData(x + 1, y, '0') == '0') {
                    renderer->drawLine(
                        drawX + IGraphicsMgr::TILE_WIDTH_HALF / screenZoom,
                        drawY + IGraphicsMgr::TILE_HEIGHT / screenZoom,
                        drawX + IGraphicsMgr::TILE_WIDTH / screenZoom,
                        drawY + IGraphicsMgr::TILE_HEIGHT_HALF / screenZoom);
                }

                // Unten links
                if (catchmentArea->getData(x, y, '0') == '1' && catchmentArea->getData(x, y + 1, '0') == '0') {
                    renderer->drawLine(
                        drawX, drawY + IGraphicsMgr::TILE_HEIGHT_HALF / screenZoom,
                        drawX + IGraphicsMgr::TILE_WIDTH_HALF / screenZoom,
                        drawY + IGraphicsMgr::TILE_HEIGHT / screenZoom);
                }
            }
        }
    }
}

StructureType GuiMap::getConcreteStreetStructureType(
    const MapCoords& mapCoords, StructureType abstractStreetStructureType) const {

    if (abstractStreetStructureType == StructureType::STREET) {
        static const StructureType structureTypeMap[16] = {
            STREET_STRAIGHT_90, // oder STREET_STRAIGHT_0, is in diesem Fall egal. TODO Sollte später mit der Drehfunktion genauer untersucht werden
            STREET_STRAIGHT_90,
            STREET_STRAIGHT_0,
            STREET_CURVE_0,
            STREET_STRAIGHT_0,
            STREET_CURVE_270,
            STREET_STRAIGHT_0,
            STREET_TEE_90,
            STREET_STRAIGHT_90,
            STREET_STRAIGHT_90,
            STREET_CURVE_90,
            STREET_TEE_180,
            STREET_CURVE_180,
            STREET_TEE_0,
            STREET_TEE_270,
            STREET_CROSS
        };

        Map* map = context->game->getMap();
        char isStreetAbove = map->isStreetAt(MapCoords(mapCoords.x(), mapCoords.y() - 1)) ? 1 : 0; // Bit 0
        char isStreetRight = map->isStreetAt(MapCoords(mapCoords.x() + 1, mapCoords.y())) ? 1 : 0; // Bit 1
        char isStreetLeft = map->isStreetAt(MapCoords(mapCoords.x() - 1, mapCoords.y())) ? 1 : 0;  // Bit 2
        char isStreetBelow = map->isStreetAt(MapCoords(mapCoords.x(), mapCoords.y() + 1)) ? 1 : 0; // Bit 3

        int index = (isStreetBelow << 3) | (isStreetLeft << 2) | (isStreetRight << 1) | isStreetAbove;

        return structureTypeMap[index];
    }

    // TODO Feldweg
    throw new std::runtime_error("Illegal abstractStreetStructureType " + toString(abstractStreetStructureType));
}

void GuiMap::onNewGame() {
    Map* map = context->game->getMap();
    int newMapWidth = map->getWidth();
    int newMapHeight = map->getHeight();

    // mapObjectAlreadyDrawnThere wegräumen
    if (mapObjectAlreadyDrawnThere != nullptr) {
        delete mapObjectAlreadyDrawnThere;
        mapObjectAlreadyDrawnThere = nullptr;
    }

    // mapObjectAlreadyDrawnThere neu anlegen, brauchen wir fürs Zeichnen
    mapObjectAlreadyDrawnThere = new RectangleData<char>(newMapWidth, newMapHeight);
}
