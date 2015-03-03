#include "economics/Carrier.h"

enum GoodsType : char;


Carrier::Carrier(Building* owningBuilding, Route* route, GoodsType goodsType, bool onOutboundTrip) :
    owningBuilding(owningBuilding), route(route), onOutboundTrip(onOutboundTrip) {

    carriedGoods.goodsType = goodsType;

    // Animation initialisieren
    animationFrame = 0;

    // Route initialisieren
    nextHopInRoute = route->cbegin();
    nextHopInRoute++;
}

Carrier::~Carrier() {
    delete route;
}

void Carrier::updateCurrentMovingDirection() {
    const MapCoords& nextHopOnRoute = *nextHopInRoute;
    const double diffX = nextHopOnRoute.x() - mapCoords.x();
    const double diffY = nextHopOnRoute.y() - mapCoords.y();

    if (diffX > 0 && diffY == 0) {
        currentMovingDirection = Direction::EAST;
    } else if (diffX > 0 && diffY > 0) {
        currentMovingDirection = Direction::SOUTHEAST;
    } else if (diffX == 0 && diffY > 0) {
        currentMovingDirection = Direction::SOUTH;
    } else if (diffX < 0 && diffY > 0) {
        currentMovingDirection = Direction::SOUTHWEST;
    } else if (diffX < 0 && diffY == 0) {
        currentMovingDirection = Direction::WEST;
    } else if (diffX < 0 && diffY < 0) {
        currentMovingDirection = Direction::NORTHWEST;
    } else if (diffX == 0 && diffY < 0) {
        currentMovingDirection = Direction::NORTH;
    } else if (diffX > 0 && diffY < 0) {
        currentMovingDirection = Direction::NORTHEAST;
    }
}