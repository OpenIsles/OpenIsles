#ifndef _NEEDED_GOOD_H
#define _NEEDED_GOOD_H

/**
 * @brief Hilfsstruktur für ein von der Bevölkerung benötigtes Gut
 */
struct NeededGood {
    /**
     * verlangtes Gut
     */
    const Good* good;

    /**
     * Verbrauch dieses Guts pro Zyklus pro 100 Einwohner
     */
    double consumePerCycle;
};

#endif
