#include "game/Game.h"
#include "map/Harvestable.h"

/**
 * @brief Wachsumsgeschwindigkeit: 1 pro 40 Sekunden [Spielzeit](@ref gameTicks)
 */
static const double growthPerMinute =  1.0 / 40.0 * 60.0; // TODO über Config steuern


bool Harvestable::updateObject(const Context& context) {
    // Wachstum durchführen
    if (age >= maxAge) {
        return true; // bereits maximales Alter erreicht. Nix tun.
    }

    double growth = (double) getTicksSinceLastUpdate(context) / TICKS_PER_MINUTE * growthPerMinute;

    double newAge = age + growth;

    // Maximales Alter
    if (newAge > maxAge) {
        newAge = maxAge;
    }

    age = newAge;
    return true;
}