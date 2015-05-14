#include "game/Game.h"
#include "map/Harvestable.h"

/**
 * @brief Wachsumsgeschwindigkeit: 1 pro 40 Sekunden
 */
static const double growthPerSecond = 1.0 / 40.0; // TODO über Config steuern


bool Harvestable::updateObject(const Context& context) {
    // Wachstum durchführen
    if (age >= maxAge) {
        return true; // bereits maximales Alter erreicht. Nix tun.
    }

    double growth = context.game->getSecondsSinceLastUpdate(this) * growthPerSecond;

    double newAge = age + growth;

    // Maximales Alter
    if (newAge > maxAge) {
        newAge = maxAge;
    }

    age = newAge;
    return true;
}