#include "game/Colony.h"

Colony::Colony() : goodsInventory(GoodsType::MAX_GOOD, 0), goodsCapacity(GoodsType::MAX_GOOD, 30) {
}

Colony::~Colony() {
    
}