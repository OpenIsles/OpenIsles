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
    COBBLED_STREET_STRAIGHT_0 = 1,
    COBBLED_STREET_STRAIGHT_90,
    COBBLED_STREET_CURVE_0,
    COBBLED_STREET_CURVE_90,
    COBBLED_STREET_CURVE_180,
    COBBLED_STREET_CURVE_270,
    COBBLED_STREET_TEE_0,
    COBBLED_STREET_TEE_90,
    COBBLED_STREET_TEE_180,
    COBBLED_STREET_TEE_270,
    COBBLED_STREET_CROSS,

    FARM_ROAD_STRAIGHT_0,
    FARM_ROAD_STRAIGHT_90,
    FARM_ROAD_CURVE_0,
    FARM_ROAD_CURVE_90,
    FARM_ROAD_CURVE_180,
    FARM_ROAD_CURVE_270,
    FARM_ROAD_TEE_0,
    FARM_ROAD_TEE_90,
    FARM_ROAD_TEE_180,
    FARM_ROAD_TEE_270,
    FARM_ROAD_CROSS,

    SQUARE1,
    SQUARE2,
    SQUARE3,

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
