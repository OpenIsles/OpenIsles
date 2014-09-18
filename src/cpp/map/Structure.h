#ifndef _STRUCTURE_H
#define _STRUCTURE_H

#include "MapObject.h"

/**
 * Enum zur Unterscheidung der verschiedenen Strukturen
 */
typedef
enum StructureType : unsigned char {
    // keine Struktur
    NO_STRUCTURE = 0,
    
    // Strukturen
    WAY_E = 1,
    WAY_N = 2,
    WAY_S = 3,
    WAY_W = 4,
    WAY_SW_NE = 5,
    WAY_NW_SE = 6,
    
    // Gebäude
    START_BUILDINGS = 101, // Marker, um zu unterscheiden, ob Structure oder Building angelegt werden
            
    CHAPEL = 101,
    PIONEERS_HOUSE1 = 102,
    SIGNALFIRE = 103,
    HERBARY = 104,
    BRICKYARD = 105,
    BRICKYARD2 = 106,
    OFFICE1 = 107,
    MARKETPLACE = 108,
    FORESTERS = 109,
            
    MAX_STRUCTURE // Marker, wie viel Platz wir im Array brauchen
} StructureType;

/**
 * Enum für die Gebäude-Gruppen, d.h. wie sie im Baumenü angeordnet werden
 */
typedef
enum BuildingGroup : unsigned char {

    // Handwerksbetriebe
    CRAFTSMAN,

    // Farmen & Plantagen
    FARM,

    // Hafenanlagen
    PORT,

    // Öffentliche Gebäude
    PUBLIC

} BuildingGroup;


/**
 * @brief MapObject, das eine Struktur darstellt. Strukturen sind Wege und Felder, also Sachen nicht klickbar sind.
 */
class Structure : public MapObject {

protected:
	/**
	 * @brief Objekt-Typ
	 */
	StructureType structureType;

public:
    StructureType getStructureType() const {
        return structureType;
    }

    void setStructureType(StructureType structureType) {
        this->structureType = structureType;
    }

};

#endif
