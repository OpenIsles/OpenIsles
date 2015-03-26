#include <SDL.h>
#include <algorithm>
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
#include "pathfinding/AStar.h"
#include "utils/Consts.h"
#include "utils/MapCoordsIterator.h"
#include "utils/StringFormat.h"

#ifdef DEBUG_A_STAR
#include "graphics/mgr/IFontMgr.h"
#endif


GuiMap::GuiMap(const Context* const context) : GuiBase(context) {
    setCoords(Consts::mapClipRect.x, Consts::mapClipRect.y, Consts::mapClipRect.w, Consts::mapClipRect.h);

#ifdef DEBUG
    debugGridOverlayGraphic = new SDLGraphic(context->graphicsMgr->getRenderer(), "data/debug-grid-overlay.png");
#endif
}

GuiMap::~GuiMap() {
    if (mapObjectAlreadyDrawnThere != nullptr) {
        delete mapObjectAlreadyDrawnThere;
        mapObjectAlreadyDrawnThere = nullptr;
    }

#ifdef DEBUG
    delete debugGridOverlayGraphic;
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
    Rect sdlMapClipRect(Consts::mapClipRect);
    sdlMapClipRect.y = renderer->getWindowHeight() - (sdlMapClipRect.y + sdlMapClipRect.h); // SDL misst das Rechteck von UNTEN, kp, warum und ob das ein Bug is
    renderer->setClipRect(&sdlMapClipRect);

    // Kacheln rendern
    mapCoordsIterator.iterate([&] (const MapCoords& mapCoords) {
        const MapTile* mapTile = map->getMapTileAt(mapCoords);
        if (mapTile == nullptr) {
            return;
        }

        const Animation* tileAnimation = mapTile->getTileAnimationForView(screenView);
        const IGraphic* tileGraphic = tileAnimation->getGraphic();

        Rect rectDestination = MapCoordUtils::mapToDrawCoords(mapCoords, *map, 0, *tileGraphic);

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
                (selectedBuilding != nullptr && selectedBuilding->isInsideCatchmentArea(context->configMgr, mapCoords));

            if (!insideCatchmentArea) {
                drawingFlags |= IGraphic::DRAWING_FLAG_DARKENED;
            }
        }

#ifdef DEBUG_GUIMAP
        // Kachel unter dem Mauszeiger rot färben
        if(mapCoords == MapCoordUtils::getMapCoordsUnderMouse(*map, context->mouseCurrentX, context->mouseCurrentY))
            drawingFlags |= IGraphic::DRAWING_FLAG_RED;
#endif

        tileGraphic->draw(nullptr, &rectDestination, drawingFlags, context->sdlTicks);
    });

    // Objekte rendern
    memset(mapObjectAlreadyDrawnThere->data, 0,
        mapObjectAlreadyDrawnThere->width * mapObjectAlreadyDrawnThere->height * sizeof(char));

    // TODO Start und End noch ein wenig weiter ausweiten?
    mapCoordsIterator.iterate([&] (const MapCoords& mapCoords) {
        // Außerhalb der Karte?
        if (mapCoords.x() < 0 || mapCoords.y() < 0 || mapCoords.x() >= map->getWidth() || mapCoords.y() >= map->getHeight()) {
            return;
        }

        // Hier schon bemalt?
        if (mapObjectAlreadyDrawnThere->getData(mapCoords.x(), mapCoords.y(), 1) == 1) {
            return;
        }

        // Gucken, ob/welches Objekt wir hier malen müssen
        const MapObjectFixed* mapObjectToDrawHere = nullptr;
        bool drawPlainTile = false;

        const MapTileTemporialy* mapTileTemporialy = mapTilesToDrawTemporarily->getData(mapCoords.x(), mapCoords.y());
        if (mapTileTemporialy->mapObject != nullptr) {
            mapObjectToDrawHere = mapTileTemporialy->mapObject;
            updateMapObjectsTemporarilyDrawingFlags();
        } else {
            const MapObjectFixed* mapObject = map->getMapObjectAt(mapCoords);
            if (mapObject != nullptr) {
                mapObjectToDrawHere = mapObject;
            } else {
                if (mapObjectBeingAddedHovering != nullptr) {
                    if (mapCoords.x() >= mapObjectBeingAddedHovering->getMapCoords().x() &&
                        mapCoords.y() >= mapObjectBeingAddedHovering->getMapCoords().y() &&
                        mapCoords.x() < mapObjectBeingAddedHovering->getMapCoords().x() +
                                        mapObjectBeingAddedHovering->getMapWidth() &&
                        mapCoords.y() < mapObjectBeingAddedHovering->getMapCoords().y() +
                                        mapObjectBeingAddedHovering->getMapHeight()) {

                        buildingToDrawCatchmentArea = dynamic_cast<Building*>(mapObjectBeingAddedHovering);
                        mapObjectToDrawHere = mapObjectBeingAddedHovering;
                        updateMapObjectsTemporarilyDrawingFlags();

                        // Gucken, ob das Gebäude komplett Platz hat. Falls nicht, nur plainTile anzeigen
                        for (int y = mapObjectBeingAddedHovering->getMapCoords().y();
                             y < mapObjectBeingAddedHovering->getMapCoords().y() +
                                 mapObjectBeingAddedHovering->getMapHeight();
                             y++) {

                            for (int x = mapObjectBeingAddedHovering->getMapCoords().x();
                             x < mapObjectBeingAddedHovering->getMapCoords().x() +
                                 mapObjectBeingAddedHovering->getMapWidth();
                             x++) {

                                if (map->getMapObjectAt(MapCoords(x, y)) != nullptr) {
                                    drawPlainTile = true;
                                    break;
                                }
                            }
                        }

                    } else {
                        return; // nix zum Zeichen an dieser Stelle
                    }
                } else {
                    return; // nix zum Zeichen an dieser Stelle
                }
            }
        }

        // @see docs/drawing-order-x-tiles.xcf für Variablen

        const MapCoords& moMapCoords = mapObjectToDrawHere->getMapCoords();
        const Structure* structure = dynamic_cast<const Structure*>(mapObjectToDrawHere); // TODO nullptr sollte nicht passieren; später checken, wenn wir Bäume und sowas haben

        int moMapWidth, moMapHeight;
        int tileOffsetXInMapObject, tileOffsetYInMapObject;
        int drawingFlags;
        const IGraphic* graphicToDrawHere;

        if (drawPlainTile) {
            // maskierte Kachel is 1x1
            moMapWidth = 1;
            moMapHeight = 1;
            tileOffsetXInMapObject = 0;
            tileOffsetYInMapObject = 0;
            drawingFlags = IGraphic::DRAWING_FLAG_MASKED;
            graphicToDrawHere = context->graphicsMgr->getGraphicSet("plain-tile")->getStatic()->getGraphic();

        } else {

            // Ausrechnen, welchen Schnibbel der Grafik wir anzeigen müssen
            moMapWidth = mapObjectToDrawHere->getMapWidth();
            moMapHeight = mapObjectToDrawHere->getMapHeight();

            tileOffsetXInMapObject = mapCoords.x() - moMapCoords.x(); // (0 ... moMapWidth-1)
            tileOffsetYInMapObject = mapCoords.y() - moMapCoords.y(); // (0 ... moMapHeight-1)

            drawingFlags = structure->getDrawingFlags();
            StructureType structureType = structure->getStructureType();

            const FourthDirection& structureView = structure->getView();
            const FourthDirection& viewToRender = Direction::addDirections(structureView, screenView);

            const std::string graphicSetName = context->graphicsMgr->getGraphicSetNameForStructure(structureType);
            const GraphicSet* mapObjectGraphicSet = context->graphicsMgr->getGraphicSet(graphicSetName);
            graphicToDrawHere = mapObjectGraphicSet->getByView(viewToRender)->getGraphic();
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
            xInMapObject =  IGraphicsMgr::TILE_WIDTH_HALF *
                ((moMapHeight - 1) - tileOffsetYInMapObject + tileOffsetXInMapObject);
            yInMapObjectFromBottom =  IGraphicsMgr::TILE_HEIGHT_HALF *
                ((moMapHeight - 1) - tileOffsetYInMapObject + (moMapWidth - 1) - tileOffsetXInMapObject);
        } else if (screenView == Direction::EAST) {
            xInMapObject =  IGraphicsMgr::TILE_WIDTH_HALF *
                ((moMapWidth - 1) + (moMapHeight - 1) - tileOffsetXInMapObject - tileOffsetYInMapObject);
            yInMapObjectFromBottom =  IGraphicsMgr::TILE_HEIGHT_HALF *
                ((moMapWidth - 1) - tileOffsetXInMapObject + tileOffsetYInMapObject);
        } else if (screenView == Direction::NORTH) {
            xInMapObject =  IGraphicsMgr::TILE_WIDTH_HALF *
                ((moMapWidth - 1) + tileOffsetYInMapObject - tileOffsetXInMapObject);
            yInMapObjectFromBottom =  IGraphicsMgr::TILE_HEIGHT_HALF *
                (tileOffsetYInMapObject + tileOffsetXInMapObject);
        } else if (screenView == Direction::WEST) {
            xInMapObject =  IGraphicsMgr::TILE_WIDTH_HALF *
                (tileOffsetXInMapObject + tileOffsetYInMapObject);
            yInMapObjectFromBottom =  IGraphicsMgr::TILE_HEIGHT_HALF *
                ((moMapHeight - 1) + tileOffsetXInMapObject - tileOffsetYInMapObject);
        } else {
            assert(false);
            xInMapObject = yInMapObjectFromBottom = 0;
        }
        yInMapObject = graphicToDrawHere->getHeight() - yInMapObjectFromBottom - IGraphicsMgr::TILE_HEIGHT;

        Rect rectSource(xInMapObject, 0, IGraphicsMgr::TILE_WIDTH, graphicToDrawHere->getHeight());

        ScreenCoords screenCoords = MapCoordUtils::mapToScreenCoords(mapCoords, screenView, *map);
        const int elevation = 1; // TODO für Gebäude wie Anlegestelle, Fischerhütte etc. muss auf 0 gesetzt werden

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
                (selectedBuilding != nullptr && selectedBuilding->isInsideCatchmentArea(context->configMgr, *structure));

            if (!insideCatchmentArea) {
                drawingFlags |= IGraphic::DRAWING_FLAG_DARKENED;
            }
        }
        graphicToDrawHere->draw(&rectSource, &rectDestination, drawingFlags, context->sdlTicks);

        // In mapObjectAlreadyDrawnThere die Kacheln-Spalte als erledigt markieren
        do {
            mapObjectAlreadyDrawnThere->setData(
                moMapCoords.x() + tileOffsetXInMapObject, moMapCoords.y() + tileOffsetYInMapObject, 1);

            if (screenView == Direction::SOUTH) {
                tileOffsetXInMapObject++;
                tileOffsetYInMapObject++;
            } else if (screenView == Direction::EAST) {
                tileOffsetXInMapObject++;
                tileOffsetYInMapObject--;
            } else if (screenView == Direction::NORTH) {
                tileOffsetXInMapObject--;
                tileOffsetYInMapObject--;
            } else if (screenView == Direction::WEST) {
                tileOffsetXInMapObject--;
                tileOffsetYInMapObject++;
            } else {
                assert(false);
            }
        } while(
            tileOffsetXInMapObject < moMapWidth && tileOffsetYInMapObject < moMapHeight &&
            tileOffsetXInMapObject >= 0 && tileOffsetYInMapObject >= 0);
    });

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

        // Übersetzung von "Laufrichtung" + "aktuelle Ansicht" in korrekte Animation
        unsigned char animViewIndex = (10 - carrier->getCurrentMovingDirection() + screenView) % 8;

        const DoubleMapCoords& mapCoords = carrier->getMapCoords();
        const Animation* animation = carrier->getAnimations()[animViewIndex];
        const IGraphic* animationCurrentFrame = animation->getFrame((unsigned int) carrier->animationFrame);

        Rect rect = MapCoordUtils::mapToDrawCoords(mapCoords, *map, 1, *animationCurrentFrame);

        animationCurrentFrame->drawScaledAt(rect.x, rect.y, (double) 1 / (double) screenZoom);
    }

    // Einzugsbereich jetzt malen, damit er oben drauf is
    if (buildingToDrawCatchmentArea != nullptr) {
        drawCatchmentArea(renderer, buildingToDrawCatchmentArea);
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

#ifdef DEBUG_GUIMAP
    // Grid zeichnen, an dem wir uns orientieren können
    debugGridOverlayGraphic->drawAt(0, 0);

    // Zusätzlich die Mitte markieren. Die sollte sich genau im ScreenCoords(0, 0)-Punkt decken
    int x = Consts::mapClipRect.w / 2;
    int y = Consts::mapClipRect.h / 2;
    renderer->drawLine(x-10, y, x+10, y);
    renderer->drawLine(x, y-10, x, y+10);

    // getDrawCoordsForBuilding()-Rechteck einzeichnen
    for (auto iter = mapObjects.crbegin(); iter != mapObjects.crend(); iter++) {
        MapObject* mapObject = *iter;

        Building* building = dynamic_cast<Building*>(mapObject);
        if (building != nullptr) {
            Rect rect = MapCoordUtils::getDrawCoordsForBuilding(*map, context->graphicsMgr, building);
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
        Map* map = context->game->getMap();
        MapCoords mapCoordsUnderMouse =
            MapCoordUtils::getMapCoordsUnderMouse(*map, context->mouseCurrentX, context->mouseCurrentY);

        bool isLeftMouseButtonDown = (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT));

        // Maustaste eben runtergedrückt oder schon gedrückt und bewegt? Dann müssen wir uns das Gebäude merken.
        if (
            (event.type == SDL_MOUSEBUTTONDOWN &&
             event.button.button == SDL_BUTTON_LEFT &&
             hitTest(event.button.x, event.button.y))
                ||
            (event.type == SDL_MOUSEMOTION &&
             isLeftMouseButtonDown &&
             hitTest(startClickWindowCoords.x(), startClickWindowCoords.y()))
           ) {

            // Ok, wir müssen nun ggf. was platzieren. Es kommt nun drauf an, was wir grade platzieren
            StructureType structureType = context->guiMgr->getPanelState().addingStructure;
            const BuildingConfig* buildingConfig = context->configMgr->getBuildingConfig(structureType);
            StructurePlacing structurePlacing = buildingConfig->structurePlacing;
            const FourthDirection& view = context->guiMgr->getPanelState().addingStructureView;

            if (structurePlacing == INDIVIDUALLY) {
                // Individuell: Wir gucken, ob hier baubar ist. Falls ja, fügen wir die Stelle zur Queue hinzu

                unsigned char isAllowedToPlaceResult = isAllowedToPlaceStructure(mapCoordsUnderMouse, structureType, view);
                if ((isAllowedToPlaceResult & ~PLACING_STRUCTURE_NO_RESOURCES) == PLACING_STRUCTURE_ALLOWED) {
                    addCurrentStructureToMapObjectsBeingAdded(mapCoordsUnderMouse);
                }
            }
            else if (structurePlacing == RECTANGLE) {
                // Rechteck: Egal, was vorher gewählt war. Wir zeichnen ein Rechteck und plazieren überall dort, wo es geht.
                // Wichtig hierbei ist, dass diese Variante nur funktioniert, wenn structureType eine Größe von 1x1 hat.

                // TODO Performace: Es wäre besser, wenn wir nur neuberechnen, wenn der Mauszeiger wirklich
                // auf einer neuen Kachel landet und nicht bei jeder Pixelbewegung.

                const MapCoords& mapCoordsClickStart = context->guiMgr->getStartClickMapCoords();

                // Die Richtung, in der wir iterieren is wichtig. Wir bauen immer vom
                // Startpunkt aus (falls nicht alle Resourcen da sind).
                FourthDirection iterationDirection = (mapCoordsClickStart.x() > mapCoordsUnderMouse.x()) ?
                    ((mapCoordsClickStart.y() > mapCoordsUnderMouse.y()) ? Direction::NORTH : Direction::WEST) :
                    ((mapCoordsClickStart.y() > mapCoordsUnderMouse.y()) ? Direction::EAST : Direction::SOUTH);

                clearAllTemporarily();
                MapCoordsIterator mapCoordsIterator(mapCoordsClickStart, mapCoordsUnderMouse, iterationDirection);
                mapCoordsIterator.iterate([&] (const MapCoords& mapCoords) {
                    unsigned char isAllowedToPlaceResult = isAllowedToPlaceStructure(mapCoords, structureType, view);
                    if ((isAllowedToPlaceResult & ~PLACING_STRUCTURE_NO_RESOURCES) == PLACING_STRUCTURE_ALLOWED) {
                        addCurrentStructureToMapObjectsBeingAdded(mapCoords);
                    }
                });

                // TODO Bestimmte Strukturen dürfen über andere gebaut werden. Z.B. kann ein Wegfeld ein Getreidefeld
                // überbauen, ein Getreidefeld aber nicht einen Wegfeld.
            }
            else if (structurePlacing == PATH) {
                // Pfad: Egal, was vorher gewählt war. Wir berechnen einen Pfad und plazieren überall auf dem Weg, wo es geht.
                // Wichtig hierbei ist, dass diese Variante nur funktioniert, wenn structureType eine Größe von 1x1 hat.

                // TODO Performace: Es wäre besser, wenn wir nur neuberechnen, wenn der Mauszeiger wirklich
                // auf einer neuen Kachel landet und nicht bei jeder Pixelbewegung.

                const MapCoords& mapCoordsClickStart = context->guiMgr->getStartClickMapCoords();

                // Sonderfall: Start = End, keine Route, sondern nur das eine Feld.
                Route* route;
                if (mapCoordsUnderMouse == mapCoordsClickStart) {
                    route = new Route();
                    route->push_back(mapCoordsUnderMouse);
                } else {
                    AStar aStar(context, mapCoordsClickStart, mapCoordsUnderMouse, nullptr, false, true);
                    route = aStar.getRoute();
                }

                clearAllTemporarily();
                if (route != nullptr) {
                    std::for_each(route->cbegin(), route->cend(), [ & ](const MapCoords& mapCoords) {
                        unsigned char isAllowedToPlaceResult = isAllowedToPlaceStructure(mapCoords, structureType, view);
                        if ((isAllowedToPlaceResult & ~PLACING_STRUCTURE_NO_RESOURCES) == PLACING_STRUCTURE_ALLOWED) {
                            addCurrentStructureToMapObjectsBeingAdded(mapCoords);
                        }
                    });

                    delete route;
                }
            }

            return false;
        }

        // Maus bewegt, der Spieler hat die linke Maustaste aber noch nicht gedrückt? Dann nur hover-Objekt bewegen
        if (event.type == SDL_MOUSEMOTION && !isLeftMouseButtonDown && mapObjectBeingAddedHovering != nullptr) {
            mapObjectBeingAddedHovering->setMapCoords(mapCoordsUnderMouse);
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

            finishAddingStructure();
            return false;
        }

        return true;
    }

    // Maustaste in der Karte geklickt
    if (event.type == SDL_MOUSEBUTTONUP &&
        event.button.button == SDL_BUTTON_LEFT &&
        hitTest(event.button.x, event.button.y) &&
        hitTest(startClickWindowCoords.x(), startClickWindowCoords.y())) {

        if (abs(event.button.x - startClickWindowCoords.x()) < 3 && abs(event.button.y - startClickWindowCoords.y()) < 3) {
            onClickInMap(event.button.x, event.button.y);
        }
        return false;
    }

    return true;
}

void GuiMap::onClickInMap(int mouseX, int mouseY) {
    Map* map = context->game->getMap();
    int screenZoom = map->getScreenZoom();

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

        const FourthDirection& screenView = map->getScreenView();
        const FourthDirection& structureView = building->getView();
        const FourthDirection& viewToRender = Direction::addDirections(structureView, screenView);

        const std::string graphicSetName = context->graphicsMgr->getGraphicSetNameForStructure(building->getStructureType());
        const GraphicSet* graphicSet = context->graphicsMgr->getGraphicSet(graphicSetName);
        graphicSet->getByView(viewToRender)->getGraphic()->getPixel(x, y, &r, &g, &b, &a);

        // Checken, ob Pixel un-transparent genug ist, um es als Treffer zu nehmen
        if (a > 127) {
            context->game->setSelectedMapObject(building);
            return;
        }
    }

    // TODO später ggf. weitere Events
    context->game->setSelectedMapObject(nullptr);
}

void GuiMap::finishAddingStructure() {
    Player* currentPlayer = context->game->getCurrentPlayer();

    // Der Reihe nach alle zu setzenden Gebäude durchgehen. Gehen die Resourcen aus, brechen wir ab.
    bool dontAddStructure = false;
    for (auto iter = mapObjectsBeingAdded.cbegin(); iter != mapObjectsBeingAdded.cend(); iter++) {
        Structure* mapObject = *iter;

        if (!dontAddStructure) {
            StructureType structureType = mapObject->getStructureType();
            const MapCoords& mapCoords = mapObject->getMapCoords();

            const FourthDirection& view = mapObject->getView();
            unsigned char isAllowedToPlaceResult = isAllowedToPlaceStructure(mapCoords, structureType, view);
            if ((isAllowedToPlaceResult & ~PLACING_STRUCTURE_SOMETHING_IN_THE_WAY_TEMPORARILY) != PLACING_STRUCTURE_ALLOWED) {
                // Dürfen wir nicht mehr setzen (zwischenzeitlich was im Weg oder Resourcen ausgegangen), dann abbrechen
                dontAddStructure = true;
                continue;
            }

            // Ok, Gebäude nun wirklich setzen
            context->game->addStructure(mapCoords, structureType, view, currentPlayer);

            // Resourcen bezahlen
            Colony* colony = context->game->getColony(
                currentPlayer, context->game->getMap()->getMapTileAt(mapCoords)->isle);
            const BuildingCosts* buildingCosts = context->configMgr->getBuildingConfig(structureType)->getBuildingCosts();
            currentPlayer->coins -= buildingCosts->coins;
            colony->subtractBuildingCosts(buildingCosts);
        }

        delete mapObject;
    }
    mapObjectsBeingAdded.clear();

    // Datenstruktur bereinigen und gleich weitermachen mit Bauen
    onCancelAddingStructure();
    onStartAddingStructure();
}

unsigned char GuiMap::isAllowedToPlaceStructure(
    const MapCoords& mapCoords, StructureType structureType, const FourthDirection& view) const {

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
    const std::string graphicSetName = context->graphicsMgr->getGraphicSetNameForStructure(structureType);
    const GraphicSet* graphicSet = context->graphicsMgr->getGraphicSet(graphicSetName);
    const IGraphic* graphic = graphicSet->getByView(view)->getGraphic();
    for (int y = mapCoords.y(); y < mapCoords.y() + graphic->getMapHeight(); y++) {
        for (int x = mapCoords.x(); x < mapCoords.x() + graphic->getMapWidth(); x++) {
            // Steht was im Weg?
            MapTile* mapTile = context->game->getMap()->getMapTileAt(MapCoords(x, y));
            if (mapTile->mapObject != nullptr) {
                result |= PLACING_STRUCTURE_SOMETHING_IN_THE_WAY;
            }

            MapTileTemporialy* mapTileTemporialy = mapTilesToDrawTemporarily->getData(x, y);
            if (mapTileTemporialy->mapObject != nullptr) {
                // Zwar noch nicht echt gebaut, allerdings wollen wir da was hinbauen.
                result |= PLACING_STRUCTURE_SOMETHING_IN_THE_WAY_TEMPORARILY;
            }

            const MapTileConfig* mapTileConfig = mapTile->getMapTileConfig();
            if (mapTile->player == nullptr ||                             // Gebiet nicht erschlossen, ..,
                mapTile->player != context->game->getCurrentPlayer() ||   // ... nicht unseres...
                !mapTileConfig->isWalkableAndBuildable) {                 // ... oder Gelände passt nicht

                result |= PLACING_STRUCTURE_ROOM_NOT_UNLOCK;
            }
        }
    }

    return result;
}

void GuiMap::drawCatchmentArea(IRenderer* const renderer, const Building* building) {
    Map* map = context->game->getMap();
    int screenZoom = map->getScreenZoom();
    const FourthDirection& screenView = map->getScreenView();

    renderer->setDrawColor(Color(0xc8, 0xaf, 0x37, 255));

    const BuildingConfig* buildingConfig = context->configMgr->getBuildingConfig(building->getStructureType());
    const RectangleData<char>* catchmentArea = buildingConfig->getCatchmentArea();

    if (catchmentArea != nullptr) {
        const MapCoords& mapCoords = building->getMapCoords();
        int catchmentAreaRadius = std::max(catchmentArea->width, catchmentArea->height); // TODO sehr optimierungsbedürftig, dafür funktionierts erstmal in allen Ansichten

        for (int mapY = mapCoords.y() - catchmentAreaRadius; mapY <= mapCoords.y() + catchmentAreaRadius; mapY++) {
            for (int mapX = mapCoords.x() - catchmentAreaRadius; mapX <= mapCoords.x() + catchmentAreaRadius; mapX++) {
                MapCoords mapCoordsHere(mapX, mapY);
                MapCoords mapCoordsEast(mapX + 1, mapY);
                MapCoords mapCoordsSouth(mapX, mapY + 1);

                bool mapCoordsHereInSideCatchmentArea =
                    building->isInsideCatchmentArea(context->configMgr, mapCoordsHere);
                bool mapCoordsEastInSideCatchmentArea =
                    building->isInsideCatchmentArea(context->configMgr, mapCoordsEast);
                bool mapCoordsSouthInSideCatchmentArea =
                    building->isInsideCatchmentArea(context->configMgr, mapCoordsSouth);

                auto drawLineBetweenMapCoords = [&](const MapCoords& mapCoords1, const MapCoords& mapCoords2) {
                    ScreenCoords screenCoords1 = MapCoordUtils::mapToScreenCoords(mapCoords1, screenView, *map);
                    ScreenCoords screenCoords2 = MapCoordUtils::mapToScreenCoords(mapCoords2, screenView, *map);

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
}

// TODO den Code brauchen wir später sicher nochmal, wenn wir die Straßen wieder fixen
//StructureType GuiMap::getConcreteStreetStructureType(
//    const MapCoords& mapCoords, StructureType abstractStreetStructureType) const {
//
//    if (abstractStreetStructureType == StructureType::STREET) {
//        static const StructureType structureTypeMap[16] = {
//            STREET_STRAIGHT_90, // oder STREET_STRAIGHT_0, is in diesem Fall egal. TODO Sollte später mit der Drehfunktion genauer untersucht werden
//            STREET_STRAIGHT_90,
//            STREET_STRAIGHT_0,
//            STREET_CURVE_0,
//            STREET_STRAIGHT_0,
//            STREET_CURVE_270,
//            STREET_STRAIGHT_0,
//            STREET_TEE_90,
//            STREET_STRAIGHT_90,
//            STREET_STRAIGHT_90,
//            STREET_CURVE_90,
//            STREET_TEE_180,
//            STREET_CURVE_180,
//            STREET_TEE_0,
//            STREET_TEE_270,
//            STREET_CROSS
//        };
//
//        Map* map = context->game->getMap();
//        char isStreetAbove = map->isStreetAt(MapCoords(mapCoords.x(), mapCoords.y() - 1)) ? 1 : 0; // Bit 0
//        char isStreetRight = map->isStreetAt(MapCoords(mapCoords.x() + 1, mapCoords.y())) ? 1 : 0; // Bit 1
//        char isStreetLeft = map->isStreetAt(MapCoords(mapCoords.x() - 1, mapCoords.y())) ? 1 : 0;  // Bit 2
//        char isStreetBelow = map->isStreetAt(MapCoords(mapCoords.x(), mapCoords.y() + 1)) ? 1 : 0; // Bit 3
//
//        int index = (isStreetBelow << 3) | (isStreetLeft << 2) | (isStreetRight << 1) | isStreetAbove;
//
//        return structureTypeMap[index];
//    }
//
//    // TODO Feldweg
//    throw std::runtime_error("Illegal abstractStreetStructureType " + toString(abstractStreetStructureType));
//}

void GuiMap::onNewGame() {
    Map* map = context->game->getMap();
    int newMapWidth = map->getWidth();
    int newMapHeight = map->getHeight();

    // Alles wegräumen und neu anlegen
    for (auto iter = mapObjectsBeingAdded.cbegin(); iter != mapObjectsBeingAdded.cend(); iter++) {
        Structure* mapObject = *iter;
        delete mapObject;
    }
    mapObjectsBeingAdded.clear();

    if (mapObjectAlreadyDrawnThere != nullptr) {
        delete mapObjectAlreadyDrawnThere;
        mapObjectAlreadyDrawnThere = nullptr;
    }
    mapObjectAlreadyDrawnThere = new RectangleData<char>(newMapWidth, newMapHeight);

    if (mapTilesToDrawTemporarily != nullptr) {
        delete mapTilesToDrawTemporarily;
        mapTilesToDrawTemporarily = nullptr;
    }

    mapTilesToDrawTemporarily = new RectangleData<MapTileTemporialy*>(newMapWidth, newMapHeight);
    for (int i = 0; i < newMapWidth * newMapHeight; i++) {
        mapTilesToDrawTemporarily->data[i] = new MapTileTemporialy();
    }

    buildingToDrawCatchmentArea = nullptr;
}

void GuiMap::onStartAddingStructure() {
    inBuildingMode = true;

    Map* map = context->game->getMap();
    MapCoords mapCoordsUnderMouse =
        MapCoordUtils::getMapCoordsUnderMouse(*map, context->mouseCurrentX, context->mouseCurrentY);

    StructureType structureType = context->guiMgr->getPanelState().addingStructure;
    const FourthDirection& view = context->guiMgr->getPanelState().addingStructureView;

    const std::string graphicSetName = context->graphicsMgr->getGraphicSetNameForStructure(structureType);
    const GraphicSet* graphicSet = context->graphicsMgr->getGraphicSet(graphicSetName);
    const IGraphic* graphic = graphicSet->getByView(view)->getGraphic();

    mapObjectBeingAddedHovering = (structureType >= START_BUILDINGS) ? new Building() : new Structure();
    mapObjectBeingAddedHovering->setStructureType(structureType);
    mapObjectBeingAddedHovering->setMapCoords(mapCoordsUnderMouse);
    mapObjectBeingAddedHovering->setMapWidth(graphic->getMapWidth());
    mapObjectBeingAddedHovering->setMapHeight(graphic->getMapHeight());
    mapObjectBeingAddedHovering->setView(context->guiMgr->getPanelState().addingStructureView);
    mapObjectBeingAddedHovering->setDrawingFlags(IGraphic::DRAWING_FLAG_MASKED);
}

void GuiMap::onCancelAddingStructure() {
    inBuildingMode = false;
    clearAllTemporarily();
}

void GuiMap::onRotateAddingStructure() {
    if (mapObjectBeingAddedHovering != nullptr) {
        StructureType structureType = context->guiMgr->getPanelState().addingStructure;
        const FourthDirection& view = context->guiMgr->getPanelState().addingStructureView;

        const std::string graphicSetName = context->graphicsMgr->getGraphicSetNameForStructure(structureType);
        const GraphicSet* graphicSet = context->graphicsMgr->getGraphicSet(graphicSetName);
        const IGraphic* graphic = graphicSet->getByView(view)->getGraphic();

        mapObjectBeingAddedHovering->setMapWidth(graphic->getMapWidth());
        mapObjectBeingAddedHovering->setMapHeight(graphic->getMapHeight());
        mapObjectBeingAddedHovering->setView(context->guiMgr->getPanelState().addingStructureView);
    }
}

void GuiMap::clearAllTemporarily() {
    for (auto iter = mapObjectsBeingAdded.cbegin(); iter != mapObjectsBeingAdded.cend(); iter++) {
        Structure* mapObject = *iter;
        delete mapObject;
    }
    mapObjectsBeingAdded.clear();

    for (int i = 0; i < mapTilesToDrawTemporarily->width * mapTilesToDrawTemporarily->height; i++) {
        mapTilesToDrawTemporarily->data[i]->mapObject = nullptr;
    }

    if (mapObjectBeingAddedHovering != nullptr) {
        delete mapObjectBeingAddedHovering;
        mapObjectBeingAddedHovering = nullptr;
    }

    buildingToDrawCatchmentArea = nullptr;
}

void GuiMap::addCurrentStructureToMapObjectsBeingAdded(const MapCoords& mapCoords) {
    StructureType structureType = context->guiMgr->getPanelState().addingStructure;
    const FourthDirection& view = context->guiMgr->getPanelState().addingStructureView;

    const std::string graphicSetName = context->graphicsMgr->getGraphicSetNameForStructure(structureType);
    const GraphicSet* graphicSet = context->graphicsMgr->getGraphicSet(graphicSetName);
    const IGraphic* graphic = graphicSet->getByView(view)->getGraphic();

    // Sonderfall Haus: Wir wählen zufällig ein Pionier-Haus aus
    if (structureType == StructureType::PIONEERS_HOUSE1) {
        std::random_device randomDevice;
        std::default_random_engine randomEngine(randomDevice());
        std::uniform_int_distribution<int> randomPioneerHouse(
            StructureType::PIONEERS_HOUSE1, StructureType::PIONEERS_HOUSE5);

        structureType = (StructureType) randomPioneerHouse(randomEngine);
    }

    Structure* structureBeingAdded = (structureType >= START_BUILDINGS) ? new Building() : new Structure();
    structureBeingAdded->setStructureType(structureType);
    structureBeingAdded->setMapCoords(mapCoords);
    structureBeingAdded->setMapWidth(graphic->getMapWidth());
    structureBeingAdded->setMapHeight(graphic->getMapHeight());
    structureBeingAdded->setView(context->guiMgr->getPanelState().addingStructureView);
    structureBeingAdded->setDrawingFlags(IGraphic::DRAWING_FLAG_MASKED);

    mapObjectsBeingAdded.push_back(structureBeingAdded);

    for (int my = mapCoords.y(); my < mapCoords.y() + graphic->getMapHeight(); my++) {
        for (int mx = mapCoords.x(); mx < mapCoords.x() + graphic->getMapWidth(); mx++) {
            mapTilesToDrawTemporarily->getData(mx, my)->mapObject = structureBeingAdded;
        }
    }

    // Hovering beenden. Jetzt ist die linke Maustaste runtergedrückt und wir adden immer zu mapObjectsBeingAdded.
    if (mapObjectBeingAddedHovering != nullptr) {
        delete mapObjectBeingAddedHovering;
        mapObjectBeingAddedHovering = nullptr;

        // Einzugsbereich wird am ersten Gebäude in mapObjectsBeingAdded gezeichnet
        Structure* firstStructureInList = mapObjectsBeingAdded.front();
        buildingToDrawCatchmentArea = dynamic_cast<Building*>(firstStructureInList);
    }

    // TODO Info an Resourcen-Leiste, damit die Kosten für die mehreren Objekte angezeigt werden
}

void GuiMap::updateMapObjectsTemporarilyDrawingFlags() {
    // Hovering? Dann gehts nur um ein Gebäude?
    if (mapObjectBeingAddedHovering != nullptr) {
        const MapCoords& mapCoords = mapObjectBeingAddedHovering->getMapCoords();
        const StructureType structureType = mapObjectBeingAddedHovering->getStructureType();
        const FourthDirection& view = mapObjectBeingAddedHovering->getView();

        if (isAllowedToPlaceStructure(mapCoords, structureType, view) & PLACING_STRUCTURE_NO_RESOURCES) {
            mapObjectBeingAddedHovering->setDrawingFlags(
                mapObjectBeingAddedHovering->getDrawingFlags() | IGraphic::DRAWING_FLAG_BLINK);
        } else {
            mapObjectBeingAddedHovering->setDrawingFlags(
                mapObjectBeingAddedHovering->getDrawingFlags() & ~IGraphic::DRAWING_FLAG_BLINK);
        }
        return;
    }

    // Ansonsten gucken wir mapObjectsBeingAdded durch

    const Player* currentPlayer = context->game->getCurrentPlayer();

    BuildingCosts sumBuildingCostTilHere;
    bool outOfResources = false;

    // Der Reihe nach alle zu setzenden Gebäude durchgehen und gucken, wann/ob die Resourcen ausgehen
    for (auto iter = mapObjectsBeingAdded.cbegin(); iter != mapObjectsBeingAdded.cend(); iter++) {
        Structure* mapObject = *iter;

        if (!outOfResources) {
            StructureType structureType = mapObject->getStructureType();

            const BuildingConfig* buildingConfig = context->configMgr->getBuildingConfig(structureType);
            const BuildingCosts& buildingCosts = buildingConfig->buildingCosts;

            sumBuildingCostTilHere.coins += buildingCosts.coins;
            sumBuildingCostTilHere.tools += buildingCosts.tools;
            sumBuildingCostTilHere.wood += buildingCosts.wood;
            sumBuildingCostTilHere.bricks += buildingCosts.bricks;

            // TODO aktuell können in mapObjectsBeingAdded Gebäude von verschiedenen Siedlungen sein. Das muss gefixed werden. Nur von einer Siedlung ist erlaubt.
            Colony* colony = context->game->getColony(
                currentPlayer, context->game->getMap()->getMapTileAt(mapObject->getMapCoords())->isle);
            if (currentPlayer->coins < sumBuildingCostTilHere.coins ||
                colony->getGoods(GoodsType::TOOLS).inventory < sumBuildingCostTilHere.tools ||
                colony->getGoods(GoodsType::WOOD).inventory < sumBuildingCostTilHere.wood ||
                colony->getGoods(GoodsType::BRICKS).inventory < sumBuildingCostTilHere.bricks) {

                outOfResources = true;
            }
        }

        if (outOfResources) {
            mapObject->setDrawingFlags(mapObject->getDrawingFlags() | IGraphic::DRAWING_FLAG_BLINK);
        } else {
            mapObject->setDrawingFlags(mapObject->getDrawingFlags() & ~IGraphic::DRAWING_FLAG_BLINK);
        }
    }
}