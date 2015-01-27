#include <algorithm>
#include <string>
#include "game/Game.h"
#include "graphics/mgr/IGraphicsMgr.h"
#include "map/Building.h"
#include "map/Map.h"
#include "utils/Rect.h"


void MapCoordUtils::mapToScreenCoords(int mapX, int mapY, int& screenX, int& screenY) {
	screenX = (mapX - mapY) * IGraphicsMgr::TILE_WIDTH_HALF;
	screenY = (mapX + mapY) * IGraphicsMgr::TILE_HEIGHT_HALF;
}

void MapCoordUtils::mapToScreenCoords(double mapX, double mapY, int& screenX, int& screenY) {
    screenX = (int) ((mapX - mapY) * IGraphicsMgr::TILE_WIDTH_HALF);
    screenY = (int) ((mapX + mapY) * IGraphicsMgr::TILE_HEIGHT_HALF);
}

void MapCoordUtils::mapToScreenCoordsCenter(const MapCoords& mapCoords, int& screenX, int& screenY) {
    // TODO mapCoords.toScreenCoordsCenter()
	screenX = (mapCoords.x() - mapCoords.y()) * IGraphicsMgr::TILE_WIDTH_HALF + IGraphicsMgr::TILE_WIDTH_HALF;
	screenY = (mapCoords.x() + mapCoords.y()) * IGraphicsMgr::TILE_HEIGHT_HALF + IGraphicsMgr::TILE_HEIGHT_HALF;

    // TODO ScreenCoords abschaffen
}

MapCoords MapCoordUtils::screenToMapCoords(int screenX, int screenY) {
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

	double xDouble = (double) screenX / (double) IGraphicsMgr::TILE_WIDTH;
	double yDouble = (double) screenY / (double) IGraphicsMgr::TILE_HEIGHT;
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

void MapCoordUtils::screenToDrawCoords(
	Map* map, int screenX, int screenY, int elevation, const IGraphic* graphic, Rect* rect) {

    rect->x = screenX - (
        graphic->getWidth() - (graphic->getMapWidth() + 1) * IGraphicsMgr::TILE_WIDTH_HALF);
    rect->y = screenY - (
        graphic->getHeight() - (graphic->getMapWidth() + graphic->getMapHeight()) * IGraphicsMgr::TILE_HEIGHT_HALF);

    // Elevation
    rect->y -= elevation * IGraphicsMgr::ELEVATION_HEIGHT;

    // Scrolling-Offset anwenden
    rect->x -= map->getScreenOffsetX();
    rect->y -= map->getScreenOffsetY();

    int screenZoom = map->getScreenZoom();
    rect->x /= screenZoom;
    rect->y /= screenZoom;

    // Größe ist gleich der Grafikgröße
    rect->w = graphic->getWidth() / screenZoom;
    rect->h = graphic->getHeight() / screenZoom;
}

void MapCoordUtils::mapToDrawCoords(
	Map* map, int mapX, int mapY, int elevation, const IGraphic* graphic, Rect* rect) {

    int screenX, screenY;
    mapToScreenCoords(mapX, mapY, screenX, screenY);

    screenToDrawCoords(map, screenX, screenY, elevation, graphic, rect);
}

void MapCoordUtils::mapToDrawCoords(
	Map* map, double mapX, double mapY, int elevation, const IGraphic* graphic, Rect* rect) {

    int screenX, screenY;
    mapToScreenCoords(mapX, mapY, screenX, screenY);

    screenToDrawCoords(map, screenX, screenY, elevation, graphic, rect);
}

void MapCoordUtils::getDrawCoordsForBuilding(Map* map, IGraphicsMgr* graphicsMgr, Building* building, Rect* rect) {
    const MapCoords& mapCoords = building->getMapCoords();

	const std::string& viewName = building->getView().getViewName();
	const std::string graphicSetName = graphicsMgr->getGraphicSetNameForStructure(building->getStructureType());
	const GraphicSet* graphicsSet = graphicsMgr->getGraphicSet(graphicSetName);
    const IGraphic* graphic = graphicsSet->getByView(viewName)->getGraphic();

    const int elevation = 1; // TODO für Gebäude wie Anlegestelle, Fischerhütte etc. muss auf 0 gesetzt werden

    // TODO mapCoords.toDrawCoords()
    mapToDrawCoords(map, mapCoords.x(), mapCoords.y(), elevation, graphic, rect);
}

MapCoords MapCoordUtils::getMapCoordsUnderMouse(Map* map, int mouseCurrentX, int mouseCurrentY) {
    int screenZoom = map->getScreenZoom();

    int mouseScreenX = (mouseCurrentX * screenZoom) + map->getScreenOffsetX();
    int mouseScreenY = (mouseCurrentY * screenZoom) + map->getScreenOffsetY();

    // Wir machen es wie Anno 1602: Für die Position, wo der Mauszeiger steht, wird immer die elevatete Position
    // genommen, selbst, wenn wir uns auf dem Wasser befinden.
    const int elevation = 1;
    return screenToMapCoords(mouseScreenX, mouseScreenY + elevation * IGraphicsMgr::ELEVATION_HEIGHT / screenZoom);
}