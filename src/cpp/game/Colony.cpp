#include <algorithm>
#include "game/Colony.h"

typedef std::unordered_map<const Good*, GoodsSlot>::value_type GoodsSlotMapValueType;

Colony::Colony(const std::unordered_map<std::string, Good>& allGoods,
               const std::set<PopulationTier>& allPopulationTiers) : goods() {

    for (auto iter = allGoods.cbegin(); iter != allGoods.cend(); iter++) {
        const Good* good = &iter->second;
        goods[good].good = good;
        goods[good].inventory = 0;
        goods[good].capacity = 0;
    }

    population = 0;

    for (auto iter = allPopulationTiers.cbegin(); iter != allPopulationTiers.cend(); iter++) {
        const PopulationTier* populationTier = &*iter;
        populationTiers[populationTier].population = 0;
        populationTiers[populationTier].populationSatisfaction = PopulationSatisfaction::GOOD;
        populationTiers[populationTier].taxesPercent = 32;
    }
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
