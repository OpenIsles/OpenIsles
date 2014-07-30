#ifndef _MAP_OBJECT_H
#define _MAP_OBJECT_H

/**
 * @brief Basisklasse für alles, was sich auf der Karte befinden kann
 */
class MapObject {

private:
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

};

#endif
