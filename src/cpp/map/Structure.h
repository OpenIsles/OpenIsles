#ifndef _STRUCTURE_H
#define _STRUCTURE_H

#include "map/coords/MapCoords.h"
#include "map/MapObject.h"
#include "map/MapObjectType.h"

/**
 * Enum für die Gebäude-Gruppen, d.h. wie sie im Baumenü angeordnet werden
 */
typedef
enum BuildingGroup : unsigned char {

    // Handwerksbetriebe
    CRAFTSMAN = 0,

    // Farmen & Plantagen
    FARM = 1,

    // Hafenanlagen
    PORT = 2,

    // Öffentliche Gebäude
    PUBLIC = 3

} BuildingGroup;


/**
 * @brief MapObject, das eine Struktur darstellt. Strukturen sind Wege und Felder, also Sachen nicht klickbar sind.
 */
class Structure : public MapObjectFixed {

public:
    virtual bool updateObject(const Context& context) override {
        return true;
    }

    /**
     * @brief Prüft, ob die Struktur eine Straße ist
     * @return `true` wenn Straße, sonst `false`
     */
    bool isStreet() const;

    /**
     * @brief Prüft, ob die Struktur ein Platz ist, d.&nbsp;h. etwas, wo ein Marktkarren fahren darf.
     * @return `true` wenn Straße oder Platz, sonst `false`
     */
    bool isPlace() const {
        // TODO Flag sollte direkt an der Config gesteuert werden

        return (mapObjectType->name.substr(0, 6) == "square");
    }

    /**
     * @brief Ermittelt, ob ein Marktkarren eine bestimmte Stelle befahren darf.
     * @return `true`, wenn ein Marktkarren dort fahren darf, d.&nbsp;h. eine Straße oder Platz da ist;
     * sonst `false`.
     */
    bool isWalkableForCart() const {
        return (isStreet() || isPlace());
    }
};

#endif
