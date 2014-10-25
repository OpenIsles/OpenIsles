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
    WOOL = 0,
    CLOTH,
    TOOLS,
    WOOD,
    BRICKS,

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
     * @brief Lagerbestand als exakter Wert. Der Benutzer wird immer nur die ganzen Waren zu Gesicht bekommen. Intern
     * müssen wir aber auch Bruchteile von Waren verwalten.
     */
    double inventory;

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

    /**
     * @brief Erhöht den Lagerbestand um einen bestimmten Wert. Würde dabei die Lagerkapazität gesprengt werden,
     * wird soviel wie möglich als Lagerbestand gesetzt.
     * @param amount Anzahl, um wie viel erhöht werden soll
     */
    void increaseInventory(double amount) {
        inventory += amount;
        if (inventory > capacity) {
            inventory = capacity;
        }
    }

    /**
     * @brief Erniedrigt den Lagerbestand um einen bestimmten Wert. Würde dabei negativer Lagerbestand rauskommen,
     * wird der Lagerbestand auf 0 gesetzt.
     * @param amount Anzahl, um wie viel erniedrigt werden soll
     */
    void decreaseInventory(double amount) {
        inventory -= amount;
        if (inventory < 0) {
            inventory = 0;
        }
    }

    /**
     * @brief Überprüft, ob die Lager voll sind
     * @return true, wenn das Lager voll ist, sonst false
     */
    inline bool isInventoryFull() const {
        return (inventory >= capacity);
    }
};

#endif
