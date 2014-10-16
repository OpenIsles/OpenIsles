#include "game/Colony.h"

Colony::Colony() : goods(GoodsType::MAX_GOOD) {
    for (int i = 0; i < GoodsType::MAX_GOOD; i++) {
        goods[i].goodsType = (GoodsType) i;
        goods[i].inventory = 0;
        goods[i].capacity = 30;
    }
}

Colony::~Colony() {
    
}