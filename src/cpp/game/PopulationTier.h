#ifndef _POPULATION_TIER_H
#define _POPULATION_TIER_H

#include <list>
#include <string>
#include "config/BuildingCosts.h"
#include "config/Good.h"
#include "config/NeededGood.h"

class MapObjectType;


/**
 * @brief Bevölkerungsgruppe
 *
 * Mit Ausnahme des ConfigMgr, der diese Datenstruktur verwaltet, sollte diese Struktur nur als `const PopulationTier*`
 * bzw. über das `const std::set<PopulationTier>&` benutzt werden.
 */
struct PopulationTier {

    /**
     * @brief interner Index (1-based), der für die Sortierung im Set benutzt wird.
     * Die nächsthöhere Bevölkerungsgruppe hat einen Index um 1 höher.
     */
    unsigned char index;

    /**
     * @brief interne Bezeichnung (z.&nbsp;B. in einem gespeicherten Spielstand)
     */
    std::string name;

    /**
     * @brief Name der Bevölkerungsgruppe (für den Spieler) (msgid)
     */
    std::string title;

    /**
     * @brief Anzahl der Güter, die komplett fehlen dürfen, aber trotzdem einen Aufstieg erlauben
     */
    unsigned char advancementMissingGoodsOk = 0;

    /**
     * @brief Baukosten für den Aufstieg in diese Bevölkerungsgruppe
     */
    BuildingCosts advancementCosts;

    /**
     * @brief verlangte Waren
     * (Die Reihenfolge ist wie in Config-Datei angegeben und wird auch so im Spiel dargestellt)
     */
    std::list<NeededGood> needsGoods;

    /**
     * @brief verlangte öffentliche Gebäude
     * (Die Reihenfolge ist wie in Config-Datei angegeben und wird auch so im Spiel dargestellt)
     */
    std::list<const MapObjectType*> needsPublicBuildings;

    /**
     * @brief maximale Einwohnerzahl pro Haus
     */
    unsigned char maxPopulationPerHouse;

    /**
     * @brief Steuersatz (Münzen pro Einwohner - bei "34% Steuer")
     *
     * Siehe @ref finances
     */
    double taxesPerInhabitant;


    /**
     * @brief Kleiner-Operator (für die totale Ordnung im Set), der nach `index` vergleicht.
     * @param populationTier Operand
     * @return `true`, wenn `this` "kleiner ist" als `populationTier`
     */
    bool operator<(const PopulationTier& populationTier) const {
        return (this->index < populationTier.index);
    }

};

#endif
