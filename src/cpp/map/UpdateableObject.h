#ifndef _UPDATEABLE_OBJECT_H
#define _UPDATEABLE_OBJECT_H

#include <gtest/gtest.h>
#include "utils/Ticks.h"

/**
 * @brief Basisklasse für Map-Objekte, die mittels einer Update-Funktion aktualisiert werden können.
 */
class UpdateableObject {

    FRIEND_TEST(EconomicsMgrTest, updateCarrier); // Test soll die Update-Zeit via setLastUpdateTime() manipulieren dürfen.

protected:
    /**
     * @brief SDL_GetTicks-Wert, wann zuletzt dieses Objekt aktualisiert wurde
     */
    uint32_t lastUpdateTime;

private:
    void setLastUpdateTime(uint32_t lastUpdateTime) {
        this->lastUpdateTime = lastUpdateTime;
    }

public:
    UpdateableObject() {
        // Bei Instanziierung sofort den Wert setzen, damit immer was Sinnvolles drinsteht.
        lastUpdateTime = getTicks();
    }

    uint32_t getLastUpdateTime() const {
        return lastUpdateTime;
    }

    /**
     * @brief Aktualisiert das Objekt.
     * @param context (Dependency)
     */
    void update(const Context& context) {
        updateObject(context);
        setLastUpdateTime(context.sdlTicks);
    }

    /**
     * @brief Interne Update-Funktion, die von den Unterklassen implementiert werden muss und die klassen-spezifische
     * Update-Logik für das Objekt ausführt.
     *
     * @param context (Dependency)
     */
    virtual void updateObject(const Context& context) = 0;
};

#endif
