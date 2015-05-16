#ifndef _MAP_OBJECT_TYPE_H
#define _MAP_OBJECT_TYPE_H

/**
 * @brief Enum zur Unterscheidung der verschiedenen Map-Objekte
 */
typedef
enum MapObjectType : unsigned char {
    // kein Map-Objekt
    NO_MAP_OBJECT = 0,

    // Erntebare Landschaften
    START_HARVESTABLES = 1, // Marker, um zu unterscheiden, ob Harvestable, Structure oder Building angelegt werden
    NORTHERN_FOREST1 = 1,
    NORTHERN_FOREST2,

    SUGARCANE_FIELD,
    TOBACCO_FIELD,

    // Strukturen
    START_STRUCTURES = 51, // Marker, um zu unterscheiden, ob Harvestable, Structure oder Building angelegt werden
    COBBLED_STREET_STRAIGHT_0 = 51,
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
    START_BUILDINGS = 101, // Marker, um zu unterscheiden, ob Harvestable, Structure oder Building angelegt werden

    CHAPEL = 101,
    PIONEERS_HOUSE1,
    PIONEERS_HOUSE2,
    PIONEERS_HOUSE3,
    PIONEERS_HOUSE4,
    PIONEERS_HOUSE5,
    SETTLERS_HOUSE1,
    SETTLERS_HOUSE2,
    SETTLERS_HOUSE3,
    SETTLERS_HOUSE4,
    SETTLERS_HOUSE5,
    STONEMASON,
    OFFICE1,
    OFFICE2,
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
    SUGARCANE_PLANTATION,
    TOBACCO_PLANTATION,
    DISTILLERY,

    MAX_MAP_OBJECT_TYPE // Marker, wie viel Platz wir im Array brauchen
} MapObjectType;

#endif
