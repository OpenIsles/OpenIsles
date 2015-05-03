#include <cstring>
#include <iostream>
#include <stdexcept>
#include "config/ConfigMgr.h"
#include "utils/StringFormat.h"


ConfigMgr::ConfigMgr() {
    loadMapObjectConfigs();
    loadTilesConfig();
}

ConfigMgr::~ConfigMgr() {
    for(int i = 0; i < MapObjectType::MAX_MAP_OBJECT_TYPE; i++) {
        if (mapObjectConfigs[i] != nullptr) {
            delete mapObjectConfigs[i];
        }
    }
    delete[] mapObjectConfigs;
}

void ConfigMgr::loadMapObjectConfigs() {
    mapObjectConfigs = new MapObjectConfig*[MapObjectType::MAX_MAP_OBJECT_TYPE];
    memset(mapObjectConfigs, 0, MapObjectType::MAX_MAP_OBJECT_TYPE * sizeof(MapObjectConfig*));
    
    for (int i = MapObjectType::NORTHERN_FOREST1; i <= MapObjectType::NORTHERN_FOREST2; i++) {
        MapObjectType mapObjectType = (MapObjectType) i;

        mapObjectConfigs[mapObjectType] = new MapObjectConfig();
        mapObjectConfigs[mapObjectType]->name = "Wald";
        mapObjectConfigs[mapObjectType]->structurePlacing = StructurePlacing::RECTANGLE;
        mapObjectConfigs[mapObjectType]->catchmentArea = nullptr;
        mapObjectConfigs[mapObjectType]->buildingCosts = { 5, 0, 0, 0 };
        mapObjectConfigs[mapObjectType]->buildingProduction = { GoodsSlot(), GoodsSlot(), GoodsSlot() };
    }
    mapObjectConfigs[MapObjectType::NORTHERN_FOREST1]->nameInSavefile = "northern_forest1";
    mapObjectConfigs[MapObjectType::NORTHERN_FOREST2]->nameInSavefile = "northern_forest2";
    for (int i = MapObjectType::NORTHERN_FOREST1; i <= MapObjectType::NORTHERN_FOREST2; i++) {
        MapObjectType mapObjectType = (MapObjectType) i;
        mapMapObjectNameInSavefile[mapObjectConfigs[mapObjectType]->nameInSavefile] = mapObjectType;
    }
    
    mapObjectConfigs[MapObjectType::SUGARCANE_FIELD] = new MapObjectConfig();
    mapObjectConfigs[MapObjectType::SUGARCANE_FIELD]->name = "Zuckerrohrfeld";
    mapObjectConfigs[MapObjectType::SUGARCANE_FIELD]->nameInSavefile = "sugarcane_field";
    mapObjectConfigs[MapObjectType::SUGARCANE_FIELD]->structurePlacing = StructurePlacing::RECTANGLE;
    mapObjectConfigs[MapObjectType::SUGARCANE_FIELD]->catchmentArea = nullptr;
    mapObjectConfigs[MapObjectType::SUGARCANE_FIELD]->buildingCosts = { 5, 0, 0, 0 };
    mapObjectConfigs[MapObjectType::SUGARCANE_FIELD]->buildingProduction = { GoodsSlot(), GoodsSlot(), GoodsSlot() };
    mapMapObjectNameInSavefile[mapObjectConfigs[MapObjectType::SUGARCANE_FIELD]->nameInSavefile] = MapObjectType::SUGARCANE_FIELD;

    for (int i = MapObjectType::COBBLED_STREET_STRAIGHT_0; i <= MapObjectType::COBBLED_STREET_CROSS; i++) {
        MapObjectType mapObjectType = (MapObjectType) i;

        mapObjectConfigs[mapObjectType] = new MapObjectConfig();
        mapObjectConfigs[mapObjectType]->name = "Pflasterstraße";
        mapObjectConfigs[mapObjectType]->structurePlacing = StructurePlacing::PATH;
        mapObjectConfigs[mapObjectType]->catchmentArea = nullptr;
        mapObjectConfigs[mapObjectType]->buildingCosts = { 5, 0, 0, 1 };
        mapObjectConfigs[mapObjectType]->buildingProduction = { GoodsSlot(), GoodsSlot(), GoodsSlot() };
    }
    mapObjectConfigs[MapObjectType::COBBLED_STREET_STRAIGHT_0]->nameInSavefile = "cobbled_street_straight_0";
    mapObjectConfigs[MapObjectType::COBBLED_STREET_STRAIGHT_90]->nameInSavefile = "cobbled_street_straight_90";
    mapObjectConfigs[MapObjectType::COBBLED_STREET_CURVE_0]->nameInSavefile = "cobbled_street_curve_0";
    mapObjectConfigs[MapObjectType::COBBLED_STREET_CURVE_90]->nameInSavefile = "cobbled_street_curve_90";
    mapObjectConfigs[MapObjectType::COBBLED_STREET_CURVE_180]->nameInSavefile = "cobbled_street_curve_180";
    mapObjectConfigs[MapObjectType::COBBLED_STREET_CURVE_270]->nameInSavefile = "cobbled_street_curve_270";
    mapObjectConfigs[MapObjectType::COBBLED_STREET_TEE_0]->nameInSavefile = "cobbled_street_tee_0";
    mapObjectConfigs[MapObjectType::COBBLED_STREET_TEE_90]->nameInSavefile = "cobbled_street_tee_90";
    mapObjectConfigs[MapObjectType::COBBLED_STREET_TEE_180]->nameInSavefile = "cobbled_street_tee_180";
    mapObjectConfigs[MapObjectType::COBBLED_STREET_TEE_270]->nameInSavefile = "cobbled_street_tee_270";
    mapObjectConfigs[MapObjectType::COBBLED_STREET_CROSS]->nameInSavefile = "cobbled_street_cross";
    for (int i = MapObjectType::COBBLED_STREET_STRAIGHT_0; i <= MapObjectType::COBBLED_STREET_CROSS; i++) {
        MapObjectType mapObjectType = (MapObjectType) i;
        mapMapObjectNameInSavefile[mapObjectConfigs[mapObjectType]->nameInSavefile] = mapObjectType;
    }
    
    for (int i = MapObjectType::FARM_ROAD_STRAIGHT_0; i <= MapObjectType::FARM_ROAD_CROSS; i++) {
        MapObjectType mapObjectType = (MapObjectType) i;

        mapObjectConfigs[mapObjectType] = new MapObjectConfig();
        mapObjectConfigs[mapObjectType]->name = "Feldweg";
        mapObjectConfigs[mapObjectType]->structurePlacing = StructurePlacing::PATH;
        mapObjectConfigs[mapObjectType]->catchmentArea = nullptr;
        mapObjectConfigs[mapObjectType]->buildingCosts = { 5, 0, 0, 0 };
        mapObjectConfigs[mapObjectType]->buildingProduction = { GoodsSlot(), GoodsSlot(), GoodsSlot() };
    }
    mapObjectConfigs[MapObjectType::FARM_ROAD_STRAIGHT_0]->nameInSavefile = "farm_road_straight_0";
    mapObjectConfigs[MapObjectType::FARM_ROAD_STRAIGHT_90]->nameInSavefile = "farm_road_straight_90";
    mapObjectConfigs[MapObjectType::FARM_ROAD_CURVE_0]->nameInSavefile = "farm_road_curve_0";
    mapObjectConfigs[MapObjectType::FARM_ROAD_CURVE_90]->nameInSavefile = "farm_road_curve_90";
    mapObjectConfigs[MapObjectType::FARM_ROAD_CURVE_180]->nameInSavefile = "farm_road_curve_180";
    mapObjectConfigs[MapObjectType::FARM_ROAD_CURVE_270]->nameInSavefile = "farm_road_curve_270";
    mapObjectConfigs[MapObjectType::FARM_ROAD_TEE_0]->nameInSavefile = "farm_road_tee_0";
    mapObjectConfigs[MapObjectType::FARM_ROAD_TEE_90]->nameInSavefile = "farm_road_tee_90";
    mapObjectConfigs[MapObjectType::FARM_ROAD_TEE_180]->nameInSavefile = "farm_road_tee_180";
    mapObjectConfigs[MapObjectType::FARM_ROAD_TEE_270]->nameInSavefile = "farm_road_tee_270";
    mapObjectConfigs[MapObjectType::FARM_ROAD_CROSS]->nameInSavefile = "farm_road_cross";
    for (int i = MapObjectType::FARM_ROAD_STRAIGHT_0; i <= MapObjectType::FARM_ROAD_CROSS; i++) {
        MapObjectType mapObjectType = (MapObjectType) i;
        mapMapObjectNameInSavefile[mapObjectConfigs[mapObjectType]->nameInSavefile] = mapObjectType;
    }

    for (int i = MapObjectType::SQUARE1; i <= MapObjectType::SQUARE3; i++) {
        MapObjectType mapObjectType = (MapObjectType) i;

        mapObjectConfigs[mapObjectType] = new MapObjectConfig();
        mapObjectConfigs[mapObjectType]->structurePlacing = StructurePlacing::RECTANGLE;
        mapObjectConfigs[mapObjectType]->catchmentArea = nullptr;
        mapObjectConfigs[mapObjectType]->buildingCosts = { 5, 0, 0, 1 };
        mapObjectConfigs[mapObjectType]->buildingProduction = { GoodsSlot(), GoodsSlot(), GoodsSlot() };
    }
    mapObjectConfigs[MapObjectType::SQUARE1]->name = "Platz I";
    mapObjectConfigs[MapObjectType::SQUARE2]->name = "Platz II";
    mapObjectConfigs[MapObjectType::SQUARE3]->name = "Platz III";
    mapObjectConfigs[MapObjectType::SQUARE1]->nameInSavefile = "square1";
    mapObjectConfigs[MapObjectType::SQUARE2]->nameInSavefile = "square2";
    mapObjectConfigs[MapObjectType::SQUARE3]->nameInSavefile = "square3";
    for (int i = MapObjectType::SQUARE1; i <= MapObjectType::SQUARE3; i++) {
        MapObjectType mapObjectType = (MapObjectType) i;
        mapMapObjectNameInSavefile[mapObjectConfigs[mapObjectType]->nameInSavefile] = mapObjectType;
    }

    mapObjectConfigs[MapObjectType::CHAPEL] = new MapObjectConfig();
    mapObjectConfigs[MapObjectType::CHAPEL]->name = "Kapelle";
    mapObjectConfigs[MapObjectType::CHAPEL]->nameInSavefile = "chapel";
    mapObjectConfigs[MapObjectType::CHAPEL]->structurePlacing = StructurePlacing::INDIVIDUALLY;
    mapObjectConfigs[MapObjectType::CHAPEL]->catchmentArea = new RectangleData<char>(18, 17);
    memcpy(mapObjectConfigs[MapObjectType::CHAPEL]->catchmentArea->data, "000000111111000000000011111111110000000111111111111000001111111111111100011111111111111110011111111111111110111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111011111111111111110011111111111111110001111111111111100000111111111111000000011111111110000000000111111000000", 306);
    mapObjectConfigs[MapObjectType::CHAPEL]->buildingCosts = { 100, 2, 5, 0 };
    mapObjectConfigs[MapObjectType::CHAPEL]->buildingProduction = { GoodsSlot(), GoodsSlot(), GoodsSlot() };
    mapObjectConfigs[MapObjectType::CHAPEL]->inhabitants = 1;
    mapMapObjectNameInSavefile[mapObjectConfigs[MapObjectType::CHAPEL]->nameInSavefile] = MapObjectType::CHAPEL;

    for (int i = 1; i <= 5; i++) {
        MapObjectType mapObjectType = (MapObjectType) (MapObjectType::PIONEERS_HOUSE1 + i - 1);

        mapObjectConfigs[mapObjectType] = new MapObjectConfig();
        mapObjectConfigs[mapObjectType]->name = "Haus (Pioniere)";
        mapObjectConfigs[mapObjectType]->nameInSavefile = std::string("pioneers_house") + toString(i);
        mapObjectConfigs[mapObjectType]->structurePlacing = StructurePlacing::INDIVIDUALLY;
        mapObjectConfigs[mapObjectType]->catchmentArea = new RectangleData<char>(14, 14);
        memcpy(mapObjectConfigs[mapObjectType]->catchmentArea->data, "0000111111000000011111111000001111111111000111111111111011111111111111111111111111111111111111111111111111111111111111111111111111111111111101111111111110001111111111000001111111100000001111110000", 196);
        mapObjectConfigs[mapObjectType]->buildingCosts = { 0, 0, 3, 0 };
        mapObjectConfigs[mapObjectType]->buildingProduction = { GoodsSlot(), GoodsSlot(), GoodsSlot() };
        mapMapObjectNameInSavefile[mapObjectConfigs[mapObjectType]->nameInSavefile] = mapObjectType;
    }

    for (int i = 1; i <= 5; i++) {
        MapObjectType mapObjectType = (MapObjectType) (MapObjectType::SETTLERS_HOUSE1 + i - 1);

        mapObjectConfigs[mapObjectType] = new MapObjectConfig();
        mapObjectConfigs[mapObjectType]->name = "Haus (Siedler)";
        mapObjectConfigs[mapObjectType]->nameInSavefile = std::string("settlers_house") + toString(i);
        mapObjectConfigs[mapObjectType]->structurePlacing = StructurePlacing::INDIVIDUALLY;
        mapObjectConfigs[mapObjectType]->catchmentArea = new RectangleData<char>(14, 14);
        memcpy(mapObjectConfigs[mapObjectType]->catchmentArea->data, "0000111111000000011111111000001111111111000111111111111011111111111111111111111111111111111111111111111111111111111111111111111111111111111101111111111110001111111111000001111111100000001111110000", 196);
        mapObjectConfigs[mapObjectType]->buildingCosts = { 0, 1, 3, 0 };
        mapObjectConfigs[mapObjectType]->buildingProduction = { GoodsSlot(), GoodsSlot(), GoodsSlot() };
        mapMapObjectNameInSavefile[mapObjectConfigs[mapObjectType]->nameInSavefile] = mapObjectType;
    }

    mapObjectConfigs[MapObjectType::STONEMASON] = new MapObjectConfig();
    mapObjectConfigs[MapObjectType::STONEMASON]->name = "Steinbruch";
    mapObjectConfigs[MapObjectType::STONEMASON]->nameInSavefile = "stonemason";
    mapObjectConfigs[MapObjectType::STONEMASON]->structurePlacing = StructurePlacing::INDIVIDUALLY;
    mapObjectConfigs[MapObjectType::STONEMASON]->catchmentArea = new RectangleData<char>(16, 16);
    memcpy(mapObjectConfigs[MapObjectType::STONEMASON]->catchmentArea->data, "0000011111100000000111111111100000111111111111000111111111111110011111111111111011111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111101111111111111100111111111111110001111111111110000011111111110000000011111100000", 256);
    mapObjectConfigs[MapObjectType::STONEMASON]->buildingCosts = { 100, 5, 5, 0 };
    mapObjectConfigs[MapObjectType::STONEMASON]->buildingProduction = {
        GoodsSlot(GoodsType::BRICKS, 8),
        GoodsSlot(),
        GoodsSlot()
    };
    mapObjectConfigs[MapObjectType::STONEMASON]->productionRate = 2.9;
    mapObjectConfigs[MapObjectType::STONEMASON]->inhabitants = 2;
    mapMapObjectNameInSavefile[mapObjectConfigs[MapObjectType::STONEMASON]->nameInSavefile] = MapObjectType::STONEMASON;

    mapObjectConfigs[MapObjectType::OFFICE1] = new MapObjectConfig();
    mapObjectConfigs[MapObjectType::OFFICE1]->name = "Kontor I";
    mapObjectConfigs[MapObjectType::OFFICE1]->nameInSavefile = "office1";
    mapObjectConfigs[MapObjectType::OFFICE1]->structurePlacing = StructurePlacing::INDIVIDUALLY;
    mapObjectConfigs[MapObjectType::OFFICE1]->catchmentArea = new RectangleData<char>(38, 38);
    memcpy(mapObjectConfigs[MapObjectType::OFFICE1]->catchmentArea->data, "0000000000000001111111100000000000000000000000000111111111111111100000000000000000000111111111111111111110000000000000000011111111111111111111110000000000000011111111111111111111111111000000000001111111111111111111111111111000000000111111111111111111111111111111000000001111111111111111111111111111110000000111111111111111111111111111111110000011111111111111111111111111111111110000111111111111111111111111111111111100011111111111111111111111111111111111100111111111111111111111111111111111111001111111111111111111111111111111111110011111111111111111111111111111111111101111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111011111111111111111111111111111111111100111111111111111111111111111111111111001111111111111111111111111111111111110011111111111111111111111111111111111100011111111111111111111111111111111110000111111111111111111111111111111111100000111111111111111111111111111111110000000111111111111111111111111111111000000001111111111111111111111111111110000000001111111111111111111111111111000000000001111111111111111111111111100000000000000111111111111111111111100000000000000000111111111111111111110000000000000000000011111111111111110000000000000000000000000011111111000000000000000", 1444);
    mapObjectConfigs[MapObjectType::OFFICE1]->buildingCosts = { 100, 3, 6, 0 };
    mapObjectConfigs[MapObjectType::OFFICE1]->buildingProduction = { GoodsSlot(), GoodsSlot(), GoodsSlot() };
    mapObjectConfigs[MapObjectType::OFFICE1]->inhabitants = 2;
    mapMapObjectNameInSavefile[mapObjectConfigs[MapObjectType::OFFICE1]->nameInSavefile] = MapObjectType::OFFICE1;

    mapObjectConfigs[MapObjectType::OFFICE2] = new MapObjectConfig();
    mapObjectConfigs[MapObjectType::OFFICE2]->name = "Kontor II";
    mapObjectConfigs[MapObjectType::OFFICE2]->nameInSavefile = "office2";
    mapObjectConfigs[MapObjectType::OFFICE2]->structurePlacing = StructurePlacing::INDIVIDUALLY;
    mapObjectConfigs[MapObjectType::OFFICE2]->catchmentArea = new RectangleData<char>(38, 38);
    memcpy(mapObjectConfigs[MapObjectType::OFFICE2]->catchmentArea->data, "0000000000000001111111100000000000000000000000000111111111111111100000000000000000000111111111111111111110000000000000000011111111111111111111110000000000000011111111111111111111111111000000000001111111111111111111111111111000000000111111111111111111111111111111000000001111111111111111111111111111110000000111111111111111111111111111111110000011111111111111111111111111111111110000111111111111111111111111111111111100011111111111111111111111111111111111100111111111111111111111111111111111111001111111111111111111111111111111111110011111111111111111111111111111111111101111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111011111111111111111111111111111111111100111111111111111111111111111111111111001111111111111111111111111111111111110011111111111111111111111111111111111100011111111111111111111111111111111110000111111111111111111111111111111111100000111111111111111111111111111111110000000111111111111111111111111111111000000001111111111111111111111111111110000000001111111111111111111111111111000000000001111111111111111111111111100000000000000111111111111111111111100000000000000000111111111111111111110000000000000000000011111111111111110000000000000000000000000011111111000000000000000", 1444);
    mapObjectConfigs[MapObjectType::OFFICE2]->buildingCosts = { 180, 3, 7, 0 };
    mapObjectConfigs[MapObjectType::OFFICE2]->buildingProduction = { GoodsSlot(), GoodsSlot(), GoodsSlot() };
    mapObjectConfigs[MapObjectType::OFFICE2]->inhabitants = 3;
    mapMapObjectNameInSavefile[mapObjectConfigs[MapObjectType::OFFICE2]->nameInSavefile] = MapObjectType::OFFICE2;

    mapObjectConfigs[MapObjectType::MARKETPLACE] = new MapObjectConfig();
    mapObjectConfigs[MapObjectType::MARKETPLACE]->name = "Marktplatz";
    mapObjectConfigs[MapObjectType::MARKETPLACE]->nameInSavefile = "marketplace";
    mapObjectConfigs[MapObjectType::MARKETPLACE]->structurePlacing = StructurePlacing::INDIVIDUALLY;
    mapObjectConfigs[MapObjectType::MARKETPLACE]->catchmentArea = new RectangleData<char>(38, 38);
    memcpy(mapObjectConfigs[MapObjectType::MARKETPLACE]->catchmentArea->data, "0000000000000001111111100000000000000000000000000111111111111111100000000000000000000111111111111111111110000000000000000011111111111111111111110000000000000011111111111111111111111111000000000001111111111111111111111111111000000000111111111111111111111111111111000000001111111111111111111111111111110000000111111111111111111111111111111110000011111111111111111111111111111111110000111111111111111111111111111111111100011111111111111111111111111111111111100111111111111111111111111111111111111001111111111111111111111111111111111110011111111111111111111111111111111111101111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111011111111111111111111111111111111111100111111111111111111111111111111111111001111111111111111111111111111111111110011111111111111111111111111111111111100011111111111111111111111111111111110000111111111111111111111111111111111100000111111111111111111111111111111110000000111111111111111111111111111111000000001111111111111111111111111111110000000001111111111111111111111111111000000000001111111111111111111111111100000000000000111111111111111111111100000000000000000111111111111111111110000000000000000000011111111111111110000000000000000000000000011111111000000000000000", 1444);
    mapObjectConfigs[MapObjectType::MARKETPLACE]->buildingCosts = { 200, 4, 10, 0 };
    mapObjectConfigs[MapObjectType::MARKETPLACE]->buildingProduction = { GoodsSlot(), GoodsSlot(), GoodsSlot() };
    mapObjectConfigs[MapObjectType::MARKETPLACE]->inhabitants = 3;
    mapMapObjectNameInSavefile[mapObjectConfigs[MapObjectType::MARKETPLACE]->nameInSavefile] = MapObjectType::MARKETPLACE;

    mapObjectConfigs[MapObjectType::FORESTERS] = new MapObjectConfig();
    mapObjectConfigs[MapObjectType::FORESTERS]->name = "Forsthaus";
    mapObjectConfigs[MapObjectType::FORESTERS]->nameInSavefile = "foresters";
    mapObjectConfigs[MapObjectType::FORESTERS]->structurePlacing = StructurePlacing::INDIVIDUALLY;
    mapObjectConfigs[MapObjectType::FORESTERS]->catchmentArea = new RectangleData<char>(8, 8);
    memcpy(mapObjectConfigs[MapObjectType::FORESTERS]->catchmentArea->data, "0011110001111110111111111111111111111111111111110111111000111100", 64);
    mapObjectConfigs[MapObjectType::FORESTERS]->buildingCosts = { 50, 2, 0, 0 };
    mapObjectConfigs[MapObjectType::FORESTERS]->buildingProduction = {
        GoodsSlot(GoodsType::WOOD, 10),
        GoodsSlot(),
        GoodsSlot()
    };
    mapObjectConfigs[MapObjectType::FORESTERS]->productionRate = 3.5;
    mapObjectConfigs[MapObjectType::FORESTERS]->inhabitants = 1;
    mapMapObjectNameInSavefile[mapObjectConfigs[MapObjectType::FORESTERS]->nameInSavefile] = MapObjectType::FORESTERS;

    mapObjectConfigs[MapObjectType::SHEEP_FARM] = new MapObjectConfig();
    mapObjectConfigs[MapObjectType::SHEEP_FARM]->name = "Schaffarm";
    mapObjectConfigs[MapObjectType::SHEEP_FARM]->nameInSavefile = "sheep_farm";
    mapObjectConfigs[MapObjectType::SHEEP_FARM]->structurePlacing = StructurePlacing::INDIVIDUALLY;
    mapObjectConfigs[MapObjectType::SHEEP_FARM]->catchmentArea = new RectangleData<char>(8, 8);
    memcpy(mapObjectConfigs[MapObjectType::SHEEP_FARM]->catchmentArea->data, "0011110001111110111111111111111111111111111111110111111000111100", 64);
    mapObjectConfigs[MapObjectType::SHEEP_FARM]->buildingCosts = { 200, 2, 4, 0 };
    mapObjectConfigs[MapObjectType::SHEEP_FARM]->buildingProduction = {
        GoodsSlot(GoodsType::WOOL, 3),
        GoodsSlot(),
        GoodsSlot()
    };
    mapObjectConfigs[MapObjectType::SHEEP_FARM]->productionRate = 30;
    mapObjectConfigs[MapObjectType::SHEEP_FARM]->inhabitants = 2;
    mapMapObjectNameInSavefile[mapObjectConfigs[MapObjectType::SHEEP_FARM]->nameInSavefile] = MapObjectType::SHEEP_FARM;

    mapObjectConfigs[MapObjectType::WEAVING_MILL1] = new MapObjectConfig();
    mapObjectConfigs[MapObjectType::WEAVING_MILL1]->name = "Webstube";
    mapObjectConfigs[MapObjectType::WEAVING_MILL1]->nameInSavefile = "weaving_mill1";
    mapObjectConfigs[MapObjectType::WEAVING_MILL1]->structurePlacing = StructurePlacing::INDIVIDUALLY;
    mapObjectConfigs[MapObjectType::WEAVING_MILL1]->catchmentArea = new RectangleData<char>(32, 32);
    memcpy(mapObjectConfigs[MapObjectType::WEAVING_MILL1]->catchmentArea->data, "0000000000001111111100000000000000000000011111111111111000000000000000011111111111111111100000000000001111111111111111111100000000000111111111111111111111100000000011111111111111111111111100000001111111111111111111111111100000111111111111111111111111111100001111111111111111111111111111000111111111111111111111111111111001111111111111111111111111111110011111111111111111111111111111101111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111011111111111111111111111111111100111111111111111111111111111111001111111111111111111111111111110001111111111111111111111111111000011111111111111111111111111110000011111111111111111111111111000000011111111111111111111111100000000011111111111111111111110000000000011111111111111111111000000000000011111111111111111100000000000000001111111111111100000000000000000000011111111000000000000", 1024);
    mapObjectConfigs[MapObjectType::WEAVING_MILL1]->buildingCosts = { 200, 3, 6, 0 };
    mapObjectConfigs[MapObjectType::WEAVING_MILL1]->buildingProduction = {
        GoodsSlot(GoodsType::CLOTH, 7),
        GoodsSlot(GoodsType::WOOL, 7),
        GoodsSlot()
    };
    mapObjectConfigs[MapObjectType::WEAVING_MILL1]->productionRate = 10.0;
    mapObjectConfigs[MapObjectType::WEAVING_MILL1]->inputConsumptionRate = 5.0;
    mapObjectConfigs[MapObjectType::WEAVING_MILL1]->inhabitants = 2;
    mapMapObjectNameInSavefile[mapObjectConfigs[MapObjectType::WEAVING_MILL1]->nameInSavefile] = MapObjectType::WEAVING_MILL1;

    mapObjectConfigs[MapObjectType::CATTLE_FARM] = new MapObjectConfig();
    mapObjectConfigs[MapObjectType::CATTLE_FARM]->name = "Rinderfarm";
    mapObjectConfigs[MapObjectType::CATTLE_FARM]->nameInSavefile = "cattle_farm";
    mapObjectConfigs[MapObjectType::CATTLE_FARM]->structurePlacing = StructurePlacing::INDIVIDUALLY;
    mapObjectConfigs[MapObjectType::CATTLE_FARM]->catchmentArea = new RectangleData<char>(6, 6);
    memcpy(mapObjectConfigs[MapObjectType::CATTLE_FARM]->catchmentArea->data, "011110111111111111111111111111011110", 36);
    mapObjectConfigs[MapObjectType::CATTLE_FARM]->buildingCosts = { 100, 1, 4, 0 };
    mapObjectConfigs[MapObjectType::CATTLE_FARM]->buildingProduction = {
        GoodsSlot(GoodsType::CATTLE, 4),
        GoodsSlot(),
        GoodsSlot()
    };
    mapObjectConfigs[MapObjectType::CATTLE_FARM]->productionRate = 7.5;
    mapObjectConfigs[MapObjectType::CATTLE_FARM]->inhabitants = 2;
    mapMapObjectNameInSavefile[mapObjectConfigs[MapObjectType::CATTLE_FARM]->nameInSavefile] = MapObjectType::CATTLE_FARM;

    mapObjectConfigs[MapObjectType::BUTCHERS] = new MapObjectConfig();
    mapObjectConfigs[MapObjectType::BUTCHERS]->name = "Fleischerei";
    mapObjectConfigs[MapObjectType::BUTCHERS]->nameInSavefile = "butchers";
    mapObjectConfigs[MapObjectType::BUTCHERS]->structurePlacing = StructurePlacing::INDIVIDUALLY;
    mapObjectConfigs[MapObjectType::BUTCHERS]->catchmentArea = new RectangleData<char>(22, 22);
    memcpy(mapObjectConfigs[MapObjectType::BUTCHERS]->catchmentArea->data, "0000000111111110000000000001111111111110000000001111111111111100000001111111111111111000001111111111111111110001111111111111111111100111111111111111111110111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111110111111111111111111110011111111111111111111000111111111111111111000001111111111111111000000011111111111111000000000111111111111000000000000111111110000000", 484);
    mapObjectConfigs[MapObjectType::BUTCHERS]->buildingCosts = { 150, 3, 6, 0 };
    mapObjectConfigs[MapObjectType::BUTCHERS]->buildingProduction = {
        GoodsSlot(GoodsType::FOOD, 4),
        GoodsSlot(GoodsType::CATTLE, 4),
        GoodsSlot()
    };
    mapObjectConfigs[MapObjectType::BUTCHERS]->productionRate = 5;
    mapObjectConfigs[MapObjectType::BUTCHERS]->inputConsumptionRate = 10;
    mapObjectConfigs[MapObjectType::BUTCHERS]->inhabitants = 2;
    mapMapObjectNameInSavefile[mapObjectConfigs[MapObjectType::BUTCHERS]->nameInSavefile] = MapObjectType::BUTCHERS;

    mapObjectConfigs[MapObjectType::TOOLSMITHS] = new MapObjectConfig();
    mapObjectConfigs[MapObjectType::TOOLSMITHS]->name = "Werkzeugschmiede";
    mapObjectConfigs[MapObjectType::TOOLSMITHS]->nameInSavefile = "toolsmiths";
    mapObjectConfigs[MapObjectType::TOOLSMITHS]->structurePlacing = StructurePlacing::INDIVIDUALLY;
    mapObjectConfigs[MapObjectType::TOOLSMITHS]->catchmentArea = new RectangleData<char>(32, 32);
    memcpy(mapObjectConfigs[MapObjectType::TOOLSMITHS]->catchmentArea->data, "0000000000001111111100000000000000000000011111111111111000000000000000011111111111111111100000000000001111111111111111111100000000000111111111111111111111100000000011111111111111111111111100000001111111111111111111111111100000111111111111111111111111111100001111111111111111111111111111000111111111111111111111111111111001111111111111111111111111111110011111111111111111111111111111101111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111011111111111111111111111111111100111111111111111111111111111111001111111111111111111111111111110001111111111111111111111111111000011111111111111111111111111110000011111111111111111111111111000000011111111111111111111111100000000011111111111111111111110000000000011111111111111111111000000000000011111111111111111100000000000000001111111111111100000000000000000000011111111000000000000", 1024);
    mapObjectConfigs[MapObjectType::TOOLSMITHS]->buildingCosts = { 150, 3, 2, 5 };
    mapObjectConfigs[MapObjectType::TOOLSMITHS]->buildingProduction = {
        GoodsSlot(GoodsType::TOOLS, 5),
        GoodsSlot(),
        GoodsSlot()
    };
    mapObjectConfigs[MapObjectType::TOOLSMITHS]->productionRate = 2.5;
    mapObjectConfigs[MapObjectType::TOOLSMITHS]->inhabitants = 3;
    mapMapObjectNameInSavefile[mapObjectConfigs[MapObjectType::TOOLSMITHS]->nameInSavefile] = MapObjectType::TOOLSMITHS;

    mapObjectConfigs[MapObjectType::HUNTERS_HUT] = new MapObjectConfig();
    mapObjectConfigs[MapObjectType::HUNTERS_HUT]->name = "Jagdhütte";
    mapObjectConfigs[MapObjectType::HUNTERS_HUT]->nameInSavefile = "hunters_hut";
    mapObjectConfigs[MapObjectType::HUNTERS_HUT]->structurePlacing = StructurePlacing::INDIVIDUALLY;
    mapObjectConfigs[MapObjectType::HUNTERS_HUT]->catchmentArea = new RectangleData<char>(17, 17);
    memcpy(mapObjectConfigs[MapObjectType::HUNTERS_HUT]->catchmentArea->data, "0000001111100000000001111111110000000111111111110000011111111111110001111111111111110011111111111111101111111111111111111111111111111111111111111111111111111111111111111111111111111111111011111111111111100111111111111111000111111111111100000111111111110000000111111111000000000011111000000", 289);
    mapObjectConfigs[MapObjectType::HUNTERS_HUT]->buildingCosts = { 50, 2, 2, 0 };
    mapObjectConfigs[MapObjectType::HUNTERS_HUT]->buildingProduction = {
        GoodsSlot(GoodsType::FOOD, 3),
        GoodsSlot(),
        GoodsSlot()
    };
    mapObjectConfigs[MapObjectType::HUNTERS_HUT]->productionRate = 2;
    mapObjectConfigs[MapObjectType::HUNTERS_HUT]->inhabitants = 1;
    mapMapObjectNameInSavefile[mapObjectConfigs[MapObjectType::HUNTERS_HUT]->nameInSavefile] = MapObjectType::HUNTERS_HUT;
    
    mapObjectConfigs[MapObjectType::CATHEDRAL] = new MapObjectConfig();
    mapObjectConfigs[MapObjectType::CATHEDRAL]->name = "Kathedrale";
    mapObjectConfigs[MapObjectType::CATHEDRAL]->nameInSavefile = "cathedral";
    mapObjectConfigs[MapObjectType::CATHEDRAL]->structurePlacing = StructurePlacing::INDIVIDUALLY;
    mapObjectConfigs[MapObjectType::CATHEDRAL]->catchmentArea = new RectangleData<char>(50, 50);
    memcpy(mapObjectConfigs[MapObjectType::CATHEDRAL]->catchmentArea->data, "0000000000000000000011111111110000000000000000000000000000000000001111111111111111110000000000000000000000000000001111111111111111111111000000000000000000000000001111111111111111111111111100000000000000000000000111111111111111111111111111100000000000000000000111111111111111111111111111111110000000000000000011111111111111111111111111111111110000000000000001111111111111111111111111111111111110000000000000111111111111111111111111111111111111110000000000011111111111111111111111111111111111111110000000000111111111111111111111111111111111111111100000000011111111111111111111111111111111111111111100000001111111111111111111111111111111111111111111100000011111111111111111111111111111111111111111111000001111111111111111111111111111111111111111111111000011111111111111111111111111111111111111111111110001111111111111111111111111111111111111111111111110011111111111111111111111111111111111111111111111100111111111111111111111111111111111111111111111111001111111111111111111111111111111111111111111111110111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111110111111111111111111111111111111111111111111111111001111111111111111111111111111111111111111111111110011111111111111111111111111111111111111111111111100111111111111111111111111111111111111111111111111000111111111111111111111111111111111111111111111100001111111111111111111111111111111111111111111111000001111111111111111111111111111111111111111111100000011111111111111111111111111111111111111111111000000011111111111111111111111111111111111111111100000000011111111111111111111111111111111111111110000000000111111111111111111111111111111111111111100000000000111111111111111111111111111111111111110000000000000111111111111111111111111111111111111000000000000000111111111111111111111111111111111100000000000000000111111111111111111111111111111110000000000000000000011111111111111111111111111110000000000000000000000011111111111111111111111111000000000000000000000000001111111111111111111111000000000000000000000000000000111111111111111111000000000000000000000000000000000000111111111100000000000000000000", 2500);
    mapObjectConfigs[MapObjectType::CATHEDRAL]->buildingCosts = { 7500, 25, 25, 70 };
    mapObjectConfigs[MapObjectType::CATHEDRAL]->buildingProduction = { GoodsSlot(), GoodsSlot(), GoodsSlot() };
    mapObjectConfigs[MapObjectType::CATHEDRAL]->inhabitants = 5;
    mapMapObjectNameInSavefile[mapObjectConfigs[MapObjectType::CATHEDRAL]->nameInSavefile] = MapObjectType::CATHEDRAL;

    mapObjectConfigs[MapObjectType::TAVERN] = new MapObjectConfig();
    mapObjectConfigs[MapObjectType::TAVERN]->name = "Wirtshaus";
    mapObjectConfigs[MapObjectType::TAVERN]->nameInSavefile = "tavern";
    mapObjectConfigs[MapObjectType::TAVERN]->structurePlacing = StructurePlacing::INDIVIDUALLY;
    mapObjectConfigs[MapObjectType::TAVERN]->catchmentArea = new RectangleData<char>(26, 25);
    memcpy(mapObjectConfigs[MapObjectType::TAVERN]->catchmentArea->data, "00000000011111111000000000000000011111111111100000000000011111111111111110000000001111111111111111110000000111111111111111111110000011111111111111111111110000111111111111111111111100011111111111111111111111100111111111111111111111111011111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111011111111111111111111111100111111111111111111111111000111111111111111111111100001111111111111111111111000001111111111111111111100000001111111111111111110000000001111111111111111000000000000111111111111000000000000000011111111000000000", 650);
    mapObjectConfigs[MapObjectType::TAVERN]->buildingCosts = { 250, 3, 4, 6 };
    mapObjectConfigs[MapObjectType::TAVERN]->buildingProduction = { GoodsSlot(), GoodsSlot(), GoodsSlot() };
    mapObjectConfigs[MapObjectType::TAVERN]->inhabitants = 3;
    mapMapObjectNameInSavefile[mapObjectConfigs[MapObjectType::TAVERN]->nameInSavefile] = MapObjectType::TAVERN;
    
    mapObjectConfigs[MapObjectType::SUGARCANE_PLANTATION] = new MapObjectConfig();
    mapObjectConfigs[MapObjectType::SUGARCANE_PLANTATION]->name = "Zuckerrohrplantage";
    mapObjectConfigs[MapObjectType::SUGARCANE_PLANTATION]->nameInSavefile = "sugarcane_plantation";
    mapObjectConfigs[MapObjectType::SUGARCANE_PLANTATION]->structurePlacing = StructurePlacing::INDIVIDUALLY;
    mapObjectConfigs[MapObjectType::SUGARCANE_PLANTATION]->catchmentArea = new RectangleData<char>(6, 6);
    memcpy(mapObjectConfigs[MapObjectType::SUGARCANE_PLANTATION]->catchmentArea->data, "011110111111111111111111111111011110", 36);
    mapObjectConfigs[MapObjectType::SUGARCANE_PLANTATION]->buildingCosts = { 300, 2, 3, 8 };
    mapObjectConfigs[MapObjectType::SUGARCANE_PLANTATION]->buildingProduction = {
        GoodsSlot(GoodsType::SUGARCANE, 6),
        GoodsSlot(),
        GoodsSlot()
    };
    mapObjectConfigs[MapObjectType::SUGARCANE_PLANTATION]->productionRate = 2.3;
    mapObjectConfigs[MapObjectType::SUGARCANE_PLANTATION]->inhabitants = 4;
    mapMapObjectNameInSavefile[mapObjectConfigs[MapObjectType::SUGARCANE_PLANTATION]->nameInSavefile] = MapObjectType::SUGARCANE_PLANTATION;
}

void ConfigMgr::loadTilesConfig() {
    // TODO Fehlermanagement, wenn die XML-Datei mal nicht so hübsch aussieht, dass alle Tags da sind

    // Datei öffnen
    rapidxml::file<> xmlFile("data/config/tiles.xml");

    rapidxml::xml_document<>* xmlDocument = new rapidxml::xml_document<>();
    xmlDocument->parse<0>(xmlFile.data());

    rapidxml::xml_node<>* tilesConfigNode = xmlDocument->first_node("tiles-config", 12, true);
    rapidxml::xml_node<>* tilesNode = tilesConfigNode->first_node("tiles", 5, true);

    // Tiles auslesen
    for (rapidxml::xml_node<>* tileNode = tilesNode->first_node("tile", 4, true); tileNode != nullptr;
         tileNode = tileNode->next_sibling("tile", 4, true)) {

        const char* tileName = tileNode->first_attribute("name", 4, true)->value();
        bool tileIsOcean = xmlAttributeToBool(tileNode->first_attribute("ocean", 5, true), false);
        bool tileIsWalkableAndBuildable =
            xmlAttributeToBool(tileNode->first_attribute("walkable-and-buildable", 22, true), false);

        MapTileConfig& mapTileConfig = mapTileConfigs[tileName];
        mapTileConfig.tileName = std::string(tileName);
        mapTileConfig.isOcean = tileIsOcean;
        mapTileConfig.isWalkableAndBuildable = tileIsWalkableAndBuildable;

        for (rapidxml::xml_node<>* tmxTileNode = tileNode->first_node("tmx-tile", 8, true); tmxTileNode != nullptr;
             tmxTileNode = tmxTileNode->next_sibling("tmx-tile", 8, true)) {

            int tmxTileIndex = atoi(tmxTileNode->first_attribute("tmx-tile-index", 14, true)->value());
            const char* tileViewName = tmxTileNode->first_attribute("view", 4, true)->value();
            int xOffsetInTileset = atoi(tmxTileNode->first_attribute("x", 1, true)->value());
            int yOffsetInTileset = atoi(tmxTileNode->first_attribute("y", 1, true)->value());

            FourthDirection tileView = Direction::fromString(tileViewName);
            mapTileConfig.mapTileViewsOffsetXYInTileset[tileView] =
                std::make_pair(xOffsetInTileset, yOffsetInTileset);

            // Zuordnung zu Tiled-Kachel-ID und View-Offset merken
            tiledIdToMapTileConfig[tmxTileIndex] = &mapTileConfig;
            tiledIdToViewOffset[tmxTileIndex] = tileView / 2;
        }

        // Den Ozean separat merken
        // TODO hübscher erkennen
        if (std::strcmp(tileName, "water") == 0) {
            mapTileConfigOcean = &mapTileConfig;
        }
    }

    delete xmlDocument;
}

bool ConfigMgr::xmlAttributeToBool(rapidxml::xml_attribute<>* attribute, bool defaultValue) {
    // Attribut nicht da? Default-Value verwenden
    if (attribute == nullptr) {
        return defaultValue;
    }

    const char* value = attribute->value();
    if (strcmp(value, "false") == 0) {
        return false;
    }
    else if (strcmp(value, "true") == 0) {
        return true;
    }
    else {
        std::cerr << "Illegal bool value '" << value << "'." << std::endl;
        throw std::runtime_error("Illegal bool value");
    }
}
