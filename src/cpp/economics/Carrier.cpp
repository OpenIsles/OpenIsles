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

    EightDirectionsView movingDirection;
    if (diffX > 0 && diffY == 0) {
        movingDirection = "east";
    } else if (diffX > 0 && diffY > 0) {
        movingDirection = "southeast";
    } else if (diffX == 0 && diffY > 0) {
        movingDirection = "south";
    } else if (diffX < 0 && diffY > 0) {
        movingDirection = "southwest";
    } else if (diffX < 0 && diffY == 0) {
        movingDirection = "west";
    } else if (diffX < 0 && diffY < 0) {
        movingDirection = "northwest";
    } else if (diffX == 0 && diffY < 0) {
        movingDirection = "north";
    } else if (diffX > 0 && diffY < 0) {
        movingDirection = "northeast";
    }

    currentMovingDirection = movingDirection;
}