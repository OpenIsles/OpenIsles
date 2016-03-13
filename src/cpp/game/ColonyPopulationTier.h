#ifndef _COLONY_POPULATION_TIER_H
#define _COLONY_POPULATION_TIER_H

#include "game/PopulationSatisfaction.h"

/**
 * @brief Struktur, die Infos zu einer Bevölkerungsgruppe in einer Siedlung zusammenhält
 */
struct ColonyPopulationTier {

    /**
     * @brief Einwohner
     */
    unsigned int population;

    /**
     * @brief [Zeitpunkt](@ref gameTicks), wann zuletzt Einwohner hinzugekommen sind
     */
    unsigned long lastIncreaseTicks;

    /**
     * @brief Zufriedenheit
     */
    PopulationSatisfaction populationSatisfaction;

    /**
     * @brief Steuerprozente
     *
     * Info: In Anno&nbsp;1602 ist das eine Fließkommazahl, wenn auch nicht sichtbar. Der Schieberegler kann
     * pixelgenau eingestellt werden. Steuereinnahmen ändern sich, aber die Prozentzahl nicht, weil der Nachkomma-Teil
     * nicht angezeigt wird.
     */
    unsigned char taxesPercent;

};

#endif
