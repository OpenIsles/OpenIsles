#ifndef _MAP_OBJECT_UTILS_H
#define _MAP_OBJECT_UTILS_H

#include "global.h"
#include "graphics/graphic/IGraphic.h"
#include "map/MapObject.h"
#include "map/MapObjectType.h"


/**
 * @brief Helper-Klasse für MapObjects
 */
class MapObjectUtils {

public:
    /**
     * @brief Liefert die Grafik für ein Map-Objekt
     *
     * @param mapObject Map-Objekt
     * @param view zu verwendende View
     * @return zu verwendende Grafik
     */
    static const IGraphic* getGraphic(const MapObject& mapObject, const FourthDirection& view);

    /**
     * @brief Liefert die Grafik für einen Map-Objekt-Typ. Das wird für Baumenü verwendet, wo wir keine konkrete
     * MapObject-Instanz haben.
     *
     * Für Straßen wird hierbei immer eine Gerade genommen.
     * Harvestable werden in ihren maximalen Wachstum gezeigt.
     *
     * @param mapObjectType Map-Objekt-Typ
     * @param view zu verwendende View
     * @return zu verwendende Grafik
     */
    static const IGraphic* getGraphic(const MapObjectType* mapObjectType, const FourthDirection& view);

};

#endif
