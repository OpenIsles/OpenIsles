#include "economics/Carrier.h"

enum GoodsType : char;


Carrier::Carrier(Building* owningBuilding, Route* route, GoodsType goodsType) :
    owningBuilding(owningBuilding), route(route) {

    carriedGoods.goodsType = goodsType;

    // Animation initialisieren
    xOffsetInTile = 0;
    yOffsetInTile = 0;
    animationFrame = 0;
}

Carrier::~Carrier() {
    delete route;
}
