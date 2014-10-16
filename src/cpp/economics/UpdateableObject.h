#ifndef _UPDATEABLE_OBJECT_H
#define _UPDATEABLE_OBJECT_H

/**
 * @brief Basisklasse für Map-Objekte, die mittels einer Update-Funktion aktualisiert werden können.
 */
class UpdateableObject {

protected:
    /**
     * @brief SDL_GetTicks-Wert, wann zuletzt dieses Objekt aktualisiert wurde
     */
    Uint32 lastUpdateTime;

public:
    UpdateableObject() {
        // Bei Instanziierung sofort den Wert setzen, damit immer was Sinnvolles drinsteht.
        lastUpdateTime = SDL_GetTicks();
    }

    Uint32 getLastUpdateTime() const {
        return lastUpdateTime;
    }

    void setLastUpdateTime(Uint32 lastUpdateTime) {
        this->lastUpdateTime = lastUpdateTime;
    }
};

#endif
