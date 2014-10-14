#ifndef _MAP_OBJECT_H
#define _MAP_OBJECT_H

#include "game/Player.h"
#include "graphics/GraphicsMgr.h"
#include "map/MapUtils.h"

/**
 * @brief Basisklasse für alles, was sich auf der Karte befinden kann
 */
class MapObject {

public:
    /**
     * @brief zeichnet das Objekt maskiert, d.h. gelbem Overlay für Gebäudeplatzierung
     * @sa #drawingFlags
     */
    static const int DRAWING_FLAG_MASKED = (1 << 0);

    /**
     * @brief zeichnet das Objekt rot eingefärbt
     * @sa #drawingFlags
     */
    static const int DRAWING_FLAG_RED    = (1 << 1);

    /**
     * @brief zeichnet das Objekt blinkend, d.h. entweder wird die Grafik gezeichnet oder nicht
     * @sa #drawingFlags
     */
    static const int DRAWING_FLAG_BLINK  = (1 << 2);

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
    
    /**
     * @brief Spieler, dem das Objekt gehört, oder nullptr für spielerlose Objekte
     */
    Player* player = nullptr;

    /**
     * @brief Flags, die angeben, wie das Objekt gezeichnet wird.
     * Eine Kombination aus DRAWING_FLAG_-Konstanten wird erwartet.
     */
    int drawingFlags = 0;

public:
	virtual ~MapObject() {
	}

	void setMapCoords(int mapX, int mapY, int mapWidth, int mapHeight) {
		this->mapX = mapX;
		this->mapY = mapY;
		this->mapWidth = mapWidth;
		this->mapHeight = mapHeight;
	}

	void getMapCoords(int& mapX, int& mapY) const {
		mapX = this->mapX;
		mapY = this->mapY;
	}

	void getMapCoords(int& mapX, int& mapY, int& mapWidth, int& mapHeight) const {
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
    
    void getScreenCoords(int& screenX, int& screenY) const {
		screenX = this->screenX;
		screenY = this->screenY;
	}

	void getScreenCoords(int& screenX, int& screenY, int& screenWidth, int& screenHeight) const {
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
    void getScreenCoordsCenter(int& screenCenterX, int& screenCenterY) const {
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
    
    Player* getPlayer() const {
        return player;
    }

    void setPlayer(Player* player) {
        this->player = player;
    }

    int getDrawingFlags() const {
        return drawingFlags;
    }

    void setDrawingFlags(int drawingFlags) {
        this->drawingFlags = drawingFlags;
    }
};

#endif
