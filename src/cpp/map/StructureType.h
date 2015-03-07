#ifndef _STRUCTURE_TYPE_H
#define _STRUCTURE_TYPE_H

/**
 * @brief Enum zur Unterscheidung der verschiedenen Strukturen
 */
typedef
enum StructureType : unsigned char {
    // keine Struktur
    NO_STRUCTURE = 0,

    // Strukturen
    STREET_STRAIGHT_0 = 1,
    STREET_STRAIGHT_90,
    STREET_CURVE_0,
    STREET_CURVE_90,
    STREET_CURVE_180,
    STREET_CURVE_270,
    STREET_TEE_0,
    STREET_TEE_90,
    STREET_TEE_180,
    STREET_TEE_270,
    STREET_CROSS,

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
    CATHEDRAL,
    TAVERN,

    MAX_STRUCTURE // Marker, wie viel Platz wir im Array brauchen
} StructureType;

#endif
