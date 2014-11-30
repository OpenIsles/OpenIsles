#ifndef _ISLE_H
#define _ISLE_H

#include <iostream>
#include "map/MapObject.h"
#include "utils/RectangleData.h"
#include "utils/StringFormat.h"

/**
 * Stellt eine Insel dar. Sie wird als Teil in einer Karte (Map) eingesetzt.
 */
class Isle : public MapObject {

private:
    /**
     * Rechteck mit den Kacheln
     */
    RectangleData<unsigned char>* tiles = nullptr;

public:
    /**
	 * @brief Lädt eine Insel von einer tiled-TMX-Datei
	 * @param filename Dateiname der zu ladenden Insel
	 */
	Isle(const char* filename);
	~Isle();
    
    int getWidth() const {
		return tiles->width;
	}

	int getHeight() const {
		return tiles->height;
	}

	unsigned char getTileAt(int x, int y) const {
        if (x < 0 || y < 0 || x >= tiles->width || y >= tiles->height) {
            std::cerr << "mapCoords (" << toString(x) << ", " + toString(y) << ") out of bounds";
        }

        return tiles->getData(x, y, 0);
    }
};

#endif
