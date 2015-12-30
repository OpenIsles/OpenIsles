#ifndef _PLAYER_STATUS_H
#define _PLAYER_STATUS_H

/**
 * @brief Hilfsstruktur, die die Informationen für das "Spielerstatus"-Panel zwischenspeichert
 */
struct PlayerStatus {

    unsigned long population;     ///< Einwohner
    unsigned long taxesIncome;    ///< Steuererträge (je 6 [Zyklen](@ref gameTicks))
    unsigned long operatingCosts; ///< Betriebskosten (je 6 [Zyklen](@ref gameTicks))
    long balance;                 ///< Bilanz (je 6 [Zyklen](@ref gameTicks))

    PlayerStatus() {
        population = 0;
        taxesIncome = 0;
        operatingCosts = 0;
        balance = 0;
    }
};

#endif
