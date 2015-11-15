#ifndef _PLAYER_STATUS_H
#define _PLAYER_STATUS_H

/**
 * @brief Hilfsstruktur, die die Informationen für das "Spielerstatus"-Panel zwischenspeichert
 */
struct PlayerStatus {

    unsigned long population;  ///< Einwohner
    unsigned long taxesIncome; ///< Steuererträge (je 6 Zyklen * 10 Sekunden)

    PlayerStatus() {
        population = 0;
        taxesIncome = 0;
    }
};

#endif
