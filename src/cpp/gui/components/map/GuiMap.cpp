#include <SDL.h>
#include <algorithm>
#include <cassert>
#include <string>
#include "config/ConfigMgr.h"
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
#include "pathfinding/AStar.h"
#include "utils/Consts.h"
#include "utils/Events.h"
#include "utils/MapCoordsIterator.h"
#include "utils/StringFormat.h"

#ifdef DEBUG_GUIMAP_COORDS
#include <sstream>
#endif

#if defined(DEBUG_A_STAR) || defined(DEBUG_GUIMAP_COORDS)
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
    clearAllTemporarily();

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
    const MapObject* selectedMapObject = map->getSelectedMapObject();
    const Building* selectedBuilding = dynamic_cast<const Building*>(selectedMapObject);

    int screenZoom = map->getScreenZoom();

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

    updateMapObjectsTemporarilyDrawingFlags();

    // Jetzt rendern
    mapCoordsIterator.iterate([&] (const MapCoords& mapCoords) {
        const MapTile* mapTile = map->getMapTileAt(mapCoords);
        if (mapTile == nullptr) {
            return;
        }

        // Kachel rendern //////////////////////////////////////////////////////////////////////////

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

        int drawingFlags = 0;
        if (selectedMapObject != nullptr) {
            bool insideCatchmentArea =
                (selectedBuilding != nullptr && selectedBuilding->isInsideCatchmentArea(mapCoords));

            if (!insideCatchmentArea) {
                drawingFlags |= IGraphic::DRAWING_FLAG_DARKENED;
            }
        }

        auto iterMapTileTemporarily = mapTilesToDrawTemporarily.find(mapCoords);
        if (iterMapTileTemporarily != mapTilesToDrawTemporarily.cend()) {
            if (iterMapTileTemporarily->second.drawTileMasked) {
                drawingFlags |= IGraphic::DRAWING_FLAG_MASKED;
            }
        }

#ifdef DEBUG_GUIMAP
        // Kachel unter dem Mauszeiger rot färben
        if(mapCoords == MapCoordUtils::getMapCoordsUnderMouse(*map, context->mouseCurrentX, context->mouseCurrentY))
            drawingFlags |= IGraphic::DRAWING_FLAG_RED;
#endif

        tileGraphic->draw(nullptr, &rectDestination, drawingFlags, context->sdlTicks);


        // MapObjekt auf die Kachel rendern ////////////////////////////////////////////////////////

        // Gucken, ob/welches Objekt wir hier malen müssen
        const MapObjectFixed* mapObjectToDrawHere = nullptr;

        // Temporär was anderes zeichnen?
        if (iterMapTileTemporarily != mapTilesToDrawTemporarily.cend()) {
            mapObjectToDrawHere = iterMapTileTemporarily->second.mapObjectToDraw.get();
        }

        // Nichts temporär? Dann gucken, ob im Spielstand was is, was wir zeichnen müssen
        if (mapObjectToDrawHere == nullptr) {
            const MapObjectFixed* mapObject = mapTile->mapObjectFixed;
            if (mapObject != nullptr) {
                mapObjectToDrawHere = mapObject;
            }
        }

        if (mapObjectToDrawHere != nullptr) {
            // @see docs/drawing-order-x-tiles.xcf für Variablen
            // TODO Vorsicht: doc/drawing-order-x-tiles.xcf ist veraltet -> mapObjectAlreadyDrawnThere gibts nicht mehr

            const MapCoords& moMapCoords = mapObjectToDrawHere->getMapCoords();
            const Structure* structure = dynamic_cast<const Structure*>(mapObjectToDrawHere);
            const Harvestable* harvestable = dynamic_cast<const Harvestable*>(mapObjectToDrawHere);

            // Ausrechnen, welchen Schnibbel der Grafik wir anzeigen müssen
            int moMapWidth = mapObjectToDrawHere->getMapWidth();
            int moMapHeight = mapObjectToDrawHere->getMapHeight();

            int tileOffsetXInMapObject = mapCoords.x() - moMapCoords.x(); // (0 ... moMapWidth-1)
            int tileOffsetYInMapObject = mapCoords.y() - moMapCoords.y(); // (0 ... moMapHeight-1)

            int drawingFlags = mapObjectToDrawHere->getDrawingFlags();
            const MapObjectType* mapObjectType = mapObjectToDrawHere->getMapObjectType();

            const FourthDirection& view = mapObjectToDrawHere->getView();
            const FourthDirection& viewToRender = Direction::addDirections(view, screenView);

            const std::string& graphicSetName = context->graphicsMgr->getGraphicSetNameForMapObject(mapObjectType);
            const GraphicSet* mapObjectGraphicSet = context->graphicsMgr->getGraphicSet(graphicSetName);

            const IGraphic* graphicToDrawHere;
            if (structure != nullptr) {
                graphicToDrawHere = mapObjectGraphicSet->getByView(viewToRender)->getGraphic();
            }
            else if (harvestable != nullptr) {
                const std::string stateToRender = "growth" + toString(int(harvestable->getAge()));
                graphicToDrawHere = mapObjectGraphicSet->getByStateAndView(stateToRender, viewToRender)->getGraphic();
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
                     selectedBuilding->isInsideCatchmentArea(*mapObjectToDrawHere));

                if (!insideCatchmentArea) {
                    drawingFlags |= IGraphic::DRAWING_FLAG_DARKENED;
                }
            }
            graphicToDrawHere->draw(&rectSource, &rectDestination, drawingFlags, context->sdlTicks);
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
    });

    // Einzugsbereich jetzt malen, damit er oben drauf is
    if (buildingToDrawCatchmentArea != nullptr) {
        drawCatchmentArea(renderer, *buildingToDrawCatchmentArea);
    }

#ifdef DEBUG_A_STAR
    Route routeToDraw;

    // Wenn Gebäude ausgewählt -> Trägerroute anzeigen
    if (selectedBuilding != nullptr) {
        routeToDraw = (selectedBuilding->carrier != nullptr) ? selectedBuilding->carrier->route : Route();
    } else {
        routeToDraw = AStar::debugAStar_route;
    }

    // A*-Route zeichnen (nur bei Maximalzoom, dann sparen wir uns Berechnungen und der Code wird einfacher)
    if (screenZoom == 1 && routeToDraw.routeExists()) {
        renderer->setDrawBlendMode(IRenderer::BLENDMODE_BLEND);

        int lastPointX = -1;
        int lastPointY = -1;

        int i = 1;
        for (auto iter = routeToDraw.cbegin(); iter != routeToDraw.cend(); iter++) {
            const MapCoords& mapCoords = *iter;

            ScreenCoords screenCoords = MapCoordUtils::mapToScreenCoords(mapCoords, screenView, *map);
            screenCoords.addY(IGraphicsMgr::TILE_HEIGHT_HALF);
            screenCoords.subY(IGraphicsMgr::ELEVATION_HEIGHT);

            screenCoords.addX(Consts::mapClipRect.w / 2);
            screenCoords.addY(Consts::mapClipRect.h / 2);

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

bool GuiMap::onEventElement(SDL_Event& event) {
    const WindowCoords& startClickWindowCoords = context->guiMgr->getStartClickWindowCoords();

    // Bauen wir grade was?
    if (inBuildingMode) {
        bool isLeftMouseButtonDown = (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT));

        // Maustaste eben runtergedrückt? Dann müssen wir uns das Gebäude merken.
        if (event.type == SDL_MOUSEBUTTONDOWN &&
            event.button.button == SDL_BUTTON_LEFT &&
            hitTest(event.button.x, event.button.y)) {

            addToBuildQueue(true);
            return false;
        }

        // Maustaste war schon gedrückt und die Position wurde bewegt? Dann müssen wir uns das Gebäude merken.
        // Wichtig in diesem Fall, dass wir die MapTileTemporarily NICHT löschen, um die bestehenden Gebäude dort
        // nicht zu verlieren.
        if (event.type == context->userEventBase + USER_EVENT_MOUSEMOTION_MAPCOORDS &&
            isLeftMouseButtonDown &&
            hitTest(startClickWindowCoords.x(), startClickWindowCoords.y())) {

            addToBuildQueue(false);
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

            clearAllTemporarily();
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

void GuiMap::addToBuildQueue(bool mustClearAllTemporarily) {
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
        if (mustClearAllTemporarily) {
            clearAllTemporarily();
        }

        unsigned char isAllowedToPlaceResult =
            isAllowedToPlaceMapObject(mapCoordsUnderMouse, mapObjectType, view);
        if ((isAllowedToPlaceResult & ~PLACING_STRUCTURE_NO_RESOURCES) == PLACING_STRUCTURE_ALLOWED) {
            addCurrentStructureToBuildQueue(mapCoordsUnderMouse);
        }
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

        clearAllTemporarily(); // immer clearen, weil das neue Rechteck alle vorher gesetzten Kacheln ersetzt

        MapCoordsIterator mapCoordsIterator(mapCoordsClickStart, mapCoordsUnderMouse, iterationDirection);
        mapCoordsIterator.iterate([&] (const MapCoords& mapCoords) {
            unsigned char isAllowedToPlaceResult = isAllowedToPlaceMapObject(mapCoords, mapObjectType, view);
            if ((isAllowedToPlaceResult & ~PLACING_STRUCTURE_NO_RESOURCES) == PLACING_STRUCTURE_ALLOWED) {
                addCurrentStructureToBuildQueue(mapCoords);
            }
        });

        // TODO Bestimmte Strukturen dürfen über andere gebaut werden. Z.B. kann ein Wegfeld ein Getreidefeld
        // überbauen, ein Getreidefeld aber nicht einen Wegfeld.
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
            route = aStar.getRoute(mapCoordsClickStart, mapCoordsUnderMouse);
        }

        clearAllTemporarily(); // immer clearen, weil der neue Pfad alle vorher gesetzten Kacheln ersetzt

        if (route.routeExists()) {
            std::for_each(route.cbegin(), route.cend(), [&](const MapCoords& mapCoords) {
                unsigned char isAllowedToPlaceResult = isAllowedToPlaceMapObject(mapCoords, mapObjectType, view);
                if ((isAllowedToPlaceResult & ~PLACING_STRUCTURE_NO_RESOURCES) == PLACING_STRUCTURE_ALLOWED) {
                    addCurrentStructureToBuildQueue(mapCoords);
                }
            });
        }
    }
}

void GuiMap::updateHoverObject() {
    Map* map = context->game->getMap();
    const MapCoords& mapCoordsUnderMouse = context->guiMgr->getMapCoordsUnderMouse();

    clearAllTemporarily();

    const MapObjectType* mapObjectType = context->guiMgr->getPanelState().addingMapObject;
    const FourthDirection& view = context->guiMgr->getPanelState().addingMapObjectView;

    unsigned char mapWidth, mapHeight;
    if (view == Direction::NORTH || view == Direction::SOUTH) {
        mapWidth = mapObjectType->mapWidth;
        mapHeight = mapObjectType->mapHeight;
    } else {
        mapWidth = mapObjectType->mapHeight;
        mapHeight = mapObjectType->mapWidth;
    }

    // Zu belegende Kachel mehrfach durchgehen.

    // Loop 1: Gucken, ob das Gebäude komplett Platz hat. Falls nicht, nur Gelände maskiert anzeigen.
    bool allTilesFree = true;
    for (int y = mapCoordsUnderMouse.y(); y < mapCoordsUnderMouse.y() + mapHeight; y++) {
        for (int x = mapCoordsUnderMouse.x(); x < mapCoordsUnderMouse.x() + mapWidth; x++) {

            if (map->getMapObjectFixedAt(MapCoords(x, y)) != nullptr) {
                allTilesFree = false;
                goto endLoop1;
            }
        }
    }
endLoop1:

    // Loop 2: Nun durchgehen und entweder das masked-Flag oder das neu zu erstellende Map-Objekt setzen.
    std::shared_ptr<MapObjectFixed> mapObjectHover;
    if (allTilesFree) {
        MapObjectFixed* mapObjectFixed = instantiateMapObjectFixed(mapObjectType);
        mapObjectFixed->setMapCoords(mapCoordsUnderMouse);
        mapObjectFixed->setMapWidth(mapWidth);
        mapObjectFixed->setMapHeight(mapHeight);
        mapObjectFixed->setView(view);
        mapObjectFixed->setDrawingFlags(IGraphic::DRAWING_FLAG_MASKED);

        mapObjectHover.reset(mapObjectFixed);

        buildingToDrawCatchmentArea = std::dynamic_pointer_cast<Building>(mapObjectHover);
    }

    for (int y = mapCoordsUnderMouse.y(); y < mapCoordsUnderMouse.y() + mapHeight; y++) {
        for (int x = mapCoordsUnderMouse.x(); x < mapCoordsUnderMouse.x() + mapWidth; x++) {
            const MapCoords& mapCoords = MapCoords(x, y);

            if (allTilesFree) {
                mapTilesToDrawTemporarily[mapCoords].mapObjectToDraw = mapObjectHover;
            } else if (!allTilesFree && map->getMapObjectFixedAt(mapCoords) == nullptr) {
                mapTilesToDrawTemporarily[mapCoords].drawTileMasked = true;
            }
        }
    }

    // Loop 3: Jetzt ist die mapTilesToDrawTemporarily vollständig befüllt, nun angrenzende Felder aktualisieren
    for (int y = mapCoordsUnderMouse.y(); y < mapCoordsUnderMouse.y() + mapHeight; y++) {
        for (int x = mapCoordsUnderMouse.x(); x < mapCoordsUnderMouse.x() + mapWidth; x++) {
            MapCoords mapCoords(x, y);
            updateMapTilesToDrawTemporarilyForStreetsAt(mapCoords); // TODO optimieren?
            updateMapTilesToDrawTemporarilyForStreetsAround(mapCoords);
        }
    }
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
        
        const IGraphic* graphic;
        if (mapObjectType->type != MapObjectTypeClass::HARVESTABLE) {
            graphic = graphicSet->getByView(viewToRender)->getGraphic();
        } else {
            const Harvestable* harvestable = dynamic_cast<const Harvestable*>(mapObjectFixed);
            const std::string state = "growth" + toString(int(harvestable->getAge()));
            graphic = graphicSet->getByStateAndView(state, viewToRender)->getGraphic();
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
    Player* currentPlayer = context->game->getCurrentPlayer();

    // Der Reihe nach alle zu setzenden Gebäude durchgehen. Gehen die Resourcen aus, brechen wir ab.
    for (auto iter = buildQueue.cbegin(); iter != buildQueue.cend(); iter++) {
        const MapObjectFixed& mapObject = **iter;

        const MapObjectType* mapObjectType = mapObject.getMapObjectType();
        const MapCoords& mapCoords = mapObject.getMapCoords();

        const FourthDirection& view = mapObject.getView();
        unsigned char isAllowedToPlaceResult = isAllowedToPlaceMapObject(mapCoords, mapObjectType, view);
        if ((isAllowedToPlaceResult & ~PLACING_STRUCTURE_SOMETHING_IN_THE_WAY_TEMPORARILY) != PLACING_STRUCTURE_ALLOWED) {
            // Dürfen wir nicht mehr setzen (zwischenzeitlich was im Weg oder Resourcen ausgegangen), dann abbrechen
            break;
        }

        // Ok, Gebäude nun wirklich setzen
        context->game->addMapObjectFixed(mapCoords, mapObjectType, view, currentPlayer);

        // Resourcen bezahlen
        Colony* colony = context->game->getColony(
            currentPlayer, context->game->getMap()->getMapTileAt(mapCoords)->isle);
        const BuildingCosts& buildingCosts = mapObjectType->buildingCosts;

        currentPlayer->coins -= buildingCosts.coins;
        colony->getGoods(toolsGood).inventory -= buildingCosts.tools;
        colony->getGoods(woodGood).inventory -= buildingCosts.wood;
        colony->getGoods(bricksGood).inventory -= buildingCosts.bricks;
    }
    buildQueue.clear();

    // Datenstruktur bereinigen und gleich weitermachen mit Bauen
    onCancelAddingStructure();
    onStartAddingStructure();
}

unsigned char GuiMap::isAllowedToPlaceMapObject(
    const MapCoords& mapCoords, const MapObjectType* mapObjectType, const FourthDirection& view) const {

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
        const BuildingCosts& buildingCosts = mapObjectType->buildingCosts;
        if ((buildingCosts.coins > currentPlayer->coins) ||
            (buildingCosts.tools > colony->getGoods(toolsGood).inventory) ||
            (buildingCosts.wood > colony->getGoods(woodGood).inventory) ||
            (buildingCosts.bricks > colony->getGoods(bricksGood).inventory)) {
            result |= PLACING_STRUCTURE_NO_RESOURCES;
        }
    }

    // Checken, ob alles frei und erlaubt is, um das Gebäude zu setzen
    unsigned char mapWidth, mapHeight;
    if (view == Direction::NORTH || view == Direction::SOUTH) {
        mapWidth = mapObjectType->mapWidth;
        mapHeight = mapObjectType->mapHeight;
    } else {
        mapWidth = mapObjectType->mapHeight;
        mapHeight = mapObjectType->mapWidth;
    }

    for (int y = mapCoords.y(); y < mapCoords.y() + mapHeight; y++) {
        for (int x = mapCoords.x(); x < mapCoords.x() + mapWidth; x++) {
            const MapTile* mapTile = context->game->getMap()->getMapTileAt(MapCoords(x, y));

            // Steht was im Weg?
            if (mapTile->mapObjectFixed != nullptr) {
                result |= PLACING_STRUCTURE_SOMETHING_IN_THE_WAY;
            }

            auto iterMapTileTemporarily = mapTilesToDrawTemporarily.find(MapCoords(x, y));
            if (iterMapTileTemporarily != mapTilesToDrawTemporarily.cend()) {
                if (iterMapTileTemporarily->second.mapObjectToDraw) {
                    // Zwar noch nicht echt gebaut, allerdings wollen wir da was hinbauen.
                    result |= PLACING_STRUCTURE_SOMETHING_IN_THE_WAY_TEMPORARILY;
                }
            }

            // Passt das Gelände?
            const MapTileConfig* mapTileConfig = mapTile->getMapTileConfig();
            if (((MapTileTypeInt) mapTileConfig->mapTileType & mapObjectType->placeableOnMapTileTypeMask) == 0) {
                result |= PLACING_STRUCTURE_MAP_TILE_TYPE_MISMATCH;
            }

            // Gebiet erschlossen?
            if (mapTile->player == nullptr ||                             // Gebiet nicht erschlossen,
                mapTile->player != context->game->getCurrentPlayer()) {   // oder nicht unseres

                result |= PLACING_STRUCTURE_ROOM_NOT_UNLOCK;
            }
        }
    }

    return result;
}

void GuiMap::drawCatchmentArea(IRenderer* const renderer, const Building& building) {
    Map* map = context->game->getMap();
    int screenZoom = map->getScreenZoom();
    const FourthDirection& screenView = map->getScreenView();

    renderer->setDrawColor(Color(0xc8, 0xaf, 0x37, 255));

    if (!building.getMapObjectType()->catchmentArea) {
        return;
    }
    const RectangleData<char>& catchmentArea = *building.getMapObjectType()->catchmentArea;

    const MapCoords& mapCoords = building.getMapCoords();
    int catchmentAreaRadius = std::max(catchmentArea.width, catchmentArea.height) / 2; // TODO sehr optimierungsbedürftig, dafür funktionierts erstmal in allen Ansichten

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

            bool mapCoordsHereInSideCatchmentArea = building.isInsideCatchmentArea(mapCoordsHere);
            bool mapCoordsEastInSideCatchmentArea = building.isInsideCatchmentArea(mapCoordsEast);
            bool mapCoordsSouthInSideCatchmentArea = building.isInsideCatchmentArea(mapCoordsSouth);

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
    // Alles wegräumen und neu anlegen
    clearAllTemporarily();
}

void GuiMap::onStartAddingStructure() {
    inBuildingMode = true;

    // Move-Handler triggern, damit die Hover-Grafik gezeigt wird
    updateHoverObject();

    // Baukosten in der Resourcen-Leiste nach einem Mehrfachbauen wieder auf 1x zurücksetzen
    updateBuildingCosts();
}

void GuiMap::onCancelAddingStructure() {
    inBuildingMode = false;
    clearAllTemporarily();
}

void GuiMap::onRotateAddingStructure() {
    if (inBuildingMode && buildQueue.empty() && !(SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON_LEFT)) {
        updateHoverObject();
    }
}

void GuiMap::clearAllTemporarily() {
    buildQueue.clear();

    for (auto iter = mapTilesToDrawTemporarily.cbegin(); iter != mapTilesToDrawTemporarily.cend(); iter++) {
        const MapTileTemporarily& mapTileTemporarily = iter->second;

        if (mapTileTemporarily.mapObjectToReplaceWith != nullptr) {
            delete mapTileTemporarily.mapObjectToReplaceWith;
        }
    }
    mapTilesToDrawTemporarily.clear();

    buildingToDrawCatchmentArea = nullptr;
}

void GuiMap::addCurrentStructureToBuildQueue(const MapCoords& mapCoords) {
    const MapObjectType* mapObjectType = context->guiMgr->getPanelState().addingMapObject;
    const FourthDirection& view = context->guiMgr->getPanelState().addingMapObjectView;

    unsigned char mapWidth, mapHeight;
    if (view == Direction::NORTH || view == Direction::SOUTH) {
        mapWidth = mapObjectType->mapWidth;
        mapHeight = mapObjectType->mapHeight;
    } else {
        mapWidth = mapObjectType->mapHeight;
        mapHeight = mapObjectType->mapWidth;
    }

    // Sonderfälle, wo zufällig gewählt wird: Haus (= zufälliges Pionier-Haus) und Wald
    if (mapObjectType->name == "pioneers-house1" || mapObjectType->name == "northern-forest1") {
        std::random_device randomDevice;
        std::default_random_engine randomEngine(randomDevice());

        if (mapObjectType->name == "pioneers-house1") {
            std::uniform_int_distribution<int> randomPioneerHouse(1, 5);
            std::string mapObjectTypeName = "pioneers-house" + toString(randomPioneerHouse(randomEngine));

            mapObjectType = context->configMgr->getMapObjectType(mapObjectTypeName);
        } else if (mapObjectType->name == "northern-forest1") {
            std::uniform_int_distribution<int> randomNorthernForest(1, 2);
            std::string mapObjectTypeName = "northern-forest" + toString(randomNorthernForest(randomEngine));

            mapObjectType = context->configMgr->getMapObjectType(mapObjectTypeName);
        }
    }

    Structure* structure = (Structure*) instantiateMapObjectFixed(mapObjectType);
    structure->setMapCoords(mapCoords);
    structure->setMapWidth(mapWidth);
    structure->setMapHeight(mapHeight);
    structure->setView(context->guiMgr->getPanelState().addingMapObjectView);
    structure->setDrawingFlags(IGraphic::DRAWING_FLAG_MASKED);

    std::shared_ptr<MapObjectFixed> structureBeingAdded(structure);
    buildQueue.push_back(structureBeingAdded);

    for (int my = mapCoords.y(); my < mapCoords.y() + mapHeight; my++) {
        for (int mx = mapCoords.x(); mx < mapCoords.x() + mapWidth; mx++) {
            mapTilesToDrawTemporarily[MapCoords(mx, my)].mapObjectToDraw = structureBeingAdded;
        }
    }

    // Hovering beenden. Jetzt ist die linke Maustaste runtergedrückt und wir adden immer zu buildQueue.
    std::shared_ptr<MapObjectFixed> firstMapObjectInList = buildQueue.front();

    // Einzugsbereich wird am ersten Gebäude in buildQueue gezeichnet
    // TODO nur machen, wenn wir grade das erste Objekt in die Liste haben
    buildingToDrawCatchmentArea = std::dynamic_pointer_cast<Building>(firstMapObjectInList);

    updateMapTilesToDrawTemporarilyForStreetsAt(mapCoords);
    updateMapTilesToDrawTemporarilyForStreetsAround(mapCoords);

    // Baukosten in der Resourcen-Leiste aktualisieren
    updateBuildingCosts();
}

void GuiMap::updateMapObjectsTemporarilyDrawingFlags() {
    if (inBuildingMode == false || mapTilesToDrawTemporarily.empty()) {
        return;
    }

    // Build-Queue leer? D.h. nur das Hover-Objekt ist in mapTilesToDrawTemporarily
    if (buildQueue.empty()) {
        if (!mapTilesToDrawTemporarily.cbegin()->second.mapObjectToDraw) {
            return; // kein Hover-Objekt da? Dann darfs hier eh nicht platziert werden, z.B. weil Platz belegt
        }

        const MapObjectFixed& mapObjectFixed = *mapTilesToDrawTemporarily.cbegin()->second.mapObjectToDraw;
        const MapCoords& mapCoords = mapObjectFixed.getMapCoords();
        const MapObjectType* mapObjectType = mapObjectFixed.getMapObjectType();
        const FourthDirection& view = mapObjectFixed.getView();

        bool noResources =
            (isAllowedToPlaceMapObject(mapCoords, mapObjectType, view) & PLACING_STRUCTURE_NO_RESOURCES);

        // In mapTilesToDrawTemporarily können zwar mehrere Kacheln stehen, allerdings referenzieren diese
        // alle dasselbe Objekt, nämlich das Hover-Objekt.
        auto iter = mapTilesToDrawTemporarily.begin();
        MapObjectFixed& hoverObject = *iter->second.mapObjectToDraw;

        if (noResources) {
            hoverObject.setDrawingFlags(hoverObject.getDrawingFlags() | IGraphic::DRAWING_FLAG_BLINK);
        } else {
            hoverObject.setDrawingFlags(hoverObject.getDrawingFlags() & ~IGraphic::DRAWING_FLAG_BLINK);
        }

        return;
    }

    // Ansonsten gucken wir buildQueue durch

    // Wir nutzen die Tatsache aus, dass immer nur dieselben Gebäude in der Bauqueue sind.
    MapObjectFixed& firstObjectInList = *buildQueue.front();
    const MapObjectType* mapObjectType = firstObjectInList.getMapObjectType();
    const BuildingCosts& buildingCostsOneTime = mapObjectType->buildingCosts;

    const Player* currentPlayer = context->game->getCurrentPlayer();

    // TODO aktuell können in buildQueue Gebäude von verschiedenen Siedlungen sein. Das muss gefixed werden. Nur von einer Siedlung ist erlaubt.
    // Wir verwenden das erstbeste MapObject in der buildQueue, um die Kolonie rauszufinden, in der wir bauen wollen.
    MapObjectFixed& someMapObject = **buildQueue.cbegin();
    Colony* colony = context->game->getColony(
        currentPlayer, context->game->getMap()->getMapTileAt(someMapObject.getMapCoords())->isle);
    const double toolsInColony = colony->getGoods(toolsGood).inventory;
    const double woodInColony = colony->getGoods(woodGood).inventory;
    const double bricksInColony = colony->getGoods(bricksGood).inventory;

    bool outOfResources = false;

    // Der Reihe nach alle zu setzenden Gebäude durchgehen und gucken, wann/ob die Resourcen ausgehen
    int structuresCount = 1;
    for (auto iter = buildQueue.cbegin(); iter != buildQueue.cend(); iter++, structuresCount++) {
        MapObjectFixed& mapObject = **iter;

        if (!outOfResources) {
            BuildingCosts sumBuildingCostTilHere = buildingCostsOneTime * structuresCount;

            if (currentPlayer->coins < sumBuildingCostTilHere.coins ||
                toolsInColony < sumBuildingCostTilHere.tools ||
                woodInColony < sumBuildingCostTilHere.wood ||
                bricksInColony < sumBuildingCostTilHere.bricks) {

                outOfResources = true;
            }
        }

        if (outOfResources) {
            mapObject.setDrawingFlags(mapObject.getDrawingFlags() | IGraphic::DRAWING_FLAG_BLINK);
        } else {
            mapObject.setDrawingFlags(mapObject.getDrawingFlags() & ~IGraphic::DRAWING_FLAG_BLINK);
        }
    }
}

void GuiMap::updateBuildingCosts() {
    // Wir nutzen die Tatsache aus, dass immer nur dieselben Gebäude in der Bauqueue sind.
    const MapObjectType* mapObjectType = context->guiMgr->getPanelState().addingMapObject;
    const BuildingCosts& buildingCostsOneTime = mapObjectType->buildingCosts;

    unsigned long structuresCount = buildQueue.size();
    if (structuresCount == 0) {
        structuresCount = 1;
    }

    guiResourcesBar->showBuildingCosts(buildingCostsOneTime * structuresCount);
}


/**
 * Suffixe von MapObjectType-Name je Bitmaske "angrenzende Straßen".
 * Dieses Suffixe muss an den Base-Namen (`farm-road` oder `cobbled-street`) angehängt werden.
 *
 * Array-Index gibt an, von welcher Seite eine andere Straße/Weg angrenzt:
 * - Bit gesetzt = Straße/Weg da
 * - Bit nicht gesetzt = frei
 *
 * <pre>
 * ----------------------------------------------------------------
 * | isStreetLeft | isStreetAbove | isStreetRight | isStreetBelow |
 * |     Bit 3    |     Bit 2     |     Bit 1     |     Bit 0     |
 * ----------------------------------------------------------------
 * </pre>
 *
 * z.&nbsp;B. T-Stück.
 * <pre>
 *
 *              isStreetAbove = 0
 *
 *                 |----------
 *                 |         |
 *  isStreetLeft   |         |    isStreetRight
 *      = 1        |XXXXXXXXX|         = 1
 *                 |    X    |
 *                 |    X    |
 *                 ----------|
 *
 *              isStreetBelow = 1
 *
 * ----------------------------------------------------------------
 * | isStreetLeft | isStreetAbove | isStreetRight | isStreetBelow |
 * |     Bit 3    |     Bit 2     |     Bit 1     |     Bit 0     |
 * ----------------------------------------------------------------
 * |       1      |       0       |       1       |       1       |  --> 11d
 * --------------------------------------------------------------------------
 *
 * mapObjectTypeNameSuffixBitmask[11] = "tee270"
 *
 * </pre>
 */
static const char* mapObjectTypeNameSuffixBitmask[16] = {
    nullptr, // alle Felder frei = Sonderbedeutung, weil wir diesen Wert dann nicht brauchen
    "-straight90",
    "-straight0",
    "-curve90",
    "-straight90",
    "-straight90",
    "-curve0",
    "-tee180",
    "-straight0",
    "-curve180",
    "-straight0",
    "-tee270",
    "-curve270",
    "-tee0",
    "-tee90",
    "-cross"
};

void GuiMap::updateMapTilesToDrawTemporarilyForStreetsAt(const MapCoords& mapCoordsToUpdate) {
    Map* map = context->game->getMap();
    MapTile* mapTile = map->getMapTileAt(mapCoordsToUpdate);

    // Ist hier eine Straße/Weg, die wir temporär anders darstellen müssen?
    if (mapTile == nullptr) {
        return;
    }

    // Methode, um das MapObject an einer Stelle zu holen (mapTilesToDrawTemporarily berücksichtigt!)
    auto getMapObject = [&](const MapCoords& mapCoords) {
        auto iterMapTileTemporialy = mapTilesToDrawTemporarily.find(mapCoords);
        if (iterMapTileTemporialy != mapTilesToDrawTemporarily.cend()) {
            if (iterMapTileTemporialy->second.mapObjectToDraw) {
                return iterMapTileTemporialy->second.mapObjectToDraw.get();
            }
        }

        return map->getMapObjectFixedAt(mapCoords);
    };

    const Structure* structureActualHere = dynamic_cast<const Structure*>(getMapObject(mapCoordsToUpdate));
    if (structureActualHere == nullptr) {
        return;
    }

    const MapObjectType* mapObjectTypeActual = structureActualHere->getMapObjectType();
    // TODO Basename sollte direkt in der Config sein und nicht mehr jedes Teil als eigenes Stück
    bool isCobbledStreet = (mapObjectTypeActual->name.substr(0, 14) == "cobbled-street");
    bool isFarmRoad = (mapObjectTypeActual->name.substr(0, 9) == "farm-road");
    if (!isCobbledStreet && !isFarmRoad) {
        return;
    }

    // Angrenzende Straßen (temporäre berücksichtigen!) checken
    auto isStreetThere = [&](const MapCoords& mapCoordsInLamdba) {
        if (map->isStreetAt(mapCoordsInLamdba)) {
            return true;
        }

        auto iterMapTileTemporialy = mapTilesToDrawTemporarily.find(mapCoordsInLamdba);
        if (iterMapTileTemporialy != mapTilesToDrawTemporarily.cend()) {
            Structure* structure = dynamic_cast<Structure*>(iterMapTileTemporialy->second.mapObjectToDraw.get());
            if (structure != nullptr && structure->isStreet()) {
                return true;
            }
        }

        return false;
    };

    // siehe mapObjectTypeNameSuffixBitmask
    char isStreetBelow = isStreetThere(MapCoords(mapCoordsToUpdate.x(), mapCoordsToUpdate.y() + 1)) ? 1 : 0; // Bit 0
    char isStreetRight = isStreetThere(MapCoords(mapCoordsToUpdate.x() + 1, mapCoordsToUpdate.y())) ? 1 : 0; // Bit 1
    char isStreetAbove = isStreetThere(MapCoords(mapCoordsToUpdate.x(), mapCoordsToUpdate.y() - 1)) ? 1 : 0; // Bit 2
    char isStreetLeft = isStreetThere(MapCoords(mapCoordsToUpdate.x() - 1, mapCoordsToUpdate.y())) ? 1 : 0;  // Bit 3

    if (!isStreetAbove && !isStreetRight && !isStreetLeft && !isStreetBelow) {
        // keine Straße außenrum? Dann greift die Orientierung, die der Spieler gewählt hat.
        return;
    }

    std::string mapObjectTypeNameBase;
    if (isCobbledStreet) {
        mapObjectTypeNameBase = "cobbled-street";
    } else if (isFarmRoad) {
        mapObjectTypeNameBase = "farm-road";
    } else {
        assert(false);
        return;
    }

    // Kachel bestimmen, die dort liegen sollte.
    int index = (isStreetLeft << 3) | (isStreetAbove << 2) | (isStreetRight << 1) | isStreetBelow;
    const MapObjectType* mapObjectTypeShouldBe =
        context->configMgr->getMapObjectType(mapObjectTypeNameBase + mapObjectTypeNameSuffixBitmask[index]);

    // Sonderfall: Wenn der Spieler beim Bauen die Kachel gedreht hat, würden wir gleich den falschen MapObjectType
    // vergleichen, deswegen passen wir das vorher noch an.
    if (structureActualHere->getView() == Direction::EAST || structureActualHere->getView() == Direction::WEST) {
         // aus ...straight0 machen wir ...straight90
        mapObjectTypeActual = context->configMgr->getMapObjectType(mapObjectTypeNameBase + "straight90");
    }

    // Wenn keine Abweichung, ok
    if (mapObjectTypeActual == mapObjectTypeShouldBe) {
        return;
    }

    // Ansonsten temporär ersetzen.
    auto iterMapTileTemporialy = mapTilesToDrawTemporarily.find(mapCoordsToUpdate);
    if (iterMapTileTemporialy == mapTilesToDrawTemporarily.cend()) {
        MapObjectFixed* structureToTemporarilyReplaceWith = instantiateMapObjectFixed(mapObjectTypeShouldBe);
        structureToTemporarilyReplaceWith->setMapCoords(mapCoordsToUpdate);
        structureToTemporarilyReplaceWith->setMapWidth(1);
        structureToTemporarilyReplaceWith->setMapHeight(1);
        structureToTemporarilyReplaceWith->setView(Direction::SOUTH);
        structureToTemporarilyReplaceWith->setDrawingFlags(0);

        mapTilesToDrawTemporarily[mapCoordsToUpdate].mapObjectToDraw.reset(structureToTemporarilyReplaceWith);
    } else {
        MapObjectFixed& mapObjectToAlter = *iterMapTileTemporialy->second.mapObjectToDraw; // TODO ggf. -> mapObjectToReplaceWith?
        mapObjectToAlter.setMapObjectType(mapObjectTypeShouldBe);
        mapObjectToAlter.setView(Direction::SOUTH);
    }

    // TODO mapObjectToReplaceWith
}

void GuiMap::updateMapTilesToDrawTemporarilyForStreetsAround(const MapCoords& mapCoordsToUpdateAround) {
    // Relevant sind nur die waagrecht-angrenzenden Felder
    MapCoords mapCoordsToCheck[4] = {
        MapCoords(mapCoordsToUpdateAround.x() - 1, mapCoordsToUpdateAround.y()),
        MapCoords(mapCoordsToUpdateAround.x() + 1, mapCoordsToUpdateAround.y()),
        MapCoords(mapCoordsToUpdateAround.x(), mapCoordsToUpdateAround.y() - 1),
        MapCoords(mapCoordsToUpdateAround.x(), mapCoordsToUpdateAround.y() + 1),
    };

    for (const MapCoords& mapCoords : mapCoordsToCheck) {
        updateMapTilesToDrawTemporarilyForStreetsAt(mapCoords);
    }
}

MapObjectFixed* GuiMap::instantiateMapObjectFixed(const MapObjectType* mapObjectType) {
    MapObjectFixed* mapObjectFixed;

    if (mapObjectType->type == MapObjectTypeClass::HARVESTABLE) {
        mapObjectFixed = new Harvestable(mapObjectType->maxAge);
    } else if (mapObjectType->type == MapObjectTypeClass::STRUCTURE) {
        mapObjectFixed = new Structure();
    } else if (mapObjectType->type == MapObjectTypeClass::BUILDING) {
        mapObjectFixed = new Building();
    } else {
        assert(false);
        return nullptr;
    }

    mapObjectFixed->setMapObjectType(mapObjectType);
    return mapObjectFixed;
}