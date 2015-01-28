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
