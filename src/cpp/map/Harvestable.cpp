#include "game/Game.h"
#include "map/Harvestable.h"

/**
 * @brief Wachsumsgeschwindigkeit: 1 pro 40 Sekunden
 */
static const double growthPerSecond = 1.0 / 40.0; // TODO über Config steuern


bool Harvestable::updateObject(const Context& context) {
    // Wachstum durchführen
    double growth = context.game->getSecondsSinceLastUpdate(this) * growthPerSecond;

    double newAge = age + growth;

    // Maximales Alter
    if (newAge > 4.0) {
        newAge = 4; // TODO über Config steuern bzw./und mit der Grafik koppeln (Tabakfeld z.B. hat maxAge 6)
    }

    age = newAge;
    return true;
}