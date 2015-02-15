#include <algorithm>
#include <cassert>
#include <cmath>
#include <string>
#include "map/Building.h"
#include "map/Map.h"


ScreenCoords MapCoordUtils::mapToScreenCoords(
    const MapCoords& mapCoords, const FourDirectionsView& screenView, const Map& map) {

    return mapToScreenCoords((const DoubleMapCoords&) mapCoords, screenView, map);
}

ScreenCoords MapCoordUtils::mapToScreenCoords(
    const DoubleMapCoords& mapCoords, const FourDirectionsView& screenView, const Map& map) {

    const MapCoords& mapCoordsCentered = map.getMapCoordsCentered();
    double mapXDiff, mapYDiff;

    // Entsprechend der Ansicht die Entfernung zur Center-Kachel bestimmen
    if (screenView == "south") {
        mapXDiff = mapCoords.x() - mapCoordsCentered.x();
        mapYDiff = mapCoords.y() - mapCoordsCentered.y();
    } else if (screenView == "east") {
        mapXDiff = -(mapCoords.y() - mapCoordsCentered.y());
        mapYDiff = mapCoords.x() - mapCoordsCentered.x();
    } else if (screenView == "north") {
        mapXDiff = -(mapCoords.x() - mapCoordsCentered.x());
        mapYDiff = -(mapCoords.y() - mapCoordsCentered.y());
    } else if (screenView == "west") {
        mapXDiff = mapCoords.y() - mapCoordsCentered.y();
        mapYDiff = -(mapCoords.x() - mapCoordsCentered.x());
    }
    else {
        assert(false);
        mapXDiff = mapYDiff = 0;
    }

    double screenX = (mapXDiff - mapYDiff) * (double) IGraphicsMgr::TILE_WIDTH_HALF;
    double screenY = (mapXDiff + mapYDiff) * (double) IGraphicsMgr::TILE_HEIGHT_HALF;

    return ScreenCoords((int) screenX, (int) screenY);
}

MapCoords MapCoordUtils::screenToMapCoords(
    const ScreenCoords& screenCoords, const FourDirectionsView& screenView, const Map& map) {

    int mapXDiffInSouthView = (int) std::floor((screenCoords.x() / (double) IGraphicsMgr::TILE_WIDTH) +
                                               (screenCoords.y() / (double) IGraphicsMgr::TILE_HEIGHT));
    int mapYDiffInSouthView = (int) std::floor((-screenCoords.x() / (double) IGraphicsMgr::TILE_WIDTH) +
                                               (screenCoords.y() / (double) IGraphicsMgr::TILE_HEIGHT));

    // Entsprechend der Ansicht umrechnen (= Invers-Operation zu der in mapToScreenCoords())
    int mapXDiff, mapYDiff;
    if (screenView == "south") {
        mapXDiff = mapXDiffInSouthView;
        mapYDiff = mapYDiffInSouthView;
    } else if (screenView == "east") {
        mapXDiff = mapYDiffInSouthView;
        mapYDiff = -mapXDiffInSouthView;
    } else if (screenView == "north") {
        mapXDiff = -mapXDiffInSouthView;
        mapYDiff = -mapYDiffInSouthView;
    } else if (screenView == "west") {
        mapXDiff = -mapYDiffInSouthView;
        mapYDiff = mapXDiffInSouthView;
    }
    else {
        assert(false);
        mapXDiff = mapYDiff = 0;
    }

    const MapCoords& mapCoordsCentered = map.getMapCoordsCentered();
    return MapCoords(mapCoordsCentered.x() + mapXDiff, mapCoordsCentered.y() + mapYDiff);
}

Rect MapCoordUtils::mapToDrawCoords(
    const DoubleMapCoords& mapCoords, const Map& map, int elevation, const IGraphic& graphic) {

    ScreenCoords screenCoords = mapToScreenCoords(mapCoords, map.getScreenView(), map);

    return screenToDrawCoords(screenCoords, map, elevation, graphic);
}

Rect MapCoordUtils::mapToDrawCoords(
    const MapCoords& mapCoords, const Map& map, int elevation, const IGraphic& graphic) {

    ScreenCoords screenCoords = mapToScreenCoords(mapCoords, map.getScreenView(), map);

    return screenToDrawCoords(screenCoords, map, elevation, graphic);
}

Rect MapCoordUtils::getDrawCoordsForBuilding(const Map& map, IGraphicsMgr* graphicsMgr, const Building* building) {
    const MapCoords& mapCoords = building->getMapCoords();

    const FourDirectionsView& structureView = building->getView();
    const FourDirectionsView& viewToRender = structureView + map.getScreenView();

	const std::string graphicSetName = graphicsMgr->getGraphicSetNameForStructure(building->getStructureType());
	const GraphicSet* graphicsSet = graphicsMgr->getGraphicSet(graphicSetName);
    const IGraphic* graphic = graphicsSet->getByView(viewToRender.getViewName())->getGraphic();

    const int elevation = 1; // TODO für Gebäude wie Anlegestelle, Fischerhütte etc. muss auf 0 gesetzt werden

    return mapToDrawCoords(mapCoords, map, elevation, *graphic);
}

ScreenCoords MapCoordUtils::getScreenCoordsUnderMouse(const Map& map, int mouseCurrentX, int mouseCurrentY) {
    int screenZoom = map.getScreenZoom();
    int screenX = (mouseCurrentX - Consts::mapClipRect.w / 2) * screenZoom;
    int screenY = (mouseCurrentY - Consts::mapClipRect.h / 2) * screenZoom;

    return ScreenCoords(screenX, screenY);
}

MapCoords MapCoordUtils::getMapCoordsUnderMouse(const Map& map, int mouseCurrentX, int mouseCurrentY) {
    ScreenCoords mouseScreenCoords = getScreenCoordsUnderMouse(map, mouseCurrentX, mouseCurrentY);

    // Wir machen es wie Anno 1602: Für die Position, wo der Mauszeiger steht, wird immer die elevatete Position
    // genommen, selbst, wenn wir uns auf dem Wasser befinden.
    const int elevation = 1;

    int screenZoom = map.getScreenZoom();
    mouseScreenCoords.addY(elevation * IGraphicsMgr::ELEVATION_HEIGHT / screenZoom);

    return screenToMapCoords(mouseScreenCoords, map.getScreenView(), map);
}

void MapCoordUtils::getMapCoordsInScreenEdges(const Map& map,
    MapCoords& mapCoordsTopLeft, MapCoords& mapCoordsTopRight,
    MapCoords& mapCoordsBottomLeft, MapCoords& mapCoordsBottomRight) {

    const MapCoords& mapCoordsCentered = map.getMapCoordsCentered();
    int screenHalfWidth = (Consts::mapClipRect.h / 2) * map.getScreenZoom();
    int screenHalfHeight = (Consts::mapClipRect.w / 2) * map.getScreenZoom();


    int mapXDistance = (int) ((screenHalfWidth / (double) IGraphicsMgr::TILE_WIDTH) +
                              (screenHalfHeight / (double) IGraphicsMgr::TILE_HEIGHT));
    int mapYDistance = (int) ((-screenHalfWidth / (double) IGraphicsMgr::TILE_WIDTH) +
                              (screenHalfHeight / (double) IGraphicsMgr::TILE_HEIGHT));

    // TODO ggf. was draufrechnen, weil wir abrunden?

    // Ecken-Koordinaten bestimmen
    MapCoords mcTopLeft = MapCoords(mapCoordsCentered.x() - mapXDistance, mapCoordsCentered.y() - mapYDistance);
    MapCoords mcTopRight = MapCoords(mapCoordsCentered.x() + mapYDistance, mapCoordsCentered.y() - mapXDistance);
    MapCoords mcBottomLeft = MapCoords(mapCoordsCentered.x() - mapYDistance, mapCoordsCentered.y() + mapXDistance);
    MapCoords mcBottomRight = MapCoords(mapCoordsCentered.x() + mapXDistance, mapCoordsCentered.y() + mapYDistance);

    // Und je Ansicht der richtigen Ecke zuordnen
    const FourDirectionsView& screenView = map.getScreenView();
    if (screenView == "south") {
        mapCoordsTopLeft = mcTopLeft;
        mapCoordsTopRight = mcTopRight;
        mapCoordsBottomLeft = mcBottomLeft;
        mapCoordsBottomRight = mcBottomRight;
    } else if (screenView == "east") {
        mapCoordsTopLeft = mcBottomLeft;
        mapCoordsTopRight = mcTopLeft;
        mapCoordsBottomLeft = mcBottomRight;
        mapCoordsBottomRight = mcTopRight;
    } else if (screenView == "north") {
        mapCoordsTopLeft = mcBottomRight;
        mapCoordsTopRight = mcBottomLeft;
        mapCoordsBottomLeft = mcTopRight;
        mapCoordsBottomRight = mcTopLeft;
    } else if (screenView == "west") {
        mapCoordsTopLeft = mcTopRight;
        mapCoordsTopRight = mcBottomRight;
        mapCoordsBottomLeft = mcTopLeft;
        mapCoordsBottomRight = mcBottomLeft;
    }
    else {
        assert(false);
    }
}

Rect MapCoordUtils::screenToDrawCoords(const ScreenCoords& screenCoords, const Map& map, int elevation, const IGraphic& graphic) {
    Rect drawCoordsRect;

    drawCoordsRect.x = screenCoords.x();
    drawCoordsRect.y = screenCoords.y();

    // Grafik entsprechend ihrer Größe in Map-Koordinaten ausrichten.
    const FourDirectionsView& screenView = map.getScreenView();
    if (screenView == "south") {
        drawCoordsRect.x -= graphic.getWidth() - graphic.getMapWidth() * IGraphicsMgr::TILE_WIDTH_HALF;
        drawCoordsRect.y -= graphic.getHeight() - (graphic.getMapWidth() + graphic.getMapHeight()) * IGraphicsMgr::TILE_HEIGHT_HALF;
    } else if (screenView == "east") {
        drawCoordsRect.x -= graphic.getWidth() - IGraphicsMgr::TILE_WIDTH_HALF;
        drawCoordsRect.y -= graphic.getHeight() - (graphic.getMapHeight() + 1) * IGraphicsMgr::TILE_HEIGHT_HALF;
    } else if (screenView == "north") {
        drawCoordsRect.x -= graphic.getWidth() - graphic.getMapHeight() * IGraphicsMgr::TILE_WIDTH_HALF;
        drawCoordsRect.y -= graphic.getHeight() - IGraphicsMgr::TILE_HEIGHT;
    } else if (screenView == "west") {
        drawCoordsRect.x -= IGraphicsMgr::TILE_WIDTH_HALF;
        drawCoordsRect.y -= graphic.getHeight() - (graphic.getMapWidth() + 1) * IGraphicsMgr::TILE_HEIGHT_HALF;
    } else {
        assert(false);
    }

    // Elevation
    drawCoordsRect.y -= elevation * IGraphicsMgr::ELEVATION_HEIGHT;

    // Zoom anwenden
    int screenZoom = map.getScreenZoom();
    drawCoordsRect.x /= screenZoom;
    drawCoordsRect.y /= screenZoom;

    // Zum Schluss in die Bildschirmmitte bringen
    drawCoordsRect.x += Consts::mapClipRect.w / 2;
    drawCoordsRect.y += Consts::mapClipRect.h / 2;

    // Größe ist gleich der Grafikgröße
    drawCoordsRect.w = graphic.getWidth() / screenZoom;
    drawCoordsRect.h = graphic.getHeight() / screenZoom;

    return drawCoordsRect;
}