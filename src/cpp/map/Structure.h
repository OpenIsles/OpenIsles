#ifndef _STRUCTURE_H
#define _STRUCTURE_H

#include "economics/UpdateableObject.h"
#include "map/MapObject.h"

/**
 * Enum zur Unterscheidung der verschiedenen Strukturen
 */
typedef
enum StructureType : unsigned char {
    // keine Struktur
    NO_STRUCTURE = 0,
    
    // Strukturen
    STREET = 1,               // wird als MapObject verwendet
    STREET_STRAIGHT_0,        // werden nur zum Zeichnen verwendet und können nicht direkt platziert werden
    STREET_STRAIGHT_90,       //  "
    STREET_CURVE_0,           //  "
    STREET_CURVE_90,          //  "
    STREET_CURVE_180,         //  "
    STREET_CURVE_270,         //  "
    STREET_TEE_0,             //  "
    STREET_TEE_90,            //  "
    STREET_TEE_180,           //  "
    STREET_TEE_270,           //  "
    STREET_CROSS,             //  "
    
    // Gebäude
    START_BUILDINGS = 101, // Marker, um zu unterscheiden, ob Structure oder Building angelegt werden
            
    CHAPEL = 101,
    PIONEERS_HOUSE1,
    PIONEERS_HOUSE2,
    PIONEERS_HOUSE3,
    PIONEERS_HOUSE4,
    PIONEERS_HOUSE5,
    STONEMASON,
    OFFICE1,
    MARKETPLACE,
    FORESTERS,
    SHEEP_FARM,
    WEAVING_MILL1,
    CATTLE_FARM,
    BUTCHERS,
    TOOLSMITHS,
    HUNTERS_HUT,
            
    MAX_STRUCTURE // Marker, wie viel Platz wir im Array brauchen
} StructureType;

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
class Structure : public MapObject, public UpdateableObject {

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
