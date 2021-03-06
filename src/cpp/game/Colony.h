#ifndef _COLONY_H
#define _COLONY_H

#include <set>
#include <string>
#include <unordered_map>
#include <vector>
#include "config/BuildingCosts.h"
#include "game/ColonyPopulationTier.h"
#include "game/GoodsSlot.h"
#include "game/PopulationTier.h"


/**
 * @brief Klasse, die eine Siedlung eines Spielers auf einer Insel darstellt
 */
class Colony {
    
private:
    /**
     * @brief Lagerbestände pro Gütertyp.
     */
    std::unordered_map<const Good*, GoodsSlot> goods;

public:
    /**
     * @brief Name der Siedlung
     */
    std::string name;

    /**
     * @brief Einwohner der Siedlung
     */
    unsigned int population;

    /**
     * @brief Infos pro Bevölkerungsgruppe
     */
    std::unordered_map<const PopulationTier*, ColonyPopulationTier> populationTiers;

    /**
     * @brief [Zeitpunkt](@ref gameTicks), wann in dieser Siedlung zuletzt ein Haus aufgestiegen ist
     */
    unsigned long lastAdvancementTicks;
    
public:
    /**
     * @brief Legt eine neue Kolonie mit 30t Lagerkapazität in allen Slots an.
     * @param allGoods Map mit allen verfügbaren Gütern
     * @param allPopulationTiers Set mit allen verfügbaren Bevölkerungsgruppen
     */
    Colony(const std::unordered_map<std::string, Good>& allGoods,
           const std::set<PopulationTier>& allPopulationTiers);

    ~Colony();

    /**
     * @brief Liefert den Slot für einen bestimmten Warentyp zurück. Dieser enthält Lagerbestand und -Kapazität.
     * @param good Gut
     * @return Slot für den angeforderten Warentyp
     */
    GoodsSlot& getGoods(const Good* good) {
        return goods[good];
    }

    /**
     * @brief Liefert den Slot für einen bestimmten Warentyp zurück. Dieser enthält Lagerbestand und -Kapazität.
     * (const-Variante)
     *
     * @param good Gut
     * @return Slot für den angeforderten Warentyp
     */
    const GoodsSlot& getGoods(const Good* good) const {
        return goods.at(good);
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
