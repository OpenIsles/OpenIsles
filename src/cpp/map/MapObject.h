#ifndef _MAP_OBJECT_H
#define _MAP_OBJECT_H

#include "game/Player.h"
#include "graphics/mgr/IGraphicsMgr.h"
#include "map/coords/MapCoords.h"
#include "map/MapCoordUtils.h"

/**
 * @brief Basisklasse für alles, was sich auf der Karte befinden kann
 */
class MapObject {

protected:
	/**
	 * @brief Breite des Objekts in Map-Koordinaten.
     *
     * Info: Dies entspricht immer der Breite in der Ausrichtung des Gebäudes. Z.B. hat ein 3x2-Gebäude, was
     * auf Ost/West-Ansicht gedreht wurde, hat eine mapWidth von 2. Auf Süd/Nord-Ansicht hat es eine mapWidth von 3.
     * Dies ist analog der zugehörigen Grafik.
	 */
	int mapWidth;

    /**
	 * @brief Höhe des Objekts in Map-Koordinaten
     *
     * Info: Dies entspricht immer der Höhe in der Ausrichtung des Gebäudes. Z.B. hat ein 3x2-Gebäude, was
     * auf Ost/West-Ansicht gedreht wurde, hat eine mapHeight von 3. Auf Süd/Nord-Ansicht hat es eine mapHeight von 2.
     * Dies ist analog der zugehörigen Grafik.
	 */
    int mapHeight;
    
    /**
     * @brief Spieler, dem das Objekt gehört, oder nullptr für spielerlose Objekte
     */
    Player* player = nullptr;

    /**
     * @brief Flags, die angeben, wie das Objekt gezeichnet wird.
     * Eine Kombination aus IGraphic::DRAWING_FLAG_-Konstanten wird erwartet.
     */
    int drawingFlags = 0;

public:
	virtual ~MapObject() {
	}

    int getMapWidth() const {
        return mapWidth;
    }

    virtual void setMapWidth(int mapWidth) {
        this->mapWidth = mapWidth;
    }

    int getMapHeight() const {
        return mapHeight;
    }

    virtual void setMapHeight(int mapHeight) {
        this->mapHeight = mapHeight;
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


/**
 * @brief Basisklasse für alles, was sich fix auf der Karte befinden kann. Fixe Objekte haben immer ganzzahlige
 * Map-Koordinaten
 */
class MapObjectFixed : public MapObject {

protected:
	/**
	 * @brief Map-Koordinaten des Objekts
	 */
	MapCoords mapCoords;

public:
	virtual ~MapObjectFixed() {
	}

    void setMapCoords(const MapCoords& mapCoords) {
        this->mapCoords = mapCoords;
    }

    MapCoords& getMapCoords() {
		return mapCoords;
	}

	const MapCoords& getMapCoords() const {
		return mapCoords;
	}
};


/**
 * @brief Basisklasse für alles, was sich auf der Karte bewegen kann. Bewegliche Objekte haben immer
 * Gleitkomma-Map-Koordinaten, da sie sich fließend von Kachel zu Kachel bewegen.
 */
class MapObjectMoving : public MapObject {

protected:
	/**
	 * @brief Map-Koordinaten des Objekts
	 */
	DoubleMapCoords mapCoords;

public:
	virtual ~MapObjectMoving() {
	}

    void setMapCoords(const DoubleMapCoords& mapCoords) {
        this->mapCoords = mapCoords;
    }

    DoubleMapCoords& getMapCoords() {
		return mapCoords;
	}

	const DoubleMapCoords& getMapCoords() const {
		return mapCoords;
	}
};

#endif
