#include <algorithm>
#include "game/Colony.h"

typedef std::unordered_map<const Good*, GoodsSlot>::value_type GoodsSlotMapValueType;

Colony::Colony(const std::unordered_map<std::string, Good>& allGoods) : goods() {
    for (auto iter = allGoods.cbegin(); iter != allGoods.cend(); iter++) {
        const Good* good = &iter->second;
        goods[good].good = good;
        goods[good].inventory = 0;
        goods[good].capacity = 30;
    }

    population = 0;
}

Colony::~Colony() {
}

void Colony::increaseGoodsCapacity(unsigned int amount) {
    std::for_each(goods.begin(), goods.end(), [&](GoodsSlotMapValueType& iter) {
        iter.second.increaseCapacity(amount);
    });
}

void Colony::decreaseGoodsCapacity(unsigned int amount) {
    std::for_each(goods.begin(), goods.end(), [&](GoodsSlotMapValueType& iter) {
        iter.second.decreaseCapacity(amount);
    });
}
