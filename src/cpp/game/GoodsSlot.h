#ifndef _GOODS_SLOT_H
#define _GOODS_SLOT_H


/**
* Enum für die verschiedenen Gütertypen
*/
typedef
enum GoodsType : char {
    // Spezialwert, der dem null-Wert entspricht
    NO_GOODS = -1,

    // Güter
    TOOLS = 0,
    WOOD = 1,
    BRICKS = 2,

    // Spezialwert am Ende, damit wir wissen, wie groß wir Arrays etc. machen müssen
    MAX_GOOD

} GoodsType;


/**
* @brief stellt einen Slot für Waren dar. Diese Klasse hält die Infos, ob mit dem Slot was assoziiert is,
* wenn ja, welches Gut und hält zusätzlich eine aktuelle und eine maximale Warenmenge.
*/
class GoodsSlot {

public:
    /**
     * @brief Gut bzw. NO_GOODS, wenn der Container nicht belegt ist
     */
    GoodsType goodsType;

    /**
     * @brief Lagerbestand
     */
    unsigned int inventory;

    /**
     * @brief Lagerkapazität
     */
    unsigned int capacity;

public:
    /**
     * @brief Konstruktur, um einen unbenutzten Slot anzulegen
     */
    GoodsSlot() {
        goodsType = GoodsType::NO_GOODS;
        inventory = 0;
        capacity = 0;
    }

    /**
     * @brief Konstruktur, um einen Slot für ein bestimmtes Gut anzulegen.
     * Lagerbestand und -kapazität werden auf 0 gesetzt.
     * @param goodsType Gut
     */
    GoodsSlot(GoodsType goodsType) : goodsType(goodsType) {
        inventory = 0;
        capacity = 0;
    }

    /**
     * @brief Konstruktur, um einen Slot für ein bestimmtes Gut und einer bestimmten Lagerkapazität anzulegen.
     * Lagerbestand wird auf 0 gesetzt.
     * @param goodsType Gut
     * @param capacity Lagerkapazität
     */
    GoodsSlot(GoodsType goodsType, unsigned int capacity) : goodsType(goodsType), capacity(capacity) {
        inventory = 0;
    }


    /**
     * @brief Prüft, ob der Slot belegt ist
     * @return true, wenn der Slot belegt ist, sonst false.
     */
    inline bool isUsed() const {
        return (goodsType != GoodsType::NO_GOODS);
    }

};

#endif
