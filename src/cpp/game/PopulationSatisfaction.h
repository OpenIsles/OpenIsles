#ifndef _POPULATION_SATISFACTION_H
#define _POPULATION_SATISFACTION_H

/**
 * @brief Zufriedenheitsstatus der Einwohner
 */
enum PopulationSatisfaction : unsigned char {
    GOOD = 0,    ///< Zuwächse in dieser Bevölkerungsgruppe
    NEUTRAL = 1, ///< Keine Zuwächse in dieser Bevölkerungsgruppe
    BAD = 2,     ///< Leichte Rückgänge in dieser Bevölkerungsgruppe
    WORST = 3    ///< Starke Rückgänge in dieser Bevölkerungsgruppe
};

#endif
