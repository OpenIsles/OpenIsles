#ifndef _SHIP_H
#define _SHIP_H

#include <vector>
#include "global.h"
#include "game/GoodsSlot.h"
#include "map/MapObject.h"

/**
 * @brief Schiff
 */
class Ship : public MapObjectMoving {

public:
    /**
     * @brief Array aus transportierte Waren
     */
    std::vector<GoodsSlot> goodsOnBoard;

public:
    /**
     * @brief (Konstruktor)
     * @param goodsSlots Anzahl Waren-Slots, die das Schiff erhalten soll
     */
    Ship(unsigned char goodsSlots);

    virtual ~Ship() {
    }

    virtual bool updateObject(const Context& context);
};

#endif
