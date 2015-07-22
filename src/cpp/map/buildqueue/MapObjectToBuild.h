#ifndef _MAP_OBJECT_TO_BUILD_H
#define _MAP_OBJECT_TO_BUILD_H

#include "map/Direction.h"
#include "map/MapObject.h"
#include "map/MapObjectType.h"

/**
 * @brief Hilfsstruktur, mit der wir ein zu bauendes Objekt beschreiben.
 *
 * Sie enthält nur die Parameter (Wo, Was, Ausrichtung) des neuen Map-Objekts.
 * Damit sparen wir uns die korrekten Objekte (Building, Structure, Harvestable) zu unterscheiden und
 * komplett zu befüllen.
 */
struct MapObjectToBuild {

public:
    MapObjectToBuild(const MapCoords& mapCoords, const MapObjectType* mapObjectType, FourthDirection view) :
        mapCoords(mapCoords), mapObjectType(mapObjectType), view(view), mode(BUILD) {}

    MapObjectToBuild(const MapCoords& mapCoords) :
        mapCoords(mapCoords), mapObjectType(nullptr), view(Direction::NONE), mode(DEMOLISH) {}

public:
    MapCoords mapCoords;
    const MapObjectType* mapObjectType;
    FourthDirection view;

    enum {
        BUILD,
        DEMOLISH
    } mode;
};

#endif