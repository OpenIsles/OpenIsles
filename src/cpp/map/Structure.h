#ifndef _STRUCTURE_H
#define _STRUCTURE_H

#include "map/coords/MapCoords.h"
#include "map/Direction.h"
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

protected:
    /**
     * @brief Ausrichtung des Objekts
     */
    FourthDirection view;

public:
    virtual bool updateObject(const Context& context) override {
        return true;
    }

    const FourthDirection& getView() const {
        return view;
    }

    void setView(const FourthDirection& view) {
        this->view = view;
    }

    /**
     * @brief Prüft, ob die Struktur ein Weg/Straße oder Platz ist, d.&nbsp;h. etwas, wo ein Marktkarren fahren darf.
     * @return `true` wenn Weg/Straße oder Platz, sonst `false`
     */
    bool isStreet() const {
        return ((mapObjectType >= MapObjectType::FARM_ROAD_STRAIGHT_0 &&
                 mapObjectType <= MapObjectType::FARM_ROAD_CROSS) ||
                (mapObjectType >= MapObjectType::COBBLED_STREET_STRAIGHT_0 &&
                 mapObjectType <= MapObjectType::COBBLED_STREET_CROSS) ||
                (mapObjectType >= MapObjectType::SQUARE1 &&
                 mapObjectType <= MapObjectType::SQUARE3));
    }
};

#endif
