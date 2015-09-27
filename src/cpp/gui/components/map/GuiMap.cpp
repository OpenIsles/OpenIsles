#include <SDL.h>
#include <algorithm>
#include <cassert>
#include <string>
#include "config/ConfigMgr.h"
#include "game/CatchmentArea.h"
#include "game/Colony.h"
#include "game/Game.h"
#include "graphics/graphic/IGraphic.h"
#include "graphics/renderer/sdl/SDLRenderer.h"
#include "gui/components/map/GuiMap.h"
#include "gui/components/map/GuiResourcesBar.h"
#include "gui/GuiMgr.h"
#include "gui/Identifiers.h"
#include "map/coords/MapCoords.h"
#include "map/coords/ScreenCoords.h"
#include "map/Map.h"
#include "map/Street.h"
#include "pathfinding/AStar.h"
#include "utils/Consts.h"
#include "utils/Events.h"
#include "utils/MapCoordsIterator.h"
#include "utils/StringFormat.h"

#ifdef DEBUG_GUIMAP_COORDS
#include <sstream>
#endif

#if defined(DEBUG_A_STAR) || defined(DEBUG_GUIMAP_COORDS)
#include <vector>
#include "graphics/mgr/IFontMgr.h"
#endif


GuiMap::GuiMap(const Context* const context, GuiResourcesBar* guiResourcesBar) :
    GuiBase(context), guiResourcesBar(guiResourcesBar) {

    setCoords(Consts::mapClipRect.x, Consts::mapClipRect.y, Consts::mapClipRect.w, Consts::mapClipRect.h);

    toolsGood = context->configMgr->getGood("tools");
    woodGood = context->configMgr->getGood("wood");
    bricksGood = context->configMgr->getGood("bricks");

#ifdef DEBUG_GUIMAP
    debugGridOverlayGraphics[0] = new SDLGraphic(
        context->graphicsMgr->getRenderer(), "data/debug-grid-overlay-elevation0.png");
    debugGridOverlayGraphics[2] = new SDLGraphic(
        context->graphicsMgr->getRenderer(), "data/debug-grid-overlay-screencoords.png");
#endif
#if defined(DEBUG_GUIMAP_COORDS) || defined(DEBUG_GUIMAP)
    debugGridOverlayGraphics[1] = new SDLGraphic(
        context->graphicsMgr->getRenderer(), "data/debug-grid-overlay-elevation1.png");
#endif
}

GuiMap::~GuiMap() {
#ifdef DEBUG_GUIMAP
    delete debugGridOverlayGraphics[0];
    delete debugGridOverlayGraphics[1];
#endif
#ifdef DEBUG_GUIMAP_COORDS
    delete debugGridOverlayGraphics[2];
#endif
}

void GuiMap::renderElement(IRenderer* renderer) {
    Map* map = context->game->getMap();

    /*
     * Optimierung: Das Loopen über ALLE Kacheln ist teuer, weil wir jedes Mal die screenCoords ermitteln müssen,
     * bevor das Clipping greifen kann. Wir ermitteln die mapCoords in den Ecken der Karten-Zeichenfläche, um Start-
     * und End-Map-Koordinaten zu ermitteln. Damit gehen wir zwar immer noch über mehr Kacheln, als auf dem Bildschirm
     * sind, aber besser als nix :-)
     */
    MapCoords mapCoordsTopLeft, mapCoordsTopRight, mapCoordsBottomLeft, mapCoordsBottomRight;
    MapCoordUtils::getMapCoordsInScreenEdges(
        *map, mapCoordsTopLeft, mapCoordsTopRight, mapCoordsBottomLeft, mapCoordsBottomRight);


    // orthogonales Rechteck zum MapCoord-Koordinatensystem für den MapCoordsIterator bauen:
    /*
     *             ----                       |------====-------
     *         ----  ----                     |  ----  ----    |
     *     ----        ----                   |---        ---- |
     *       ----        ----      ---->      | ----        ---|
     *         ----  ----                     |   ----  ----   |
     *           ----                         ------====-------|
     *
     */
    int mapXStart = std::min({
        mapCoordsTopLeft.x(), mapCoordsTopRight.x(), mapCoordsBottomLeft.x(), mapCoordsBottomRight.x()
    });
    int mapYStart = std::min({
        mapCoordsTopLeft.y(), mapCoordsTopRight.y(), mapCoordsBottomLeft.y(), mapCoordsBottomRight.y()
    });
    int mapXEnd = std::max({
        mapCoordsTopLeft.x(), mapCoordsTopRight.x(), mapCoordsBottomLeft.x(), mapCoordsBottomRight.x()
    });
    int mapYEnd = std::max({
        mapCoordsTopLeft.y(), mapCoordsTopRight.y(), mapCoordsBottomLeft.y(), mapCoordsBottomRight.y()
    });

    // Wir rechnen plus/minus 2, damit sicher alles drauf is (große Gebäude, Elevation).
    // Außerdem stellen wir sicher, dass wir nicht außerhalb der Karte landen.
    // TODO später nochmal probieren, wenn n Riesengebäude, wie z.B. ne Kathedrale drauf is
    const int extraRadius = 2;
    mapXStart = std::max(mapXStart - extraRadius, 0);
    mapYStart = std::max(mapYStart - extraRadius, 0);
    mapXEnd = std::min(mapXEnd + extraRadius, map->getWidth() - 1);
    mapYEnd = std::min(mapYEnd + extraRadius, map->getHeight() - 1);

    const FourthDirection& screenView = map->getScreenView();
    MapCoordsIterator mapCoordsIterator(MapCoords(mapXStart, mapYStart), MapCoords(mapXEnd, mapYEnd), screenView);

    // Nur die Kartenfläche vollmalen
    renderer->setClipRect(&Consts::mapClipRect);

    if (buildOperation) {
        // TODO kann man optimieren, indem wirklich nur bei Änderung der Resourcen (Geld oder Baumaterial der betreffenden Kolonie) aufruft.
        // Allerdings macht das der Carrier und der weiß nix von Colony oder GUI. Also erstmal so.
        buildOperation->updateBuildMaterials();
    }

    // Jetzt rendern
    mapCoordsIterator.iterate([&] (const MapCoords& mapCoords) {
        renderTile(mapCoords);
    });

    // Einzugsbereich jetzt malen, damit er oben drauf is
    if (buildOperation) {
        const std::list<MapObjectToBuild>& mapObjectsToBuild = buildOperation->getMapObjectsToBuild();
        // nur zeichnen, wenn genau ein Objekt in der Buildqueue is.
        if (mapObjectsToBuild.size() == 1) {
            const MapObjectToBuild& firstMapObjectToBuild = mapObjectsToBuild.front();
            drawCatchmentArea(renderer, firstMapObjectToBuild);
        }
    }

#ifdef DEBUG_A_STAR
    std::vector<const Route*> routesToDraw;

    // Wenn Gebäude ausgewählt -> Trägerroute anzeigen
    const MapObject* selectedMapObject = map->getSelectedMapObject();
    const Building* selectedBuilding = dynamic_cast<const Building*>(selectedMapObject);
    if (selectedBuilding != nullptr) {
        for (const Carrier* carrier : selectedBuilding->carriers) {
            if (carrier->route.routeExists()) {
                routesToDraw.push_back(&carrier->route);
            }
        }
    } else {
        if (AStar::debugAStar_route.routeExists()) {
            routesToDraw.push_back(&AStar::debugAStar_route);
        }
    }

    // A*-Routen zeichnen (nur bei Maximalzoom, dann sparen wir uns Berechnungen und der Code wird einfacher)
    if (map->getScreenZoom() == 1 && !routesToDraw.empty()) {
        renderer->setDrawBlendMode(IRenderer::BLENDMODE_BLEND);

        static const Color colorWhite = Color(255, 255, 255, 92);
        static const Color colors[3] = {
            Color(0, 0, 255, 92),
            Color(0, 192, 0, 92),
            Color(255, 0, 0, 92)
        };

        int routeIndex = 0;
        for (const Route* routeToDraw : routesToDraw) {
            int lastPointX = -1;
            int lastPointY = -1;

            const Color& color = colors[routeIndex++ % 3];

            int hopIndex = 1;
            for (auto iter = routeToDraw->cbegin(); iter != routeToDraw->cend(); iter++) {
                const MapCoords& mapCoords = *iter;

                ScreenCoords screenCoords = MapCoordUtils::mapToScreenCoords(mapCoords, screenView, *map);
                screenCoords.addY(IGraphicsMgr::TILE_HEIGHT_HALF);
                screenCoords.subY(IGraphicsMgr::ELEVATION_HEIGHT);

                screenCoords.addX(Consts::mapClipRect.w / 2);
                screenCoords.addY(Consts::mapClipRect.h / 2);

                // Verbindungslinie zuerst
                if (lastPointX != -1) {
                    renderer->setDrawColor(color);
                    renderer->drawLine(lastPointX, lastPointY, screenCoords.x(), screenCoords.y());
                }
                lastPointX = screenCoords.x();
                lastPointY = screenCoords.y();

                // dann Rechteck mit Zahl drin
                Rect rect(screenCoords.x() - 8, screenCoords.y() - 8, 16, 16);
                renderer->setDrawColor(color);
                renderer->fillRect(rect);

                context->fontMgr->renderText(renderer, toString(hopIndex++), screenCoords.x(), screenCoords.y(),
                                             &colorWhite, nullptr,
                                             "DroidSans.ttf", 9, RENDERTEXT_HALIGN_CENTER | RENDERTEXT_VALIGN_MIDDLE);
            }
        }
    }
#endif

#if defined(DEBUG_GUIMAP_COORDS) && !defined(DEBUG_GUIMAP)
    debugGridOverlayGraphics[1]->drawAt(0, 0);
#endif
#ifdef DEBUG_GUIMAP
    // Grid zeichnen, an dem wir uns orientieren können
    debugGridOverlayGraphics[0]->drawAt(0, 0);
    debugGridOverlayGraphics[1]->drawAt(0, 0);
    debugGridOverlayGraphics[2]->drawAt(0, 0);

    // Zusätzlich die Mitte markieren. Die sollte sich genau im ScreenCoords(0, 0)-Punkt decken
    int x = Consts::mapClipRect.w / 2;
    int y = Consts::mapClipRect.h / 2;
    renderer->drawLine(x-10, y, x+10, y);
    renderer->drawLine(x, y-10, x, y+10);

    // getDrawCoordsForBuilding()-Rechteck einzeichnen
    const std::list<MapObject*>& mapObjects = map->getMapObjects();
    for (auto iter = mapObjects.crbegin(); iter != mapObjects.crend(); iter++) {
        MapObject* mapObject = *iter;

        Building* building = dynamic_cast<Building*>(mapObject);
        if (building != nullptr) {
            Rect rect = MapCoordUtils::getDrawCoordsForMapObjectFixed(*map, context->graphicsMgr, building);
            context->graphicsMgr->getRenderer()->drawLine(rect.x, rect.y, rect.x, rect.y+rect.h);
            context->graphicsMgr->getRenderer()->drawLine(rect.x, rect.y, rect.x+rect.w, rect.y);
            context->graphicsMgr->getRenderer()->drawLine(rect.x+rect.w, rect.y, rect.x+rect.w, rect.y+rect.h);
            context->graphicsMgr->getRenderer()->drawLine(rect.x, rect.y+rect.h, rect.x+rect.w, rect.y+rect.h);
        }
    }
#endif

    // Clipping wieder zurücksetzen, bevor der nächste mit Malen drankommt
    renderer->setClipRect(nullptr);

    // TODO nicht die Konstante benutzen für die Größe/Position der Karte, sondern die GUI-Properties,
    // damit die GUI-Komponente auch mal verschoben werden kann; ohne alle Stellen zu ändern;
    // nur der Konstruktor sollte die Konstante benutzen
}

void GuiMap::renderTile(const MapCoords& mapCoords) {
    const Map* map = context->game->getMap();

    const MapTile* mapTile = map->getMapTileAt(mapCoords);
    if (mapTile == nullptr) {
        return;
    }

    // Kachel rendern //////////////////////////////////////////////////////////////////////////

    const FourthDirection& screenView = map->getScreenView();
    const Animation* tileAnimation = mapTile->getTileAnimationForView(screenView);
    const IGraphic* tileGraphic = tileAnimation->getGraphic();

    Rect rectDestination = MapCoordUtils::mapToDrawCoords(mapCoords, *map, 0, *tileGraphic, 1, 1);

    // Clipping
    if (rectDestination.x >= Consts::mapClipRect.x + Consts::mapClipRect.w ||
        rectDestination.y >= Consts::mapClipRect.y + Consts::mapClipRect.h ||
        rectDestination.x + rectDestination.w < Consts::mapClipRect.x ||
        rectDestination.y + rectDestination.h < Consts::mapClipRect.y) {

        return;
    }

    const MapObject* selectedMapObject = map->getSelectedMapObject();
    const Building* selectedBuilding = dynamic_cast<const Building*>(selectedMapObject);
    int screenZoom = map->getScreenZoom();

    int drawingFlags = 0;
    if (selectedMapObject != nullptr) {
        bool insideCatchmentArea =
            (selectedBuilding != nullptr && CatchmentArea::isInsideCatchmentArea(*selectedBuilding, mapCoords));

        if (!insideCatchmentArea) {
            drawingFlags |= IGraphic::DRAWING_FLAG_DARKENED;
        }
    }

    const BuildOperationResultBit* buildOperationResultBit = nullptr;
    if (buildOperation) {
        const BuildOperationResult& buildOperationResult = buildOperation->getResult();
        auto iterBuildOperationResultBit = buildOperationResult.find(mapCoords);
        if (iterBuildOperationResultBit != buildOperationResult.cend()) {
            buildOperationResultBit = iterBuildOperationResultBit->second.get();
        }
    }

    if (buildOperationResultBit) {
        if (buildOperationResultBit->somethingInTheWay) {
            drawingFlags |= IGraphic::DRAWING_FLAG_MASKED;
        }
    }

#ifdef DEBUG_GUIMAP
    // Kachel unter dem Mauszeiger rot färben
    if (mapCoords == MapCoordUtils::getMapCoordsUnderMouse(*map, context->mouseCurrentX, context->mouseCurrentY)) {
        drawingFlags |= IGraphic::DRAWING_FLAG_RED;
    }
#endif

    tileGraphic->draw(nullptr, &rectDestination, drawingFlags);


    // MapObjekt auf die Kachel rendern ////////////////////////////////////////////////////////

    // Was sagt buildOperationResult, was zu zeichnen is?
    const MapObjectFixed* mapObjectInBuildOperationResult = nullptr;
    if (buildOperationResultBit) {
        mapObjectInBuildOperationResult = buildOperationResultBit->mapObjectToDraw.get();
    }

    // Was sagt die Karte, was zu zeichnen is?
    const MapObjectFixed* mapObjectInMap = nullptr;
    if (mapTile->mapObjectFixed != nullptr) {
        mapObjectInMap = mapTile->mapObjectFixed;
    }

    // Gucken, ob/welches Objekt wir hier wirklich malen müssen:
    enum {
        FROM_MAP,
        FROM_BUILD_OPERATION_RESULT
    } drawWhat;

    if (mapObjectInBuildOperationResult == nullptr) {
        drawWhat = FROM_MAP;
    } else {
        // Wenn wir bauen, die Resourcen aber nicht reichen, blinkt das Objekt aus der buildOperationResult.
        // Es alterniert mit dem Objekt aus der Map (das kann auch nullptr sein, wenn auf der Map nix is)
        if (!buildOperationResultBit->costsNothingBecauseOfChange &&
            !buildOperationResultBit->resourcesEnoughToBuildThis) {

            drawWhat = (SDL_GetTicks() % 800 < 400) ? FROM_BUILD_OPERATION_RESULT : FROM_MAP;
        } else {
            drawWhat = FROM_BUILD_OPERATION_RESULT;
        }
    }

    const MapObjectFixed* mapObjectToDrawHere =
        (drawWhat == FROM_BUILD_OPERATION_RESULT) ? mapObjectInBuildOperationResult : mapObjectInMap;


    if (mapObjectToDrawHere != nullptr) {
        // @see docs/drawing-order-x-tiles.xcf für Variablen
        // TODO Vorsicht: doc/drawing-order-x-tiles.xcf ist veraltet -> mapObjectAlreadyDrawnThere gibts nicht mehr

        const MapCoords& moMapCoords = mapObjectToDrawHere->getMapCoords();
        const Structure* structure = dynamic_cast<const Structure*>(mapObjectToDrawHere);
        const Street* street = dynamic_cast<const Street*>(mapObjectToDrawHere);
        const Harvestable* harvestable = dynamic_cast<const Harvestable*>(mapObjectToDrawHere);

        // Ausrechnen, welchen Schnibbel der Grafik wir anzeigen müssen
        int moMapWidth = mapObjectToDrawHere->getMapWidth();
        int moMapHeight = mapObjectToDrawHere->getMapHeight();

        int tileOffsetXInMapObject = mapCoords.x() - moMapCoords.x(); // (0 ... moMapWidth-1)
        int tileOffsetYInMapObject = mapCoords.y() - moMapCoords.y(); // (0 ... moMapHeight-1)

        int drawingFlags = mapObjectToDrawHere->getDrawingFlags();
        const MapObjectType* mapObjectType = mapObjectToDrawHere->getMapObjectType();

        if (drawWhat == FROM_BUILD_OPERATION_RESULT && !buildOperationResultBit->costsNothingBecauseOfChange) {
            // Maskiert zeichnen,
            // - das neu zu platzierende Objekt
            // - ein bereits bestehendes Objekt: nur Strukturen/Harvestables maskiert zeigen, keine Gebäude
            if ((mapObjectType->type != MapObjectTypeClass::BUILDING) ||
                !buildOperationResultBit->somethingInTheWay) {

                drawingFlags = IGraphic::DRAWING_FLAG_MASKED;
            }
        }

        const FourthDirection& view = mapObjectToDrawHere->getView();
        const FourthDirection& viewToRender = Direction::addDirections(view, screenView);

        const std::string& graphicSetName = context->graphicsMgr->getGraphicSetNameForMapObject(mapObjectType);
        const GraphicSet* mapObjectGraphicSet = context->graphicsMgr->getGraphicSet(graphicSetName);

        // TODO duplicate code
        const IGraphic* graphicToDrawHere;
        if (harvestable != nullptr) {
            const std::string stateToRender = "growth" + toString(int(harvestable->getAge()));
            graphicToDrawHere = mapObjectGraphicSet->getByStateAndView(stateToRender, viewToRender)->getGraphic();
        }
        else if (street != nullptr) {
            const std::string stateToRender = street->getStateToRender();
            graphicToDrawHere = mapObjectGraphicSet->getByStateAndView(stateToRender, viewToRender)->getGraphic();
        }
        else if (structure != nullptr) {
            graphicToDrawHere = mapObjectGraphicSet->getByView(viewToRender)->getGraphic();
        }
        else {
            assert(false);
            return;
        }

        /* Tricky part: Die Berechnung von xInMapObject und yInMapObject in allen Ansichten.
         *
         * Um die Berechnung zu vereinfachen, führen wir yInMapObjectFromBottom ein. Das ist yInMapObject von
         * unten gerechnet. Die Berücksichtigung der Grafikhöhe ist in allen Fällen nämlich gleich.
         *
         *
         * Um die Formeln unten zu verstehen, der Reihe nach folgende Überlegungen durchführen:
         *
         * 1. Welchen Wert hat xInMapObject/yInMapObjectFromBottom, wenn die Grafik nur 1x1 groß ist?
         *    -> Es kommen Summanden mit Abhängigkeit zu moMapWidth/moMapHeight in den Term.
         * 2. Angenommen die Grafik ist 2x1 (also belegt 2 Kacheln in mapX-Richtung), wie ändert sich xInMapObject/yInMapObjectFromBottom?
         *    -> Es kommen Summanden mit Abhängigkeit zu moMapWidth/moMapHeight in den Term.
         * 3. Angenommen die Grafik ist 1x2 (also belegt 2 Kacheln in mapY-Richtung), wie ändert sich xInMapObject/yInMapObjectFromBottom?
         *    -> Es kommen Summanden mit Abhängigkeit zu moMapWidth/moMapHeight in den Term.
         *
         * Jetzt, unabhängig von der Größe überlegen:
         *
         * 4. Angenommen, ich gehe eine Kachel in mapX-Richtung, wie ändert sich xInMapObject/yInMapObjectFromBottom?
         *    -> Es kommen Summanden mit Abhängigkeit zu tileOffsetXInMapObject/tileOffsetYInMapObject in den Term.
         * 5. Angenommen, ich gehe eine Kachel in mapY-Richtung, wie ändert sich xInMapObject/yInMapObjectFromBottom?
         *    -> Es kommen Summanden mit Abhängigkeit zu tileOffsetXInMapObject/tileOffsetYInMapObject in den Term.
         *
         * Fertig :-)
         */
        int xInMapObject, yInMapObject, yInMapObjectFromBottom;
        if (screenView == Direction::SOUTH) {
            xInMapObject = IGraphicsMgr::TILE_WIDTH_HALF *
                           ((moMapHeight - 1) - tileOffsetYInMapObject + tileOffsetXInMapObject);
            yInMapObjectFromBottom = IGraphicsMgr::TILE_HEIGHT_HALF *
                                     ((moMapHeight - 1) - tileOffsetYInMapObject + (moMapWidth - 1) -
                                                                                   tileOffsetXInMapObject);
        } else if (screenView == Direction::EAST) {
            xInMapObject = IGraphicsMgr::TILE_WIDTH_HALF *
                           ((moMapWidth - 1) + (moMapHeight - 1) - tileOffsetXInMapObject - tileOffsetYInMapObject);
            yInMapObjectFromBottom = IGraphicsMgr::TILE_HEIGHT_HALF *
                                     ((moMapWidth - 1) - tileOffsetXInMapObject + tileOffsetYInMapObject);
        } else if (screenView == Direction::NORTH) {
            xInMapObject = IGraphicsMgr::TILE_WIDTH_HALF *
                           ((moMapWidth - 1) + tileOffsetYInMapObject - tileOffsetXInMapObject);
            yInMapObjectFromBottom = IGraphicsMgr::TILE_HEIGHT_HALF *
                                     (tileOffsetYInMapObject + tileOffsetXInMapObject);
        } else if (screenView == Direction::WEST) {
            xInMapObject = IGraphicsMgr::TILE_WIDTH_HALF *
                           (tileOffsetXInMapObject + tileOffsetYInMapObject);
            yInMapObjectFromBottom = IGraphicsMgr::TILE_HEIGHT_HALF *
                                     ((moMapHeight - 1) + tileOffsetXInMapObject - tileOffsetYInMapObject);
        } else {
            assert(false);
            xInMapObject = yInMapObjectFromBottom = 0;
        }
        yInMapObject = graphicToDrawHere->getHeight() - yInMapObjectFromBottom - IGraphicsMgr::TILE_HEIGHT;

        Rect rectSource(xInMapObject, 0, IGraphicsMgr::TILE_WIDTH, graphicToDrawHere->getHeight());

        ScreenCoords screenCoords = MapCoordUtils::mapToScreenCoords(mapCoords, screenView, *map);
        int elevation = (mapObjectType->name == "pier") ? 0 : 1; // TODO in die Config nehmen!

        int destDrawX = screenCoords.x() - IGraphicsMgr::TILE_WIDTH_HALF;
        int destDrawY = screenCoords.y() - yInMapObject - elevation * IGraphicsMgr::ELEVATION_HEIGHT;

        // Zoom anwenden
        destDrawX /= screenZoom;
        destDrawY /= screenZoom;

        // Zum Schluss in die Bildschirmmitte bringen
        destDrawX += Consts::mapClipRect.w / 2;
        destDrawY += Consts::mapClipRect.h / 2;

        Rect rectDestination = Rect(destDrawX, destDrawY, rectSource.w / screenZoom, rectSource.h / screenZoom);

        if (selectedMapObject != nullptr) {
            bool insideCatchmentArea =
                (selectedBuilding != nullptr &&
                 CatchmentArea::isInsideCatchmentArea(*selectedBuilding, *mapObjectToDrawHere));

            if (!insideCatchmentArea) {
                drawingFlags |= IGraphic::DRAWING_FLAG_DARKENED;
            }
        }
        graphicToDrawHere->draw(&rectSource, &rectDestination, drawingFlags);
    }


    // Träger etc. auf die Kachel zeichnen /////////////////////////////////////////////////////

    for (auto iter = mapTile->mapObjectsMoving.cbegin(); iter != mapTile->mapObjectsMoving.cend(); iter++) {
        MapObjectMoving* mapObject = *iter;
        // TODO Aktuell machen wir nur Träger
        Carrier* carrier = dynamic_cast<Carrier*>(mapObject);
        if (carrier == nullptr) {
            continue;
        }

        // Übersetzung von "Laufrichtung" + "aktuelle Ansicht" in korrekte Animation
        unsigned char animViewIndex = (10 - carrier->getCurrentMovingDirection() + screenView) % 8;

        const DoubleMapCoords& mapCoords = carrier->getMapCoords();
        const Animation* animation = carrier->getAnimations()[animViewIndex];
        const IGraphic* animationCurrentFrame = animation->getFrame((unsigned int) carrier->animationFrame);

        Rect rect = MapCoordUtils::mapToDrawCoords(
            mapCoords, *map, 1, *animationCurrentFrame, carrier->getMapWidth(), carrier->getMapHeight());

        animationCurrentFrame->drawScaledAt(rect.x, rect.y, (double) 1 / (double) screenZoom);
    }

#ifdef DEBUG_GUIMAP_COORDS
    if (screenZoom == 1) {
        ScreenCoords screenCoords = MapCoordUtils::mapToScreenCoords(mapCoords, screenView, *map);
        screenCoords.addY(IGraphicsMgr::TILE_HEIGHT_HALF);
        screenCoords.subY(IGraphicsMgr::ELEVATION_HEIGHT);

        screenCoords.addX(Consts::mapClipRect.w / 2);
        screenCoords.addY(Consts::mapClipRect.h / 2);

        std::stringstream stringstream;
        stringstream << mapCoords;

        static Color colorWhite = Color(255, 255, 255, 95);
        context->fontMgr->renderText(renderer, stringstream.str(), screenCoords.x(), screenCoords.y(), &colorWhite,
                                     nullptr, "DroidSans.ttf", 9, RENDERTEXT_HALIGN_CENTER | RENDERTEXT_VALIGN_MIDDLE);
    }
#endif
}

bool GuiMap::onEventElement(SDL_Event& event) {
    const WindowCoords& startClickWindowCoords = context->guiMgr->getStartClickWindowCoords();

    // Bauen wir grade was?
    if (inBuildingMode) {
        bool isLeftMouseButtonDown = (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT));

        // Maustaste eben runtergedrückt? Dann müssen wir uns das Gebäude merken.
        if (event.type == SDL_MOUSEBUTTONDOWN &&
            event.button.button == SDL_BUTTON_LEFT &&
            hitTest(event.button.x, event.button.y)) {

            addToBuildQueration(true);
            return false;
        }

        // Maustaste war schon gedrückt und die Position wurde bewegt? Dann müssen wir uns das Gebäude merken.
        // Wichtig in diesem Fall, dass wir die BuildOperation NICHT löschen, um die bestehenden Gebäude dort
        // nicht zu verlieren.
        if (event.type == context->userEventBase + USER_EVENT_MOUSEMOTION_MAPCOORDS &&
            isLeftMouseButtonDown &&
            hitTest(startClickWindowCoords.x(), startClickWindowCoords.y())) {

            addToBuildQueration(false);
            return false;
        }

        // Maus bewegt, der Spieler hat die linke Maustaste aber noch nicht gedrückt? Dann nur hover-Objekt bewegen
        if (event.type == context->userEventBase + USER_EVENT_MOUSEMOTION_MAPCOORDS &&
            !isLeftMouseButtonDown) {

            updateHoverObject();
            return false;
        }

        // Rechtsklick während linke Maustaste gedrückt -> resettet, wir bleiben aber im Baumodus
        if (event.type == SDL_MOUSEBUTTONUP &&
            event.button.button == SDL_BUTTON_RIGHT &&
            isLeftMouseButtonDown) {

            resetBuildOperation();
            return false;
        }

        // Maustaste losgelassen? Dann jetzt fest die Gebäude setzen
        if (event.type == SDL_MOUSEBUTTONUP &&
            event.button.button == SDL_BUTTON_LEFT &&
            hitTest(startClickWindowCoords.x(), startClickWindowCoords.y())) {

            onReleaseMouseLeftInBuildingMode();
            return false;
        }

        return true;
    }

    // Maustaste in der Karte geklickt
    if (event.type == SDL_MOUSEBUTTONUP &&
        event.button.button == SDL_BUTTON_LEFT &&
        hitTest(event.button.x, event.button.y) &&
        hitTest(startClickWindowCoords.x(), startClickWindowCoords.y())) {

        const int maxRadiusFromClick = 3;

        if (abs(event.button.x - startClickWindowCoords.x()) < maxRadiusFromClick &&
            abs(event.button.y - startClickWindowCoords.y()) < maxRadiusFromClick) {

            onClickInMapForSelection(event.button.x, event.button.y);
        }
        return false;
    }

    // Mauszeiger bewegt? Dann Statuszeile aktualisieren
    if (event.type == SDL_MOUSEMOTION && hitTest(event.motion.x, event.motion.y)) {
        const MapObjectFixed* mapObjectFixed = getMapObjectFixedUnderMouseCoords(event.button.x, event.button.y);
        if (mapObjectFixed != nullptr && !mapObjectFixed->getMapObjectType()->isForest) {
            context->guiMgr->setStatusBarText(mapObjectFixed->getMapObjectType()->title);
        } else {
            const Map* map = context->game->getMap();
            const MapCoords& mapCoords = MapCoordUtils::getMapCoordsUnderMouse(*map, event.motion.x, event.motion.y);
            const MapTile* mapTile = map->getMapTileAt(mapCoords);

            // Fruchtbarkeit nur auf Grassland anzeigen, nicht Küste, Flüsse etc.
            if (mapTile != nullptr && mapTile->getMapTileConfig()->mapTileType == MapTileType::GRASS) {
                // TODO Fruchtbarkeit von mapTile->isle;
                context->guiMgr->setStatusBarText("TODO - Fruchtbarkeit der Insel");
            } else {
                context->guiMgr->setStatusBarText("");
            }
        }

        return false;
    }

    return true;
}

void GuiMap::addToBuildQueration(bool mustResetBefore) {
    const MapCoords& mapCoordsUnderMouse = context->guiMgr->getMapCoordsUnderMouse();

    // Ok, wir müssen nun ggf. was platzieren. Es kommt nun drauf an, was wir grade platzieren
    const MapObjectType* mapObjectType = context->guiMgr->getPanelState().addingMapObject;
    StructurePlacing structurePlacing = mapObjectType->structurePlacing;
    const FourthDirection& view = context->guiMgr->getPanelState().addingMapObjectView;

    if (structurePlacing == INDIVIDUALLY) {
        // Individuell: Wir gucken, ob hier baubar ist. Falls ja, fügen wir die Stelle zur Queue hinzu

        // Der Aufrufer entscheidet, ob clearAllTemporarily() aufgerufen wird. Das ist notwendig, weil beim Runterdrücken
        // der Maustaste wir clearen müssen, um das Hover-Objekt vorher wegzukriegen. Allerdings darf beim Mausbewegen
        // dann nicht mehr gecleart werden, um nicht alle Gebäude, die wir schon haben, wieder zu verlieren.
        if (mustResetBefore) {
            resetBuildOperation();
        }

        buildOperation->requestBuildWhenNothingInTheWay(mapCoordsUnderMouse, mapObjectType, view);
    }
    else if (structurePlacing == RECTANGLE) {
        // Rechteck: Egal, was vorher gewählt war. Wir zeichnen ein Rechteck und plazieren überall dort, wo es geht.
        // Wichtig hierbei ist, dass diese Variante nur funktioniert, wenn mapObjectType eine Größe von 1x1 hat.

        const MapCoords& mapCoordsClickStart = context->guiMgr->getStartClickMapCoords();

        // Die Richtung, in der wir iterieren is wichtig. Wir bauen immer vom
        // Startpunkt aus (falls nicht alle Resourcen da sind).
        FourthDirection iterationDirection = (mapCoordsClickStart.x() > mapCoordsUnderMouse.x()) ?
            ((mapCoordsClickStart.y() > mapCoordsUnderMouse.y()) ? Direction::NORTH : Direction::WEST) :
            ((mapCoordsClickStart.y() > mapCoordsUnderMouse.y()) ? Direction::EAST : Direction::SOUTH);

        resetBuildOperation(); // immer clearen, weil das neue Rechteck alle vorher gesetzten Kacheln ersetzt

        MapCoordsIterator mapCoordsIterator(mapCoordsClickStart, mapCoordsUnderMouse, iterationDirection);
        mapCoordsIterator.iterate([&] (const MapCoords& mapCoords) {
            buildOperation->requestBuildWhenNothingInTheWay(mapCoords, mapObjectType, view);
        });
    }
    else if (structurePlacing == PATH) {
        // Pfad: Egal, was vorher gewählt war. Wir berechnen einen Pfad und plazieren überall auf dem Weg, wo es geht.
        // Wichtig hierbei ist, dass diese Variante nur funktioniert, wenn mapObjectType eine Größe von 1x1 hat.

        const MapCoords& mapCoordsClickStart = context->guiMgr->getStartClickMapCoords();

        // Sonderfall: Start = End, keine Route, sondern nur das eine Feld.
        Route route;
        if (mapCoordsUnderMouse == mapCoordsClickStart) {
            route.push_back(mapCoordsUnderMouse);
        } else {
            AStar aStar(context, nullptr, false, false, true);
            route = aStar.getRoute(mapCoordsClickStart, mapCoordsUnderMouse); // TODO Pathfinding muss berücksichtigen, dass Straße über Felder gebaut werden darf
        }

        resetBuildOperation(); // immer clearen, weil der neue Pfad alle vorher gesetzten Kacheln ersetzt

        if (route.routeExists()) {
            std::for_each(route.cbegin(), route.cend(), [&](const MapCoords& mapCoords) {
                buildOperation->requestBuildWhenNothingInTheWay(mapCoords, mapObjectType, view);
            });
        }
    }

    updateBuildingCosts();
}

void GuiMap::updateHoverObject() {
    const MapCoords& mapCoordsUnderMouse = context->guiMgr->getMapCoordsUnderMouse();

    const MapObjectType* mapObjectType = context->guiMgr->getPanelState().addingMapObject;
    const FourthDirection& view = context->guiMgr->getPanelState().addingMapObjectView;

    resetBuildOperation();
    buildOperation->requestBuild(mapCoordsUnderMouse, mapObjectType, view);
}

const MapObjectFixed* GuiMap::getMapObjectFixedUnderMouseCoords(int mouseX, int mouseY) {
    Map* map = context->game->getMap();
    int screenZoom = map->getScreenZoom();

    // Objekte iterieren und merken, welche in Frage kommen
    std::list<const MapObjectFixed*> mapObjectsHit;

    const std::list<MapObject*>& mapObjects = map->getMapObjects();
    for (auto iter = mapObjects.crbegin(); iter != mapObjects.crend(); iter++) {
        MapObject* mapObject = *iter;
        MapObjectFixed* mapObjectFixed = dynamic_cast<MapObjectFixed*>(mapObject);
        if (mapObjectFixed == nullptr) {
            continue;
        }

        Rect rect = MapCoordUtils::getDrawCoordsForMapObjectFixed(*map, context->graphicsMgr, mapObjectFixed);

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

        const FourthDirection& screenView = map->getScreenView();
        const FourthDirection& structureView = mapObjectFixed->getView();
        const FourthDirection& viewToRender = Direction::addDirections(structureView, screenView);

        const MapObjectType* mapObjectType = mapObjectFixed->getMapObjectType();
        const std::string graphicSetName =
            context->graphicsMgr->getGraphicSetNameForMapObject(mapObjectType);
        const GraphicSet* graphicSet = context->graphicsMgr->getGraphicSet(graphicSetName);

        // TODO duplicate code
        const IGraphic* graphic;
        if (mapObjectType->type == MapObjectTypeClass::HARVESTABLE) {
            const Harvestable* harvestable = dynamic_cast<const Harvestable*>(mapObjectFixed);
            const std::string state = "growth" + toString(int(harvestable->getAge()));
            graphic = graphicSet->getByStateAndView(state, viewToRender)->getGraphic();
        }
        else if (mapObjectType->type == MapObjectTypeClass::STREET) {
            const Street* street = dynamic_cast<const Street*>(mapObjectFixed);
            const std::string state = street->getStateToRender();
            graphic = graphicSet->getByStateAndView(state, viewToRender)->getGraphic();
        }
        else {
            graphic = graphicSet->getByView(viewToRender)->getGraphic();
        }
        
        graphic->getPixel(x, y, &r, &g, &b, &a);

        // Checken, ob Pixel un-transparent genug ist, um es als Treffer zu nehmen
        if (a > 127) {
            mapObjectsHit.push_back(mapObjectFixed);
        }
    }

    unsigned long hits = mapObjectsHit.size();
    if (hits == 0) {
        return nullptr;
    } else if (hits == 1) {
        return mapObjectsHit.front();
    }

    // Mehrere Treffer. Wir müssen gucken, wer oben liegt. Je nachdem, wie die Karte ausgerichtet is, anders sortieren
    const FourthDirection& screenView = map->getScreenView();

    mapObjectsHit.sort([&](const MapObjectFixed*& a, const MapObjectFixed*& b) {
        if (screenView == Direction::SOUTH) {
            return
                (a->getMapCoords().y() > b->getMapCoords().y()) ||
                ((a->getMapCoords().y() == b->getMapCoords().y()) && (a->getMapCoords().x() > b->getMapCoords().x()));
        }
        else if (screenView == Direction::EAST) {
            return
                (a->getMapCoords().x() > b->getMapCoords().x()) ||
                ((a->getMapCoords().x() == b->getMapCoords().x()) && (a->getMapCoords().y() < b->getMapCoords().y()));
        }
        else if (screenView == Direction::NORTH) {
            return
                (a->getMapCoords().y() < b->getMapCoords().y()) ||
                ((a->getMapCoords().y() == b->getMapCoords().y()) && (a->getMapCoords().x() < b->getMapCoords().x()));
        }
        else if (screenView == Direction::WEST) {
            return
                (a->getMapCoords().x() < b->getMapCoords().x()) ||
                ((a->getMapCoords().x() == b->getMapCoords().x()) && (a->getMapCoords().y() > b->getMapCoords().y()));
        }
        else {
            assert(false);
            return false;
        }
    });
    return mapObjectsHit.front();
}

void GuiMap::onClickInMapForSelection(int mouseX, int mouseY) {
    // TODO hier später weitere Typen (Schiffe) checken

    // Gucken, ob ein Gebäude geklickt wurde.
    const MapObjectFixed* mapObjectFixedClicked = getMapObjectFixedUnderMouseCoords(mouseX, mouseY);
    const Building* buildingClicked = dynamic_cast<const Building*>(mapObjectFixedClicked);

    if (buildingClicked != nullptr) {
        context->game->setSelectedMapObject(buildingClicked);
        return;
    }

    // TODO später ggf. weitere Events
    context->game->setSelectedMapObject(nullptr);
}

void GuiMap::onReleaseMouseLeftInBuildingMode() {
    buildOperation->doBuild();

    // Datenstruktur bereinigen und gleich weitermachen mit Bauen
    onCancelAddingStructure();
    onStartAddingStructure();
}

void GuiMap::drawCatchmentArea(IRenderer* const renderer, const MapObjectToBuild& mapObjectToBuild) {
    if (!mapObjectToBuild.mapObjectType->catchmentArea) {
        return;
    }

    Map* map = context->game->getMap();
    int screenZoom = map->getScreenZoom();
    const FourthDirection& screenView = map->getScreenView();

    renderer->setDrawColor(Color(0xc8, 0xaf, 0x37, 255));

    const RectangleData<char>& catchmentArea = *mapObjectToBuild.mapObjectType->catchmentArea;
    const MapCoords& mapCoords = mapObjectToBuild.mapCoords;
    int catchmentAreaRadius = (std::max(catchmentArea.width, catchmentArea.height) / 2) + 2; // TODO sehr optimierungsbedürftig, dafür funktionierts erstmal in allen Ansichten

    // Offset für die Umrechnung von Screen- in Drawing-Koordinaten berechnen.
    // Je nach Ansicht müssen wir die Koordinaten verschieben, damit die Linien an der richtigen Stelle sitzen.
    ScreenCoords screenCoordsOffset;
    if (screenView == Direction::SOUTH) {
        screenCoordsOffset = ScreenCoords(0, 0);
    } else if (screenView == Direction::EAST) {
        screenCoordsOffset = ScreenCoords(IGraphicsMgr::TILE_WIDTH_HALF, IGraphicsMgr::TILE_HEIGHT_HALF);
    } else if (screenView == Direction::NORTH) {
        screenCoordsOffset = ScreenCoords(0, IGraphicsMgr::TILE_HEIGHT);
    } else if (screenView == Direction::WEST) {
        screenCoordsOffset = ScreenCoords(-IGraphicsMgr::TILE_WIDTH_HALF, IGraphicsMgr::TILE_HEIGHT_HALF);
    }

    for (int mapY = mapCoords.y() - catchmentAreaRadius; mapY <= mapCoords.y() + catchmentAreaRadius; mapY++) {
        for (int mapX = mapCoords.x() - catchmentAreaRadius; mapX <= mapCoords.x() + catchmentAreaRadius; mapX++) {
            MapCoords mapCoordsHere(mapX, mapY);
            MapCoords mapCoordsEast(mapX + 1, mapY);
            MapCoords mapCoordsSouth(mapX, mapY + 1);

            bool mapCoordsHereInSideCatchmentArea =
                CatchmentArea::isInsideCatchmentArea(mapObjectToBuild, mapCoordsHere);
            bool mapCoordsEastInSideCatchmentArea =
                CatchmentArea::isInsideCatchmentArea(mapObjectToBuild, mapCoordsEast);
            bool mapCoordsSouthInSideCatchmentArea =
                CatchmentArea::isInsideCatchmentArea(mapObjectToBuild, mapCoordsSouth);

            auto drawLineBetweenMapCoords = [&](const MapCoords& mapCoords1, const MapCoords& mapCoords2) {
                ScreenCoords screenCoords1 = MapCoordUtils::mapToScreenCoords(mapCoords1, screenView, *map);
                ScreenCoords screenCoords2 = MapCoordUtils::mapToScreenCoords(mapCoords2, screenView, *map);

                screenCoords1 += screenCoordsOffset;
                screenCoords2 += screenCoordsOffset;

                int drawX1 = screenCoords1.x() / screenZoom;
                int drawY1 = (screenCoords1.y() - IGraphicsMgr::ELEVATION_HEIGHT) / screenZoom;
                drawX1 += Consts::mapClipRect.w / 2;
                drawY1 += Consts::mapClipRect.h / 2;

                int drawX2 = screenCoords2.x() / screenZoom;
                int drawY2 = (screenCoords2.y() - IGraphicsMgr::ELEVATION_HEIGHT) / screenZoom;
                drawX2 += Consts::mapClipRect.w / 2;
                drawY2 += Consts::mapClipRect.h / 2;

                renderer->drawLine(drawX1, drawY1, drawX2, drawY2);
            };

            bool drawYLine =
                ((mapCoordsHereInSideCatchmentArea && !mapCoordsEastInSideCatchmentArea) ||
                (!mapCoordsHereInSideCatchmentArea && mapCoordsEastInSideCatchmentArea));
            bool drawXLine =
                ((mapCoordsHereInSideCatchmentArea && !mapCoordsSouthInSideCatchmentArea) ||
                (!mapCoordsHereInSideCatchmentArea && mapCoordsSouthInSideCatchmentArea));

            // Übergang West -> Ost (mapX-Richtung)
            if (drawYLine) {
                drawLineBetweenMapCoords(mapCoordsEast, MapCoords(mapX + 1, mapY + 1));
            }

            // Übergang Nord -> Süd (mapY-Richtung)
            if (drawXLine) {
                drawLineBetweenMapCoords(mapCoordsSouth, MapCoords(mapX + 1, mapY + 1));
            }
        }
    }
}

void GuiMap::onNewGame() {
    // TODO gucken, ob wir den Callback überhaupt noch brauchen
}

void GuiMap::onStartAddingStructure() {
    inBuildingMode = true;

    // Move-Handler triggern, damit die Hover-Grafik gezeigt wird
    updateHoverObject();

    // Baukosten in der Resourcen-Leiste nach einem Mehrfachbauen wieder auf 1x zurücksetzen
    updateBuildingCosts();
}

void GuiMap::onCancelAddingStructure() {
    buildOperation.release();

    inBuildingMode = false;
}

void GuiMap::onRotateAddingStructure() {
    if (inBuildingMode && !(SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON_LEFT)) {
        updateHoverObject();
    }
}

void GuiMap::resetBuildOperation() {
    Player* player = context->game->getCurrentPlayer();
    buildOperation.reset(new BuildOperation(context, *player));
}

void GuiMap::updateBuildingCosts() {
    if (!buildOperation) {
        guiResourcesBar->hideBuildingCosts();
        return;
    }

    guiResourcesBar->showBuildingCosts(buildOperation->getResult().buildingCosts);
}