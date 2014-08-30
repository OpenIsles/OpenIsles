#ifndef _COLONY_H
#define _COLONY_H

#include <vector>


/**
 * Enum für die verschiedenen Gütertypen
 */
typedef
enum GoodsType : unsigned char {
    TOOLS = 0,
    WOOD = 1,
    BRICKS = 2,
            
    // Spezialwert am Ende, damit wir wissen, wie groß wir Arrays etc. machen müssen
    MAX_GOOD
            
} GoodsType;


/**
 * @brief Klasse, die eine Siedlung eines Spielers auf einer Insel darstellt
 */
class Colony {
    
private:
    /**
     * Lagerbestand pro Gütertyp. Vektor-Indexes siehe GoodsType.
     */
    std::vector<int> goodsInventory;
    
    /**
     * Lagerkapazität pro Gütertyp. Vektor-Indexes siehe GoodsType.
     */
    std::vector<int> goodsCapacity;
    
public:
	Colony();
	~Colony();
    
    /**
     * Ändert den Lagerbestand eines Guts
     * 
     * @param goodsType Gütertyp
     * @param inventory neuer Lagerbestand
     */
    void setGoodsInventory(GoodsType goodsType, int inventory) {
        goodsInventory[goodsType] = inventory;
    }
    
    /**
     * Liefert den Lagerbestand eines Guts
     * 
     * @param goodsType Gütertyp
     * @return Lagerbestand
     */
    int getGoodsInventory(GoodsType goodsType) const {
        return goodsInventory[goodsType];
    }
    
    /**
     * Ändert die Lagerkapazität eines Guts
     * 
     * @param goodsType Gütertyp
     * @param capacity neue Lagerkapazität
     */
    void setGoodsCapacity(GoodsType goodsType, int capacity) {
        goodsCapacity[goodsType] = capacity;
    }
    
    /**
     * Liefert die Lagerkapazität eines Guts
     * 
     * @param goodsType Gütertyp
     * @return Lagerkapazität
     */
    int getGoodsCapacity(GoodsType goodsType) const {
        return goodsInventory[goodsType];
    }
    
};

#endif
