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
        movingDirection = EightDirectionsView(EightDirectionsView::EAST);
    } else if (diffX > 0 && diffY > 0) {
        movingDirection = EightDirectionsView(EightDirectionsView::SOUTHEAST);
    } else if (diffX == 0 && diffY > 0) {
        movingDirection = EightDirectionsView(EightDirectionsView::SOUTH);
    } else if (diffX < 0 && diffY > 0) {
        movingDirection = EightDirectionsView(EightDirectionsView::SOUTHWEST);
    } else if (diffX < 0 && diffY == 0) {
        movingDirection = EightDirectionsView(EightDirectionsView::WEST);
    } else if (diffX < 0 && diffY < 0) {
        movingDirection = EightDirectionsView(EightDirectionsView::NORTHWEST);
    } else if (diffX == 0 && diffY < 0) {
        movingDirection = EightDirectionsView(EightDirectionsView::NORTH);
    } else if (diffX > 0 && diffY < 0) {
        movingDirection = EightDirectionsView(EightDirectionsView::NORTHEAST);
    }

    currentMovingDirection = movingDirection;
}