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
     * @brief Liefert die Animation für ein Map-Objekt
     *
     * @param mapObject Map-Objekt
     * @param screenView aktuelle Ansicht der Karte
     * @return zu verwendende Animation
     */
    static const Animation* getAnimation(const MapObject& mapObject, const FourthDirection& screenView);

    /**
     * @brief Liefert die Grafik für ein Map-Objekt
     *
     * @param mapObject Map-Objekt
     * @param screenView aktuelle Ansicht der Karte
     * @return zu verwendende Grafik
     */
    static const IGraphic* getGraphic(const MapObject& mapObject, const FourthDirection& screenView);

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
    static const IGraphic* getGraphic(const MapObjectType* mapObjectType, const EighthDirection& view);

    /**
     * Aktualisiert MapObject::animationFrame entsprechend der vergangenen Zeit seit dem letzten Update
     *
     * @param mapObject aktualisierendes Map-Objekt
     * @param animation Animation, die verwendet wird (für FPS und Anzahl Frames)
     * @param context (Dependency)
     */
    static void animateObject(MapObject& mapObject, const Animation* animation, const Context& context);

};

#endif
