#include "map/Building.h"
#include "map/Harvestable.h"
#include "map/MapObject.h"
#include "map/Street.h"
#include "map/Structure.h"


MapObjectFixed* MapObjectFixed::instantiate(const MapObjectType* mapObjectType) {
    MapObjectFixed* mapObjectFixed;

    if (mapObjectType->type == MapObjectTypeClass::HARVESTABLE) {
        mapObjectFixed = new Harvestable(mapObjectType->maxAge);
    } else if (mapObjectType->type == MapObjectTypeClass::STRUCTURE) {
        mapObjectFixed = new Structure();
    } else if (mapObjectType->type == MapObjectTypeClass::STREET) {
        mapObjectFixed = new Street();
    } else if (mapObjectType->type == MapObjectTypeClass::BUILDING) {
        mapObjectFixed = new Building();
    } else {
        assert(false);
        return nullptr;
    }

    mapObjectFixed->setMapObjectType(mapObjectType);
    return mapObjectFixed;
}