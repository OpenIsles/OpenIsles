#include <algorithm>
#include "game/Colony.h"

typedef std::unordered_map<std::string, GoodsSlot>::value_type GoodsSlotMapValueType;

Colony::Colony(const std::unordered_map<std::string, Good>& allGoods) : goods() {
    for (auto iter = allGoods.cbegin(); iter != allGoods.cend(); iter++) {
        const std::string& goodName = iter->first;
        goods[goodName].good = &iter->second;
        goods[goodName].inventory = 0;
        goods[goodName].capacity = 30;
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
