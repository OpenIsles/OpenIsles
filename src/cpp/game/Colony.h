#ifndef _COLONY_H
#define _COLONY_H

#include <vector>
#include "config/BuildingConfigMgr.h"


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
 * @brief Klasse, die eine Siedlung eines Spielers auf einer Insel darstellt
 */
class Colony {
    
private:
    /**
     * Lagerbestand pro Gütertyp. Vektor-Indexes siehe GoodsType.
     */
    std::vector<unsigned int> goodsInventory;
    
    /**
     * Lagerkapazität pro Gütertyp. Vektor-Indexes siehe GoodsType.
     */
    std::vector<unsigned int> goodsCapacity;
    
public:
	Colony();
	~Colony();
    
    /**
     * Ändert den Lagerbestand eines Guts
     * 
     * @param goodsType Gütertyp
     * @param inventory neuer Lagerbestand
     */
    void setGoodsInventory(GoodsType goodsType, unsigned int inventory) {
        goodsInventory[goodsType] = inventory;
    }
    
    /**
     * Liefert den Lagerbestand eines Guts
     * 
     * @param goodsType Gütertyp
     * @return Lagerbestand
     */
    unsigned int getGoodsInventory(GoodsType goodsType) const {
        return goodsInventory[goodsType];
    }
    
    /**
     * Ändert die Lagerkapazität eines Guts
     * 
     * @param goodsType Gütertyp
     * @param capacity neue Lagerkapazität
     */
    void setGoodsCapacity(GoodsType goodsType, unsigned int capacity) {
        goodsCapacity[goodsType] = capacity;
    }
    
    /**
     * Liefert die Lagerkapazität eines Guts
     * 
     * @param goodsType Gütertyp
     * @return Lagerkapazität
     */
    unsigned int getGoodsCapacity(GoodsType goodsType) const {
        return goodsInventory[goodsType];
    }
    
    /**
     * @brief Erniedrigt die Lagerbestände der Güter der Kolonie für den Bau eines Gebäudes
     * @param buildingCosts Gebäudekosten, wo denen die Güterkosten dekrementiert werden.
     */
    void subtractBuildingCosts(const BuildingCosts* buildingCosts) {
        goodsInventory[GoodsType::TOOLS] -= buildingCosts->tools;
        goodsInventory[GoodsType::WOOD] -= buildingCosts->wood;
        goodsInventory[GoodsType::BRICKS] -= buildingCosts->bricks; 
    }
    
};

#endif
