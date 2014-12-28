#ifndef _ABSTRACT_MAP_OBJECT_GRAPHIC_H
#define _ABSTRACT_MAP_OBJECT_GRAPHIC_H

#include "graphics/graphic/IMapObjectGraphic.h"
#include "graphics/renderer/IRenderer.h"
#include "utils/Rect.h"

/**
 * @brief Abstrakter MapObject-Grafik, der die gemeinsame Funktionalität zwischen dem
 * `SDLMapObjectGraphic` und dem `NoSDLMapObjectGraphic` enthält.
 */
class AbstractMapObjectGraphic : public virtual IMapObjectGraphic {

protected:
    /**
     * @brief Breite (X-Richtung) der Grafik in Map-Koordinaten
     */
    unsigned char mapWidth;

    /**
     * @brief Höhe (Y-Richtung) der Grafik in Map-Koordinaten
     */
    unsigned char mapHeight;

public:
    /**
     * @brief Konstruktor
     * @param mapWidth Breite in mapCoords
     * @param mapHeight Höhe in mapCoords
     */
    AbstractMapObjectGraphic(unsigned char mapWidth, unsigned char mapHeight) :
        mapWidth(mapWidth), mapHeight(mapHeight) {}

    virtual ~AbstractMapObjectGraphic() {}

    /**
     * @brief Liefert die Breite der Grafik in Map-Koordinaten zurück
     * @return Breite der Grafik in Map-Koordinaten
     */
    virtual unsigned char getMapWidth() const {
        return mapWidth;
    }

    /**
     * @brief Liefert die Höhe der Grafik in Map-Koordinaten zurück
     * @return Breite der Höhe in Map-Koordinaten
     */
    virtual unsigned char getMapHeight() const {
        return mapHeight;
    }


    virtual void draw(Rect* rectSource, Rect* rectDestination, int drawingFlags, uint32_t sdlTicks) = 0;

};

#endif
