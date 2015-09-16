#include <cassert>
#include <climits>
#include "game/Game.h"
#include "map/Building.h"
#include "map/Harvestable.h"
#include "map/MapObject.h"
#include "map/Street.h"
#include "map/Structure.h"


unsigned int MapObject::getTicksSinceLastUpdate(const Context& context) const {
    unsigned long ticksSinceLastUpdate = context.game->getTicks() - lastUpdateTicks;

    // Der Zeitraum zwischen zwei Aktualisierungen wird niemals extrem groß sein! Wir casten ulong->uint.
    assert(ticksSinceLastUpdate <= UINT_MAX);

    return (unsigned int) ticksSinceLastUpdate;
}

bool MapObject::update(const Context& context) {
    bool notDeleteMe = updateObject(context);
    lastUpdateTicks = context.game->getTicks();
    return notDeleteMe;
}


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