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
    long coins;

    /**
     * @brief Werkzeug
     */
    int tools;

    /**
     * @brief Holz
     */
    int wood;

    /**
     * @brief Ziegel
     */
    int bricks;

public:
    BuildingCosts() : coins(0), tools(0), wood(0), bricks(0) {
    }

    BuildingCosts(long coins, int tools, int wood, int bricks)
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

    /**
     * @brief Subtraktions-Zuweisungsoperator, der Baukosten abzieht.
     * @param subtrahend Subtrahend
     */
    void operator-=(const BuildingCosts& subtrahend) {
        coins -= subtrahend.coins;
        tools -= subtrahend.tools;
        wood -= subtrahend.wood;
        bricks -= subtrahend.bricks;
    }

    /**
     * @brief Ermittelt, ob alle Felder nicht-negativ sind.
     * @return `true` wenn alle Felder nicht-negativ sind, sonst `false`.
     */
    bool isNonNegative() const {
        return ((coins >= 0) && (tools >= 0) && (wood >= 0) && (bricks >= 0));
    }

} BuildingCosts;

#endif