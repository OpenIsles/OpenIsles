#include "game/Colony.h"

Colony::Colony() : goods(GoodsType::MAX_GOOD) {
    for (int i = 0; i < GoodsType::MAX_GOOD; i++) {
        goods[i].goodsType = (GoodsType) i;
        goods[i].inventory = 0;
        goods[i].capacity = 30;
    }

    population = 0;
}

Colony::~Colony() {
    
}

void Colony::increaseGoodsCapacity(unsigned int amount) {
    for (int i = 0; i < GoodsType::MAX_GOOD; i++) {
        goods[i].increaseCapacity(amount);
    }
}

void Colony::decreaseGoodsCapacity(unsigned int amount) {
    for (int i = 0; i < GoodsType::MAX_GOOD; i++) {
        goods[i].decreaseCapacity(amount);
    }
}
