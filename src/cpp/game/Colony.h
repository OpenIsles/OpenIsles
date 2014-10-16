#ifndef _COLONY_H
#define _COLONY_H

#include <vector>
#include "config/BuildingConfigMgr.h"
#include "game/GoodsSlot.h"


/**
 * @brief Klasse, die eine Siedlung eines Spielers auf einer Insel darstellt
 */
class Colony {
    
private:
    /**
     * Lagerbestände pro Gütertyp. Vektor-Indexes siehe GoodsType.
     */
    std::vector<GoodsSlot> goods;
    
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
        goods[goodsType].inventory = inventory;
    }
    
    /**
     * Liefert den Lagerbestand eines Guts
     * 
     * @param goodsType Gütertyp
     * @return Lagerbestand
     */
    unsigned int getGoodsInventory(GoodsType goodsType) const {
        return goods[goodsType].inventory;
    }
    
    /**
     * Ändert die Lagerkapazität eines Guts
     * 
     * @param goodsType Gütertyp
     * @param capacity neue Lagerkapazität
     */
    void setGoodsCapacity(GoodsType goodsType, unsigned int capacity) {
        goods[goodsType].capacity = capacity;
    }
    
    /**
     * Liefert die Lagerkapazität eines Guts
     * 
     * @param goodsType Gütertyp
     * @return Lagerkapazität
     */
    unsigned int getGoodsCapacity(GoodsType goodsType) const {
        return goods[goodsType].capacity;
    }
    
    /**
     * @brief Erniedrigt die Lagerbestände der Güter der Kolonie für den Bau eines Gebäudes
     * @param buildingCosts Gebäudekosten, wo denen die Güterkosten dekrementiert werden.
     */
    void subtractBuildingCosts(const BuildingCosts* buildingCosts) {
        goods[GoodsType::TOOLS].inventory -= buildingCosts->tools;
        goods[GoodsType::WOOD].inventory -= buildingCosts->wood;
        goods[GoodsType::BRICKS].inventory -= buildingCosts->bricks;
    }
    
};

#endif
