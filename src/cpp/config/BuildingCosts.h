#ifndef _BUILDING_COSTS_H
#define _BUILDING_COSTS_H

/**
 * @brief Baukosten eines Gebäudes
 */
typedef
struct BuildingCosts {

    /**
     * @brief Münzen
     */
    unsigned int coins;

    /**
     * @brief Werkzeuge
     */
    unsigned int tools;

    /**
     * @brief Holz
     */
    unsigned int wood;

    /**
     * @brief Ziegel
     */
    unsigned int bricks;

public:
    BuildingCosts() : coins(0), tools(0), wood(0), bricks(0) {
    }

    BuildingCosts(unsigned int coins, unsigned int tools, unsigned int wood, unsigned int bricks)
        : coins(coins), tools(tools), wood(wood), bricks(bricks) {
    }

    /**
     * @brief Skalar-Multiplikations-Operator, der die Baukosten für mehrere Gebäude berechnet.
     * @param n Multiplikator
     * @return Baukosten für `n` Gebäude
     */
    BuildingCosts operator*(int n) const {
        return BuildingCosts(coins * n, tools * n, wood * n, bricks * n);
    }

} BuildingCosts;

#endif