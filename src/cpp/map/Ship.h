#ifndef _SHIP_H
#define _SHIP_H

#include <string>
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
     * @brief Name des Schiffs
     */
    std::string name;

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

    const std::string& getName() const {
        return name;
    }

    void setName(const std::string& name) {
        this->name = name;
    }

    virtual bool updateObject(const Context& context);
};

#endif
