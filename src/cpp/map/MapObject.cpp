#include <cassert>
#include <climits>
#include "game/Game.h"
#include "map/Building.h"
#include "map/Harvestable.h"
#include "map/MapObject.h"
#include "map/Ship.h"
#include "map/Street.h"
#include "map/Structure.h"


MapObject* MapObject::instantiate(const MapObjectType* mapObjectType) {
    MapObject* mapObject;

    if (mapObjectType->type == MapObjectTypeClass::HARVESTABLE) {
        mapObject = new Harvestable(mapObjectType->maxAge);
    } else if (mapObjectType->type == MapObjectTypeClass::STRUCTURE) {
        mapObject = new Structure();
    } else if (mapObjectType->type == MapObjectTypeClass::STREET) {
        mapObject = new Street();
    } else if (mapObjectType->type == MapObjectTypeClass::BUILDING) {
        mapObject = new Building();
    } else if (mapObjectType->type == MapObjectTypeClass::CARRIER) {
        mapObject = new Carrier();
    } else if (mapObjectType->type == MapObjectTypeClass::SHIP) {
        mapObject = new Ship(mapObjectType->goodsSlots);
    } else {
        assert(false);
        return nullptr;
    }

    mapObject->mapObjectType = mapObjectType;
    return mapObject;
}

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