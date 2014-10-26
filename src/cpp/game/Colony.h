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
     * @brief Liefert den Slot für einen bestimmten Warentyp zurück. Dieser enthält Lagerbestand und -Kapazität.
     * @return Slot für den angeforderten Warentyp
     */
    GoodsSlot& getGoods(GoodsType goodsType) {
        return goods[goodsType];
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
