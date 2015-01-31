#include <algorithm>
#include <string>
#include "map/Building.h"
#include "map/Map.h"


ScreenCoords MapCoordUtils::mapToScreenCoords(const MapCoords& mapCoords) {
	int screenX = (mapCoords.x() - mapCoords.y()) * IGraphicsMgr::TILE_WIDTH_HALF;
	int screenY = (mapCoords.x() + mapCoords.y()) * IGraphicsMgr::TILE_HEIGHT_HALF;
    return ScreenCoords(screenX, screenY);
}

ScreenCoords MapCoordUtils::mapToScreenCoords(const DoubleMapCoords& mapCoords) {
	int screenX = (int) ((mapCoords.x() - mapCoords.y()) * (double) IGraphicsMgr::TILE_WIDTH_HALF);
	int screenY = (int) ((mapCoords.x() + mapCoords.y()) * (double) IGraphicsMgr::TILE_HEIGHT_HALF);
    return ScreenCoords(screenX, screenY);
}

ScreenCoords MapCoordUtils::mapToScreenCoordsCenter(const MapCoords& mapCoords) {
    ScreenCoords screenCoords = mapToScreenCoords(mapCoords);
    screenCoords.addX(IGraphicsMgr::TILE_WIDTH_HALF);
    screenCoords.addY(IGraphicsMgr::TILE_HEIGHT_HALF);
    return screenCoords;
}

MapCoords MapCoordUtils::screenToMapCoords(const ScreenCoords& screenCoords) {
	/*
	 * Screen-Koordinaten erst in ein (TILE_WIDTH x TILE_HEIGHT)-Koordinatensystem runterrechnen
	 * Dann gibts 8 Fälle und wir haben es. Ohne unperformante Matrizen und Projektionen :-)
	 *
	 * |          TILE_WIDTH           |
	 * |   xDiff<0.5       xDiff>0.5   |
	 * O-------------------------------O-------------
	 * | x-1          /|\          y-1 |
	 * |   [1]   /---- | ----\   [6]   | yDiff<0.5
	 * |    /----  [2] | [5]  ----\    |
	 * |---------------+---------------|  TILE_HEIGHT
	 * |    \----  [4] | [7]  ----/    |
	 * |   [3]   \---- | ----/   [8]   | yDiff>0.5
	 * | y+1          \|/          x+1 |
	 * O-------------------------------O-------------
	 *
	 * O = obere linke Ecke der Kacheln = Punkt, der mit mapToScreenCoords() berechnet wird.
	 *     Entspricht genau den (TILE_WIDTH x TILE_HEIGHT)-Koordinatensystem-Punkten
	 * [x] = 8 Fälle, siehe if-Block unten
	 *
	 * x/yDouble = exakte Koordinate im (TILE_WIDTH x TILE_HEIGHT)-Koordinatensystem.
	 * x/yInt = abgerundete Koordinate im (TILE_WIDTH x TILE_HEIGHT)-Koordinatensystem, entspricht den O-Punkten im Bild
	 * x/yDiff = Wert zwischen 0.0 und 1.0, siehe Grafik oben, mit dem die 8 Fälle identifiziert werden
	 *
	 */

	double xDouble = (double) screenCoords.x() / (double) IGraphicsMgr::TILE_WIDTH;
	double yDouble = (double) screenCoords.y() / (double) IGraphicsMgr::TILE_HEIGHT;
	int xInt = (int) floor(xDouble);
	int yInt = (int) floor(yDouble);
	double xDiff = xDouble - (double) xInt;
	double yDiff = yDouble - (double) yInt;

	// preliminaryMap-Koordinate = Map-Koordinate der Kachel, die ggf. noch in 4 von 8 Fällen angepasst werden muss
	int preliminaryMapX = xInt + yInt;
	int preliminaryMapY = yInt - xInt;

	// Jetzt die 8 Fälle durchgehen und ggf. noch plus-minus 1 auf x oder y
    int mapX, mapY;
	if (xDiff < 0.5) {
		if (yDiff < 0.5) {
			if (xDiff < 0.5 - yDiff) {
				// Fall 1
				mapX = preliminaryMapX - 1;
				mapY = preliminaryMapY;
			} else {
				// Fall 2
				mapX = preliminaryMapX;
				mapY = preliminaryMapY;
			}
		} else {
			if (xDiff < yDiff - 0.5) {
				// Fall 3
				mapX = preliminaryMapX;
				mapY = preliminaryMapY + 1;
			} else {
				// Fall 4
				mapX = preliminaryMapX;
				mapY = preliminaryMapY;
			}
		}
	} else {
		if (yDiff < 0.5) {
			if (xDiff - 0.5 < yDiff) {
				// Fall 5
				mapX = preliminaryMapX;
				mapY = preliminaryMapY;
			} else {
				// Fall 6
				mapX = preliminaryMapX;
				mapY = preliminaryMapY - 1;
			}
		} else {
			if (xDiff - 0.5 < 1 - yDiff) {
				// Fall 7
				mapX = preliminaryMapX;
				mapY = preliminaryMapY;
			} else {
				// Fall 8
				mapX = preliminaryMapX + 1;
				mapY = preliminaryMapY;
			}
		}
	}

    return MapCoords(mapX, mapY);
}

Rect MapCoordUtils::mapToDrawCoords(const DoubleMapCoords& mapCoords, const Map& map, int elevation, const IGraphic& graphic) {
    ScreenCoords screenCoords = mapToScreenCoords(mapCoords);
    return screenToDrawCoords(screenCoords, map, elevation, graphic);
}

Rect MapCoordUtils::mapToDrawCoords(const MapCoords& mapCoords, const Map& map, int elevation, const IGraphic& graphic) {
    ScreenCoords screenCoords = mapToScreenCoords(mapCoords);
    return screenToDrawCoords(screenCoords, map, elevation, graphic);
}

Rect MapCoordUtils::getDrawCoordsForBuilding(const Map& map, IGraphicsMgr* graphicsMgr, Building* building) {
    const MapCoords& mapCoords = building->getMapCoords();

	const std::string& viewName = building->getView().getViewName();
	const std::string graphicSetName = graphicsMgr->getGraphicSetNameForStructure(building->getStructureType());
	const GraphicSet* graphicsSet = graphicsMgr->getGraphicSet(graphicSetName);
    const IGraphic* graphic = graphicsSet->getByView(viewName)->getGraphic();

    const int elevation = 1; // TODO für Gebäude wie Anlegestelle, Fischerhütte etc. muss auf 0 gesetzt werden

    return mapToDrawCoords(mapCoords, map, elevation, *graphic);
}

MapCoords MapCoordUtils::getMapCoordsUnderMouse(const Map& map, int mouseCurrentX, int mouseCurrentY) {
    int screenZoom = map.getScreenZoom();

    const ScreenCoords& screenCoordsOffset = map.getScreenCoordsOffset();
    int mouseScreenX = (mouseCurrentX * screenZoom) + screenCoordsOffset.x();
    int mouseScreenY = (mouseCurrentY * screenZoom) + screenCoordsOffset.y();

    // Wir machen es wie Anno 1602: Für die Position, wo der Mauszeiger steht, wird immer die elevatete Position
    // genommen, selbst, wenn wir uns auf dem Wasser befinden.
    const int elevation = 1;

    ScreenCoords mouseScreenCoords(mouseScreenX, mouseScreenY + elevation * IGraphicsMgr::ELEVATION_HEIGHT / screenZoom);
    return screenToMapCoords(mouseScreenCoords);
}

Rect MapCoordUtils::screenToDrawCoords(const ScreenCoords& screenCoords, const Map& map, int elevation, const IGraphic& graphic) {
    Rect drawCoordsRect;

    drawCoordsRect.x = screenCoords.x() - (
        graphic.getWidth() - (graphic.getMapWidth() + 1) * IGraphicsMgr::TILE_WIDTH_HALF);
    drawCoordsRect.y = screenCoords.y() - (
        graphic.getHeight() - (graphic.getMapWidth() + graphic.getMapHeight()) * IGraphicsMgr::TILE_HEIGHT_HALF);

    // Elevation
    drawCoordsRect.y -= elevation * IGraphicsMgr::ELEVATION_HEIGHT;

    // Scrolling-Offset anwenden
    const ScreenCoords& screenCoordsOffset = map.getScreenCoordsOffset();
    drawCoordsRect.x -= screenCoordsOffset.x();
    drawCoordsRect.y -= screenCoordsOffset.y();

    int screenZoom = map.getScreenZoom();
    drawCoordsRect.x /= screenZoom;
    drawCoordsRect.y /= screenZoom;

    // Größe ist gleich der Grafikgröße
    drawCoordsRect.w = graphic.getWidth() / screenZoom;
    drawCoordsRect.h = graphic.getHeight() / screenZoom;

    return drawCoordsRect;
}