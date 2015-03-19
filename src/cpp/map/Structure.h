#ifndef _STRUCTURE_H
#define _STRUCTURE_H

#include "economics/UpdateableObject.h"
#include "map/coords/MapCoords.h"
#include "map/Direction.h"
#include "map/MapObject.h"
#include "map/StructureType.h"

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
class Structure : public MapObjectFixed, public UpdateableObject {

protected:
	/**
	 * @brief Objekt-Typ
	 */
	StructureType structureType;

    /**
     * @brief Ausrichtung des Objekts
     */
    FourthDirection view;

public:
    StructureType getStructureType() const {
        return structureType;
    }

    void setStructureType(StructureType structureType) {
        this->structureType = structureType;
    }

    const FourthDirection& getView() const {
        return view;
    }

    void setView(const FourthDirection& view) {
        this->view = view;
    }

    /**
     * @brief Prüft, ob die Struktur ein Weg/Straße ist
     * @return `true` wenn Weg/Straße, sonst `false`
     */
    bool isStreet() const {
        return ((structureType >= StructureType::FARM_ROAD_STRAIGHT_0 &&
                 structureType <= StructureType::FARM_ROAD_CROSS) ||
                (structureType >= StructureType::COBBLED_STREET_STRAIGHT_0 &&
                 structureType <= StructureType::COBBLED_STREET_CROSS));
    }
};

#endif
