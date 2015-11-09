#ifndef _POPULATION_TIER_H
#define _POPULATION_TIER_H

#include <string>
#include "config/BuildingCosts.h"

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
     * @brief Name der Bevölkerungsgruppe (für den Spieler)
     */
    std::string title;

    /**
     * @brief Baukosten für den Aufstieg in diese Bevölkerungsgruppe
     */
    BuildingCosts advancementCosts;

    /**
     * @brief maximale Einwohnerzahl pro Haus
     */
    unsigned char maxPopulationPerHouse;

    // TODO Steuern
    // TODO Bedürfnisse (Waren)
    // TODO Bedürfnisse (öffentliche Gebäude)


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
