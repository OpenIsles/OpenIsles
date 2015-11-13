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
     * @brief Zufriedenheit
     */
    PopulationSatisfaction populationSatisfaction;

};

#endif
