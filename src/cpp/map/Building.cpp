#include "map/Building.h"
#include "map/Map.h"
#include "map/MapUtils.h"

/**
 * @brief die Karte
 */
extern Map* map;


void Building::onClick(int mouseXInBuilding, int mouseYInBuilding) {
	std::cout << "Klick auf Building " << std::to_string(this->object) << "@(" << std::to_string(mouseXInBuilding)
			<< ", " << std::to_string(mouseYInBuilding) << ")" << std::endl;

	map->setSelectedMapObject(this);
}

bool Building::isInsideCatchmentArea(int mapX, int mapY) {
    // Wir müssen in Screen-Koordinaten rechnen. Der Einzugsbereich ist eine 2:1-Ellipse um den Gebäude-Mittelpunkt.
    int screenX, screenY, thisScreenCenterX, thisScreenCenterY;
    
    MapUtils::mapToScreenCoordsCenter(mapX, mapY, screenX, screenY);
    getScreenCoordsCenter(thisScreenCenterX, thisScreenCenterY);
    
    /* Ellipsengleichung:
     *   (x - x0)²   (y - y0)²
     *   --------- + --------- = 1
     *       a²          b² 
     */

    float radiusA = ((float)catchmentAreaRadius + 0.25) * GraphicsMgr::TILE_WIDTH_HALF;
    float radiusB = ((float)catchmentAreaRadius + 0.25) * GraphicsMgr::TILE_HEIGHT_HALF;
    
    return ( ((float)(screenX - thisScreenCenterX) * (screenX - thisScreenCenterX) /
              (radiusA * radiusA)) +
             ((float)(screenY - thisScreenCenterY) * (screenY - thisScreenCenterY) /
              (radiusB * radiusB))
            <= 1);
}

bool Building::isInsideCatchmentArea(MapObject* mapObject) {
    int testX, testY, testWidth, testHeight;
    mapObject->getMapCoords(testX, testY, testWidth, testHeight);

    for (int y = testY; y < testY + testHeight; y++) {
        for (int x = testX; x < testX + testWidth; x++) {
            if (isInsideCatchmentArea(x, y)) {
                return true;
            }
        }
    }
    return false;
}