#ifndef _UPDATEABLE_OBJECT_H
#define _UPDATEABLE_OBJECT_H

#include "utils/Ticks.h"

/**
 * @brief Basisklasse für Map-Objekte, die mittels einer Update-Funktion aktualisiert werden können.
 */
class UpdateableObject {

protected:
    /**
     * @brief SDL_GetTicks-Wert, wann zuletzt dieses Objekt aktualisiert wurde
     */
    uint32_t lastUpdateTime;

public:
    UpdateableObject() {
        // Bei Instanziierung sofort den Wert setzen, damit immer was Sinnvolles drinsteht.
        lastUpdateTime = getTicks();
    }

    uint32_t getLastUpdateTime() const {
        return lastUpdateTime;
    }

    void setLastUpdateTime(uint32_t lastUpdateTime) {
        this->lastUpdateTime = lastUpdateTime;
    }
};

#endif
