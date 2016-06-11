#include "map/Ship.h"


Ship::Ship(unsigned char goodsSlots) {
    // Warenslots anlegen
    for (unsigned char i = 0; i < goodsSlots; i++) {
        goodsOnBoard.push_back(GoodsSlot());
    }
}

bool Ship::updateObject(const Context& context) {
    return true;
}