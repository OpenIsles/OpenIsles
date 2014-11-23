#ifndef _COLONY_H
#define _COLONY_H

#include <vector>
#include "config/ConfigMgr.h"
#include "game/GoodsSlot.h"


/**
 * @brief Klasse, die eine Siedlung eines Spielers auf einer Insel darstellt
 */
class Colony {
    
private:
    /**
     * @brief Lagerbestände pro Gütertyp. Vektor-Indexes siehe GoodsType.
     */
    std::vector<GoodsSlot> goods;

public:
    /**
     * @brief Einwohner der Siedlung
     */
    unsigned int population;
    
public:
    /**
     * Legt eine neue Kolonie mit 30t Lagerkapazität in allen Slots an.
     */
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
     * @brief Erniedrigt die Lagerbestände der Güter der Siedlung für den Bau eines Gebäudes
     * @param buildingCosts Gebäudekosten, wo denen die Güterkosten dekrementiert werden.
     */
    void subtractBuildingCosts(const BuildingCosts* buildingCosts) {
        goods[GoodsType::TOOLS].inventory -= buildingCosts->tools;
        goods[GoodsType::WOOD].inventory -= buildingCosts->wood;
        goods[GoodsType::BRICKS].inventory -= buildingCosts->bricks;
    }

    /**
     * @brief Erhöht die Lagerkapazität der Kolonie um einen bestimmten Wert. Es werden damit die Lagerkapazitäten
     * in allen Slots erhöht.
     * @param amount Anzahl, um wie viel erhöht werden soll
     */
    void increaseGoodsCapacity(unsigned int amount);

    /**
     * @brief Erniedrigt die Lagerkapazität der Kolonie um einen bestimmten Wert. Es werden damit die Lagerkapazitäten
     * in allen Slots erniedrigt. Überstehende Waren gehen verloren.
     * @param amount Anzahl, um wie viel erniedrigt werden soll
     */
    void decreaseGoodsCapacity(unsigned int amount);
};

#endif
