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
    STREET = 1,               // wird als MapObject verwendet
    STREET_STRAIGHT_0 = 2,    // werden nur zum Zeichnen verwendet und können nicht direkt platziert werden
    STREET_STRAIGHT_90 = 3,   //  "
    STREET_CURVE_0 = 4,       //  "
    STREET_CURVE_90 = 5,      //  "
    STREET_CURVE_180 = 6,     //  "
    STREET_CURVE_270 = 7,     //  "
    STREET_TEE_0 = 8,         //  "
    STREET_TEE_90 = 9,        //  "
    STREET_TEE_180 = 10,      //  "
    STREET_TEE_270 = 11,      //  "
    STREET_CROSS = 12,        //  "
    
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
