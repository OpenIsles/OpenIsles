#ifndef _ISLE_H
#define _ISLE_H

#include "global.h"
#include "map/MapObject.h"
#include "utils/RectangleData.h"
#include "utils/StringFormat.h"

/**
 * Stellt eine Insel dar. Sie wird als Teil in einer Karte (Map) eingesetzt.
 */
class Isle : public MapObjectFixed {

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

    virtual bool updateObject(const Context& context) override {
        return true;
    }
    
    int getWidth() const {
        return tiles->width;
    }

    int getHeight() const {
        return tiles->height;
    }

    unsigned char getTileAt(int x, int y) const {
        if (x < 0 || y < 0 || x >= tiles->width || y >= tiles->height) {
            Log::error(_("mapCoords (%d, %d) out of bounds."), x, y);
        }

        return tiles->getData(x, y, 0);
    }
};

#endif
