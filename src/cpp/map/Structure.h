#ifndef _STRUCTURE_H
#define _STRUCTURE_H

#include "economics/UpdateableObject.h"
#include "map/coords/MapCoords.h"
#include "map/Directions.h"
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
    FourDirectionsView view;

public:
    StructureType getStructureType() const {
        return structureType;
    }

    void setStructureType(StructureType structureType) {
        this->structureType = structureType;
    }

    const FourDirectionsView& getView() const {
        return view;
    }

    void setView(const FourDirectionsView& view) {
        this->view = view;
    }
};

#endif
