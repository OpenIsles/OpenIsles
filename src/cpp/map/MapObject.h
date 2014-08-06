#ifndef _MAP_OBJECT_H
#define _MAP_OBJECT_H

#include "map/MapUtils.h"
#include "GraphicsMgr.h"

/**
 * @brief Basisklasse für alles, was sich auf der Karte befinden kann
 */
class MapObject {

protected:
	/**
	 * @brief Map-Koordinaten des Objekts
	 */
	int mapX, mapY;

	/**
	 * @brief Größe des Objekts in Map-Koordinaten
	 */
	int mapWidth, mapHeight;

	/**
	 * @brief berechnete X-Screen-Koordinaten, an dem die Grafik gezeichnet werden muss
	 */
	int screenX, screenY;

	/**
	 * @brief Größe der Grafik
	 */
	int screenWidth, screenHeight;

public:
	virtual ~MapObject() {
	}

	void setMapCoords(int mapX, int mapY, int mapWidth, int mapHeight) {
		this->mapX = mapX;
		this->mapY = mapY;
		this->mapWidth = mapWidth;
		this->mapHeight = mapHeight;
	}

	void getMapCoords(int& mapX, int& mapY) {
		mapX = this->mapX;
		mapY = this->mapY;
	}

	void getMapCoords(int& mapX, int& mapY, int& mapWidth, int& mapHeight) {
		mapX = this->mapX;
		mapY = this->mapY;
		mapWidth = this->mapWidth;
		mapHeight = this->mapHeight;
	}

	void setScreenCoords(int screenX, int screenY, int screenWidth, int screenHeight) {
		this->screenX = screenX;
		this->screenY = screenY;
		this->screenWidth = screenWidth;
		this->screenHeight = screenHeight;
	}

	void getScreenCoords(int& screenX, int& screenY, int& screenWidth, int& screenHeight) {
		screenX = this->screenX;
		screenY = this->screenY;
		screenWidth = this->screenWidth;
		screenHeight = this->screenHeight;
	}
    
    /**
     * Ermittelt de Screen-Koordinaten des Mittelpunkts des Objekts
     * @param screenCenterX Referenz, die die X-Screen-Koordinate des Mittelpunkts empfängt
     * @param screenCenterY Referenz, die die Y-Screen-Koordinate des Mittelpunkts empfängt
     */
    void getScreenCoordsCenter(int& screenCenterX, int& screenCenterY) {
        // Mittelpunkt aus den beiden Screen-Koordinaten an den beiden Ecken des Objekts bestimmen
        int screenXUpperLeft, screenYUpperLeft;
        int screenXLowerRight, screenYLowerRight;
        
        MapUtils::mapToScreenCoords(mapX, mapY, screenXUpperLeft, screenYUpperLeft);
        MapUtils::mapToScreenCoords(mapX + mapWidth - 1, mapY + mapHeight - 1, screenXLowerRight, screenYLowerRight);
        
        screenCenterX = (screenXUpperLeft + screenXLowerRight) / 2;
        screenCenterY = (screenYUpperLeft + screenYLowerRight) / 2;
        
        // ScreenCoords sind in der oberen linken Ecke einer Kachel.
        // Wir wollen die Mitte der Kachel, also noch ne halbe Kacheln draufaddieren.
        screenCenterX += GraphicsMgr::TILE_WIDTH_HALF;
        screenCenterY += GraphicsMgr::TILE_HEIGHT_HALF;
    }

};

#endif
