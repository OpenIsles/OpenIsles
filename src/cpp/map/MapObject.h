#ifndef _MAP_OBJECT_H
#define _MAP_OBJECT_H

#include "game/Player.h"
#include "graphics/mgr/IGraphicsMgr.h"
#include "map/MapCoordUtils.h"

/**
 * @brief Basisklasse für alles, was sich auf der Karte befinden kann
 */
class MapObject {

    friend class EconomicsMgr; // EconomicsMgr soll zum Aktualisieren des Trägers einfach zugreifen können

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
     * @brief Spieler, dem das Objekt gehört, oder nullptr für spielerlose Objekte
     */
    Player* player = nullptr;

    /**
     * @brief Flags, die angeben, wie das Objekt gezeichnet wird.
     * Eine Kombination aus MapObjectGraphic::DRAWING_FLAG_-Konstanten wird erwartet.
     */
    int drawingFlags = 0;

public:
	virtual ~MapObject() {
	}

    void setMapCoords(int mapX, int mapY) {
        this->mapX = mapX;
        this->mapY = mapY;
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
