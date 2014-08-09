#ifndef _STRUCTURE_H
#define _STRUCTURE_H

#include "MapObject.h"

/**
 * Enum zur Unterscheidung der verschiedenen Strukturen
 */
typedef
enum StructureType : unsigned char {
    // Strukturen
    WAY_E = 1,
    WAY_N = 2,
    WAY_S = 3,
    WAY_W = 4,
    WAY_SW_NE = 5,
    WAY_NW_SE = 6,
    
    // Gebäude
    CHAPEL = 101,
    WEAPONSMITH = 102,
    SIGNALFIRE = 103,
    HERBARY = 104,
    BRICKYARD = 105,
    BRICKYARD2 = 106,  
} StructureType;


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
