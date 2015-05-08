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
     * @brief Prüft, ob die Struktur ein Feldweg oder Pflasterstraße ist
     * @return `true` wenn Feldweg oder Pflasterstraße, sonst `false`
     */
    bool isStreet() const {
        return ((mapObjectType >= MapObjectType::FARM_ROAD_STRAIGHT_0 &&
                 mapObjectType <= MapObjectType::FARM_ROAD_CROSS) ||
                (mapObjectType >= MapObjectType::COBBLED_STREET_STRAIGHT_0 &&
                 mapObjectType <= MapObjectType::COBBLED_STREET_CROSS));
    }

    /**
     * @brief Prüft, ob die Struktur ein Platz ist, d.&nbsp;h. etwas, wo ein Marktkarren fahren darf.
     * @return `true` wenn Weg/Straße oder Platz, sonst `false`
     */
    bool isPlace() const {
        return (mapObjectType >= MapObjectType::SQUARE1 && mapObjectType <= MapObjectType::SQUARE3);
    }

    /**
	 * @brief Ermittelt, ob ein Marktkarren eine bestimmte Stelle befahren darf.
	 * @return `true`, wenn ein Marktkarren dort fahren darf, d.&nbsp;h. ein Feldweg, Pflasterstraße oder Platz da ist;
     * sonst `false`.
	 */
    bool isWalkableForCart() const {
        return (isStreet() || isPlace());
    }
};

#endif
