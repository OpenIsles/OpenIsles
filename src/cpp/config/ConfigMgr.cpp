#include <cstring>
#include <iostream>
#include <stdexcept>
#include "config/ConfigMgr.h"
#include "utils/StringFormat.h"


ConfigMgr::ConfigMgr() {
    loadBuildingConfig();
    loadTilesConfig();

    // TODO hübsch in Methode auslagern. In loadBuildingConfig() passt es aktuell noch nicht -> *Building*-Config muss umbenannt werden
    mapMapObjectNameInSavefile["northern_forest1"] = MapObjectType::NORTHERN_FOREST1;
    mapMapObjectNameInSavefile["northern_forest2"] = MapObjectType::NORTHERN_FOREST2;
}

ConfigMgr::~ConfigMgr() {
    for(int i = 0; i < MapObjectType::MAX_MAP_OBJECT_TYPE; i++) {
        if (buildingConfigs[i] != nullptr) {
            delete buildingConfigs[i];
        }
    }
    delete[] buildingConfigs;
}

void ConfigMgr::loadBuildingConfig() {
    buildingConfigs = new BuildingConfig*[MapObjectType::MAX_MAP_OBJECT_TYPE];
    memset(buildingConfigs, 0, MapObjectType::MAX_MAP_OBJECT_TYPE * sizeof(BuildingConfig*));

    for (int i = MapObjectType::COBBLED_STREET_STRAIGHT_0; i <= MapObjectType::COBBLED_STREET_CROSS; i++) {
        MapObjectType mapObjectType = (MapObjectType) i;

        buildingConfigs[mapObjectType] = new BuildingConfig();
        buildingConfigs[mapObjectType]->name = "Pflasterstraße";
        buildingConfigs[mapObjectType]->structurePlacing = StructurePlacing::PATH;
        buildingConfigs[mapObjectType]->catchmentArea = nullptr;
        buildingConfigs[mapObjectType]->buildingCosts = { 5, 0, 0, 1 };
        buildingConfigs[mapObjectType]->buildingProduction = { GoodsSlot(), GoodsSlot(), GoodsSlot() };
    }
    buildingConfigs[MapObjectType::COBBLED_STREET_STRAIGHT_0]->nameInSavefile = "cobbled_street_straight_0";
    buildingConfigs[MapObjectType::COBBLED_STREET_STRAIGHT_90]->nameInSavefile = "cobbled_street_straight_90";
    buildingConfigs[MapObjectType::COBBLED_STREET_CURVE_0]->nameInSavefile = "cobbled_street_curve_0";
    buildingConfigs[MapObjectType::COBBLED_STREET_CURVE_90]->nameInSavefile = "cobbled_street_curve_90";
    buildingConfigs[MapObjectType::COBBLED_STREET_CURVE_180]->nameInSavefile = "cobbled_street_curve_180";
    buildingConfigs[MapObjectType::COBBLED_STREET_CURVE_270]->nameInSavefile = "cobbled_street_curve_270";
    buildingConfigs[MapObjectType::COBBLED_STREET_TEE_0]->nameInSavefile = "cobbled_street_tee_0";
    buildingConfigs[MapObjectType::COBBLED_STREET_TEE_90]->nameInSavefile = "cobbled_street_tee_90";
    buildingConfigs[MapObjectType::COBBLED_STREET_TEE_180]->nameInSavefile = "cobbled_street_tee_180";
    buildingConfigs[MapObjectType::COBBLED_STREET_TEE_270]->nameInSavefile = "cobbled_street_tee_270";
    buildingConfigs[MapObjectType::COBBLED_STREET_CROSS]->nameInSavefile = "cobbled_street_cross";
    for (int i = MapObjectType::COBBLED_STREET_STRAIGHT_0; i <= MapObjectType::COBBLED_STREET_CROSS; i++) {
        MapObjectType mapObjectType = (MapObjectType) i;
        mapMapObjectNameInSavefile[buildingConfigs[mapObjectType]->nameInSavefile] = mapObjectType;
    }
    
    for (int i = MapObjectType::FARM_ROAD_STRAIGHT_0; i <= MapObjectType::FARM_ROAD_CROSS; i++) {
        MapObjectType mapObjectType = (MapObjectType) i;

        buildingConfigs[mapObjectType] = new BuildingConfig();
        buildingConfigs[mapObjectType]->name = "Feldweg";
        buildingConfigs[mapObjectType]->structurePlacing = StructurePlacing::PATH;
        buildingConfigs[mapObjectType]->catchmentArea = nullptr;
        buildingConfigs[mapObjectType]->buildingCosts = { 5, 0, 0, 0 };
        buildingConfigs[mapObjectType]->buildingProduction = { GoodsSlot(), GoodsSlot(), GoodsSlot() };
    }
    buildingConfigs[MapObjectType::FARM_ROAD_STRAIGHT_0]->nameInSavefile = "farm_road_straight_0";
    buildingConfigs[MapObjectType::FARM_ROAD_STRAIGHT_90]->nameInSavefile = "farm_road_straight_90";
    buildingConfigs[MapObjectType::FARM_ROAD_CURVE_0]->nameInSavefile = "farm_road_curve_0";
    buildingConfigs[MapObjectType::FARM_ROAD_CURVE_90]->nameInSavefile = "farm_road_curve_90";
    buildingConfigs[MapObjectType::FARM_ROAD_CURVE_180]->nameInSavefile = "farm_road_curve_180";
    buildingConfigs[MapObjectType::FARM_ROAD_CURVE_270]->nameInSavefile = "farm_road_curve_270";
    buildingConfigs[MapObjectType::FARM_ROAD_TEE_0]->nameInSavefile = "farm_road_tee_0";
    buildingConfigs[MapObjectType::FARM_ROAD_TEE_90]->nameInSavefile = "farm_road_tee_90";
    buildingConfigs[MapObjectType::FARM_ROAD_TEE_180]->nameInSavefile = "farm_road_tee_180";
    buildingConfigs[MapObjectType::FARM_ROAD_TEE_270]->nameInSavefile = "farm_road_tee_270";
    buildingConfigs[MapObjectType::FARM_ROAD_CROSS]->nameInSavefile = "farm_road_cross";
    for (int i = MapObjectType::FARM_ROAD_STRAIGHT_0; i <= MapObjectType::FARM_ROAD_CROSS; i++) {
        MapObjectType mapObjectType = (MapObjectType) i;
        mapMapObjectNameInSavefile[buildingConfigs[mapObjectType]->nameInSavefile] = mapObjectType;
    }

    for (int i = MapObjectType::SQUARE1; i <= MapObjectType::SQUARE3; i++) {
        MapObjectType mapObjectType = (MapObjectType) i;

        buildingConfigs[mapObjectType] = new BuildingConfig();
        buildingConfigs[mapObjectType]->structurePlacing = StructurePlacing::RECTANGLE;
        buildingConfigs[mapObjectType]->catchmentArea = nullptr;
        buildingConfigs[mapObjectType]->buildingCosts = { 5, 0, 0, 1 };
        buildingConfigs[mapObjectType]->buildingProduction = { GoodsSlot(), GoodsSlot(), GoodsSlot() };
    }
    buildingConfigs[MapObjectType::SQUARE1]->name = "Platz I";
    buildingConfigs[MapObjectType::SQUARE2]->name = "Platz II";
    buildingConfigs[MapObjectType::SQUARE3]->name = "Platz III";
    buildingConfigs[MapObjectType::SQUARE1]->nameInSavefile = "square1";
    buildingConfigs[MapObjectType::SQUARE2]->nameInSavefile = "square2";
    buildingConfigs[MapObjectType::SQUARE3]->nameInSavefile = "square3";
    for (int i = MapObjectType::SQUARE1; i <= MapObjectType::SQUARE3; i++) {
        MapObjectType mapObjectType = (MapObjectType) i;
        mapMapObjectNameInSavefile[buildingConfigs[mapObjectType]->nameInSavefile] = mapObjectType;
    }

    buildingConfigs[MapObjectType::CHAPEL] = new BuildingConfig();
    buildingConfigs[MapObjectType::CHAPEL]->name = "Kapelle";
    buildingConfigs[MapObjectType::CHAPEL]->nameInSavefile = "chapel";
    buildingConfigs[MapObjectType::CHAPEL]->structurePlacing = StructurePlacing::INDIVIDUALLY;
    buildingConfigs[MapObjectType::CHAPEL]->catchmentArea = new RectangleData<char>(18, 17);
    memcpy(buildingConfigs[MapObjectType::CHAPEL]->catchmentArea->data, "000000111111000000000011111111110000000111111111111000001111111111111100011111111111111110011111111111111110111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111011111111111111110011111111111111110001111111111111100000111111111111000000011111111110000000000111111000000", 306);
    buildingConfigs[MapObjectType::CHAPEL]->buildingCosts = { 100, 2, 5, 0 };
    buildingConfigs[MapObjectType::CHAPEL]->buildingProduction = { GoodsSlot(), GoodsSlot(), GoodsSlot() };
    buildingConfigs[MapObjectType::CHAPEL]->inhabitants = 1;
    mapMapObjectNameInSavefile[buildingConfigs[MapObjectType::CHAPEL]->nameInSavefile] = MapObjectType::CHAPEL;

    for (int i = 1; i <= 5; i++) {
        MapObjectType mapObjectType = (MapObjectType) (MapObjectType::PIONEERS_HOUSE1 + i - 1);

        buildingConfigs[mapObjectType] = new BuildingConfig();
        buildingConfigs[mapObjectType]->name = "Haus (Pioniere)";
        buildingConfigs[mapObjectType]->nameInSavefile = std::string("pioneers_house") + toString(i);
        buildingConfigs[mapObjectType]->structurePlacing = StructurePlacing::INDIVIDUALLY;
        buildingConfigs[mapObjectType]->catchmentArea = new RectangleData<char>(14, 14);
        memcpy(buildingConfigs[mapObjectType]->catchmentArea->data, "0000111111000000011111111000001111111111000111111111111011111111111111111111111111111111111111111111111111111111111111111111111111111111111101111111111110001111111111000001111111100000001111110000", 196);
        buildingConfigs[mapObjectType]->buildingCosts = { 0, 0, 3, 0 };
        buildingConfigs[mapObjectType]->buildingProduction = { GoodsSlot(), GoodsSlot(), GoodsSlot() };
        mapMapObjectNameInSavefile[buildingConfigs[mapObjectType]->nameInSavefile] = mapObjectType;
    }

    for (int i = 1; i <= 4 /* TODO more to come :-) */; i++) {
        MapObjectType mapObjectType = (MapObjectType) (MapObjectType::SETTLERS_HOUSE1 + i - 1);

        buildingConfigs[mapObjectType] = new BuildingConfig();
        buildingConfigs[mapObjectType]->name = "Haus (Siedler)";
        buildingConfigs[mapObjectType]->nameInSavefile = std::string("settlers_house") + toString(i);
        buildingConfigs[mapObjectType]->structurePlacing = StructurePlacing::INDIVIDUALLY;
        buildingConfigs[mapObjectType]->catchmentArea = new RectangleData<char>(14, 14);
        memcpy(buildingConfigs[mapObjectType]->catchmentArea->data, "0000111111000000011111111000001111111111000111111111111011111111111111111111111111111111111111111111111111111111111111111111111111111111111101111111111110001111111111000001111111100000001111110000", 196);
        buildingConfigs[mapObjectType]->buildingCosts = { 0, 1, 3, 0 };
        buildingConfigs[mapObjectType]->buildingProduction = { GoodsSlot(), GoodsSlot(), GoodsSlot() };
        mapMapObjectNameInSavefile[buildingConfigs[mapObjectType]->nameInSavefile] = mapObjectType;
    }

    buildingConfigs[MapObjectType::STONEMASON] = new BuildingConfig();
    buildingConfigs[MapObjectType::STONEMASON]->name = "Steinbruch";
    buildingConfigs[MapObjectType::STONEMASON]->nameInSavefile = "stonemason";
    buildingConfigs[MapObjectType::STONEMASON]->structurePlacing = StructurePlacing::INDIVIDUALLY;
    buildingConfigs[MapObjectType::STONEMASON]->catchmentArea = new RectangleData<char>(16, 16);
    memcpy(buildingConfigs[MapObjectType::STONEMASON]->catchmentArea->data, "0000011111100000000111111111100000111111111111000111111111111110011111111111111011111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111101111111111111100111111111111110001111111111110000011111111110000000011111100000", 256);
    buildingConfigs[MapObjectType::STONEMASON]->buildingCosts = { 100, 5, 5, 0 };
    buildingConfigs[MapObjectType::STONEMASON]->buildingProduction = {
        GoodsSlot(GoodsType::BRICKS, 8),
        GoodsSlot(),
        GoodsSlot()
    };
    buildingConfigs[MapObjectType::STONEMASON]->productionRate = 2.9;
    buildingConfigs[MapObjectType::STONEMASON]->inhabitants = 2;
    mapMapObjectNameInSavefile[buildingConfigs[MapObjectType::STONEMASON]->nameInSavefile] = MapObjectType::STONEMASON;

    buildingConfigs[MapObjectType::OFFICE1] = new BuildingConfig();
    buildingConfigs[MapObjectType::OFFICE1]->name = "Kontor I";
    buildingConfigs[MapObjectType::OFFICE1]->nameInSavefile = "office1";
    buildingConfigs[MapObjectType::OFFICE1]->structurePlacing = StructurePlacing::INDIVIDUALLY;
    buildingConfigs[MapObjectType::OFFICE1]->catchmentArea = new RectangleData<char>(38, 38);
    memcpy(buildingConfigs[MapObjectType::OFFICE1]->catchmentArea->data, "0000000000000001111111100000000000000000000000000111111111111111100000000000000000000111111111111111111110000000000000000011111111111111111111110000000000000011111111111111111111111111000000000001111111111111111111111111111000000000111111111111111111111111111111000000001111111111111111111111111111110000000111111111111111111111111111111110000011111111111111111111111111111111110000111111111111111111111111111111111100011111111111111111111111111111111111100111111111111111111111111111111111111001111111111111111111111111111111111110011111111111111111111111111111111111101111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111011111111111111111111111111111111111100111111111111111111111111111111111111001111111111111111111111111111111111110011111111111111111111111111111111111100011111111111111111111111111111111110000111111111111111111111111111111111100000111111111111111111111111111111110000000111111111111111111111111111111000000001111111111111111111111111111110000000001111111111111111111111111111000000000001111111111111111111111111100000000000000111111111111111111111100000000000000000111111111111111111110000000000000000000011111111111111110000000000000000000000000011111111000000000000000", 1444);
    buildingConfigs[MapObjectType::OFFICE1]->buildingCosts = { 100, 3, 6, 0 };
    buildingConfigs[MapObjectType::OFFICE1]->buildingProduction = { GoodsSlot(), GoodsSlot(), GoodsSlot() };
    buildingConfigs[MapObjectType::OFFICE1]->inhabitants = 2;
    mapMapObjectNameInSavefile[buildingConfigs[MapObjectType::OFFICE1]->nameInSavefile] = MapObjectType::OFFICE1;

    buildingConfigs[MapObjectType::OFFICE2] = new BuildingConfig();
    buildingConfigs[MapObjectType::OFFICE2]->name = "Kontor II";
    buildingConfigs[MapObjectType::OFFICE2]->nameInSavefile = "office2";
    buildingConfigs[MapObjectType::OFFICE2]->structurePlacing = StructurePlacing::INDIVIDUALLY;
    buildingConfigs[MapObjectType::OFFICE2]->catchmentArea = new RectangleData<char>(38, 38);
    memcpy(buildingConfigs[MapObjectType::OFFICE2]->catchmentArea->data, "0000000000000001111111100000000000000000000000000111111111111111100000000000000000000111111111111111111110000000000000000011111111111111111111110000000000000011111111111111111111111111000000000001111111111111111111111111111000000000111111111111111111111111111111000000001111111111111111111111111111110000000111111111111111111111111111111110000011111111111111111111111111111111110000111111111111111111111111111111111100011111111111111111111111111111111111100111111111111111111111111111111111111001111111111111111111111111111111111110011111111111111111111111111111111111101111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111011111111111111111111111111111111111100111111111111111111111111111111111111001111111111111111111111111111111111110011111111111111111111111111111111111100011111111111111111111111111111111110000111111111111111111111111111111111100000111111111111111111111111111111110000000111111111111111111111111111111000000001111111111111111111111111111110000000001111111111111111111111111111000000000001111111111111111111111111100000000000000111111111111111111111100000000000000000111111111111111111110000000000000000000011111111111111110000000000000000000000000011111111000000000000000", 1444);
    buildingConfigs[MapObjectType::OFFICE2]->buildingCosts = { 180, 3, 7, 0 };
    buildingConfigs[MapObjectType::OFFICE2]->buildingProduction = { GoodsSlot(), GoodsSlot(), GoodsSlot() };
    buildingConfigs[MapObjectType::OFFICE2]->inhabitants = 3;
    mapMapObjectNameInSavefile[buildingConfigs[MapObjectType::OFFICE2]->nameInSavefile] = MapObjectType::OFFICE2;

    buildingConfigs[MapObjectType::MARKETPLACE] = new BuildingConfig();
    buildingConfigs[MapObjectType::MARKETPLACE]->name = "Marktplatz";
    buildingConfigs[MapObjectType::MARKETPLACE]->nameInSavefile = "marketplace";
    buildingConfigs[MapObjectType::MARKETPLACE]->structurePlacing = StructurePlacing::INDIVIDUALLY;
    buildingConfigs[MapObjectType::MARKETPLACE]->catchmentArea = new RectangleData<char>(38, 38);
    memcpy(buildingConfigs[MapObjectType::MARKETPLACE]->catchmentArea->data, "0000000000000001111111100000000000000000000000000111111111111111100000000000000000000111111111111111111110000000000000000011111111111111111111110000000000000011111111111111111111111111000000000001111111111111111111111111111000000000111111111111111111111111111111000000001111111111111111111111111111110000000111111111111111111111111111111110000011111111111111111111111111111111110000111111111111111111111111111111111100011111111111111111111111111111111111100111111111111111111111111111111111111001111111111111111111111111111111111110011111111111111111111111111111111111101111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111011111111111111111111111111111111111100111111111111111111111111111111111111001111111111111111111111111111111111110011111111111111111111111111111111111100011111111111111111111111111111111110000111111111111111111111111111111111100000111111111111111111111111111111110000000111111111111111111111111111111000000001111111111111111111111111111110000000001111111111111111111111111111000000000001111111111111111111111111100000000000000111111111111111111111100000000000000000111111111111111111110000000000000000000011111111111111110000000000000000000000000011111111000000000000000", 1444);
    buildingConfigs[MapObjectType::MARKETPLACE]->buildingCosts = { 200, 4, 10, 0 };
    buildingConfigs[MapObjectType::MARKETPLACE]->buildingProduction = { GoodsSlot(), GoodsSlot(), GoodsSlot() };
    buildingConfigs[MapObjectType::MARKETPLACE]->inhabitants = 3;
    mapMapObjectNameInSavefile[buildingConfigs[MapObjectType::MARKETPLACE]->nameInSavefile] = MapObjectType::MARKETPLACE;

    buildingConfigs[MapObjectType::FORESTERS] = new BuildingConfig();
    buildingConfigs[MapObjectType::FORESTERS]->name = "Forsthaus";
    buildingConfigs[MapObjectType::FORESTERS]->nameInSavefile = "foresters";
    buildingConfigs[MapObjectType::FORESTERS]->structurePlacing = StructurePlacing::INDIVIDUALLY;
    buildingConfigs[MapObjectType::FORESTERS]->catchmentArea = new RectangleData<char>(8, 8);
    memcpy(buildingConfigs[MapObjectType::FORESTERS]->catchmentArea->data, "0011110001111110111111111111111111111111111111110111111000111100", 64);
    buildingConfigs[MapObjectType::FORESTERS]->buildingCosts = { 50, 2, 0, 0 };
    buildingConfigs[MapObjectType::FORESTERS]->buildingProduction = {
        GoodsSlot(GoodsType::WOOD, 10),
        GoodsSlot(),
        GoodsSlot()
    };
    buildingConfigs[MapObjectType::FORESTERS]->productionRate = 3.5;
    buildingConfigs[MapObjectType::FORESTERS]->inhabitants = 1;
    mapMapObjectNameInSavefile[buildingConfigs[MapObjectType::FORESTERS]->nameInSavefile] = MapObjectType::FORESTERS;

    buildingConfigs[MapObjectType::SHEEP_FARM] = new BuildingConfig();
    buildingConfigs[MapObjectType::SHEEP_FARM]->name = "Schaffarm";
    buildingConfigs[MapObjectType::SHEEP_FARM]->nameInSavefile = "sheep_farm";
    buildingConfigs[MapObjectType::SHEEP_FARM]->structurePlacing = StructurePlacing::INDIVIDUALLY;
    buildingConfigs[MapObjectType::SHEEP_FARM]->catchmentArea = new RectangleData<char>(8, 8);
    memcpy(buildingConfigs[MapObjectType::SHEEP_FARM]->catchmentArea->data, "0011110001111110111111111111111111111111111111110111111000111100", 64);
    buildingConfigs[MapObjectType::SHEEP_FARM]->buildingCosts = { 200, 2, 4, 0 };
    buildingConfigs[MapObjectType::SHEEP_FARM]->buildingProduction = {
        GoodsSlot(GoodsType::WOOL, 3),
        GoodsSlot(),
        GoodsSlot()
    };
    buildingConfigs[MapObjectType::SHEEP_FARM]->productionRate = 30;
    buildingConfigs[MapObjectType::SHEEP_FARM]->inhabitants = 2;
    mapMapObjectNameInSavefile[buildingConfigs[MapObjectType::SHEEP_FARM]->nameInSavefile] = MapObjectType::SHEEP_FARM;

    buildingConfigs[MapObjectType::WEAVING_MILL1] = new BuildingConfig();
    buildingConfigs[MapObjectType::WEAVING_MILL1]->name = "Webstube";
    buildingConfigs[MapObjectType::WEAVING_MILL1]->nameInSavefile = "weaving_mill1";
    buildingConfigs[MapObjectType::WEAVING_MILL1]->structurePlacing = StructurePlacing::INDIVIDUALLY;
    buildingConfigs[MapObjectType::WEAVING_MILL1]->catchmentArea = new RectangleData<char>(32, 32);
    memcpy(buildingConfigs[MapObjectType::WEAVING_MILL1]->catchmentArea->data, "0000000000001111111100000000000000000000011111111111111000000000000000011111111111111111100000000000001111111111111111111100000000000111111111111111111111100000000011111111111111111111111100000001111111111111111111111111100000111111111111111111111111111100001111111111111111111111111111000111111111111111111111111111111001111111111111111111111111111110011111111111111111111111111111101111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111011111111111111111111111111111100111111111111111111111111111111001111111111111111111111111111110001111111111111111111111111111000011111111111111111111111111110000011111111111111111111111111000000011111111111111111111111100000000011111111111111111111110000000000011111111111111111111000000000000011111111111111111100000000000000001111111111111100000000000000000000011111111000000000000", 1024);
    buildingConfigs[MapObjectType::WEAVING_MILL1]->buildingCosts = { 200, 3, 6, 0 };
    buildingConfigs[MapObjectType::WEAVING_MILL1]->buildingProduction = {
        GoodsSlot(GoodsType::CLOTH, 7),
        GoodsSlot(GoodsType::WOOL, 7),
        GoodsSlot()
    };
    buildingConfigs[MapObjectType::WEAVING_MILL1]->productionRate = 10.0;
    buildingConfigs[MapObjectType::WEAVING_MILL1]->inputConsumptionRate = 5.0;
    buildingConfigs[MapObjectType::WEAVING_MILL1]->inhabitants = 2;
    mapMapObjectNameInSavefile[buildingConfigs[MapObjectType::WEAVING_MILL1]->nameInSavefile] = MapObjectType::WEAVING_MILL1;

    buildingConfigs[MapObjectType::CATTLE_FARM] = new BuildingConfig();
    buildingConfigs[MapObjectType::CATTLE_FARM]->name = "Rinderfarm";
    buildingConfigs[MapObjectType::CATTLE_FARM]->nameInSavefile = "cattle_farm";
    buildingConfigs[MapObjectType::CATTLE_FARM]->structurePlacing = StructurePlacing::INDIVIDUALLY;
    buildingConfigs[MapObjectType::CATTLE_FARM]->catchmentArea = new RectangleData<char>(6, 6);
    memcpy(buildingConfigs[MapObjectType::CATTLE_FARM]->catchmentArea->data, "011110111111111111111111111111011110", 36);
    buildingConfigs[MapObjectType::CATTLE_FARM]->buildingCosts = { 100, 1, 4, 0 };
    buildingConfigs[MapObjectType::CATTLE_FARM]->buildingProduction = {
        GoodsSlot(GoodsType::CATTLE, 4),
        GoodsSlot(),
        GoodsSlot()
    };
    buildingConfigs[MapObjectType::CATTLE_FARM]->productionRate = 7.5;
    buildingConfigs[MapObjectType::CATTLE_FARM]->inhabitants = 2;
    mapMapObjectNameInSavefile[buildingConfigs[MapObjectType::CATTLE_FARM]->nameInSavefile] = MapObjectType::CATTLE_FARM;

    buildingConfigs[MapObjectType::BUTCHERS] = new BuildingConfig();
    buildingConfigs[MapObjectType::BUTCHERS]->name = "Fleischerei";
    buildingConfigs[MapObjectType::BUTCHERS]->nameInSavefile = "butchers";
    buildingConfigs[MapObjectType::BUTCHERS]->structurePlacing = StructurePlacing::INDIVIDUALLY;
    buildingConfigs[MapObjectType::BUTCHERS]->catchmentArea = new RectangleData<char>(22, 22);
    memcpy(buildingConfigs[MapObjectType::BUTCHERS]->catchmentArea->data, "0000000111111110000000000001111111111110000000001111111111111100000001111111111111111000001111111111111111110001111111111111111111100111111111111111111110111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111110111111111111111111110011111111111111111111000111111111111111111000001111111111111111000000011111111111111000000000111111111111000000000000111111110000000", 484);
    buildingConfigs[MapObjectType::BUTCHERS]->buildingCosts = { 150, 3, 6, 0 };
    buildingConfigs[MapObjectType::BUTCHERS]->buildingProduction = {
        GoodsSlot(GoodsType::FOOD, 4),
        GoodsSlot(GoodsType::CATTLE, 4),
        GoodsSlot()
    };
    buildingConfigs[MapObjectType::BUTCHERS]->productionRate = 5;
    buildingConfigs[MapObjectType::BUTCHERS]->inputConsumptionRate = 10;
    buildingConfigs[MapObjectType::BUTCHERS]->inhabitants = 2;
    mapMapObjectNameInSavefile[buildingConfigs[MapObjectType::BUTCHERS]->nameInSavefile] = MapObjectType::BUTCHERS;

    buildingConfigs[MapObjectType::TOOLSMITHS] = new BuildingConfig();
    buildingConfigs[MapObjectType::TOOLSMITHS]->name = "Werkzeugschmiede";
    buildingConfigs[MapObjectType::TOOLSMITHS]->nameInSavefile = "toolsmiths";
    buildingConfigs[MapObjectType::TOOLSMITHS]->structurePlacing = StructurePlacing::INDIVIDUALLY;
    buildingConfigs[MapObjectType::TOOLSMITHS]->catchmentArea = new RectangleData<char>(32, 32);
    memcpy(buildingConfigs[MapObjectType::TOOLSMITHS]->catchmentArea->data, "0000000000001111111100000000000000000000011111111111111000000000000000011111111111111111100000000000001111111111111111111100000000000111111111111111111111100000000011111111111111111111111100000001111111111111111111111111100000111111111111111111111111111100001111111111111111111111111111000111111111111111111111111111111001111111111111111111111111111110011111111111111111111111111111101111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111011111111111111111111111111111100111111111111111111111111111111001111111111111111111111111111110001111111111111111111111111111000011111111111111111111111111110000011111111111111111111111111000000011111111111111111111111100000000011111111111111111111110000000000011111111111111111111000000000000011111111111111111100000000000000001111111111111100000000000000000000011111111000000000000", 1024);
    buildingConfigs[MapObjectType::TOOLSMITHS]->buildingCosts = { 150, 3, 2, 5 };
    buildingConfigs[MapObjectType::TOOLSMITHS]->buildingProduction = {
        GoodsSlot(GoodsType::TOOLS, 5),
        GoodsSlot(),
        GoodsSlot()
    };
    buildingConfigs[MapObjectType::TOOLSMITHS]->productionRate = 2.5;
    buildingConfigs[MapObjectType::TOOLSMITHS]->inhabitants = 3;
    mapMapObjectNameInSavefile[buildingConfigs[MapObjectType::TOOLSMITHS]->nameInSavefile] = MapObjectType::TOOLSMITHS;

    buildingConfigs[MapObjectType::HUNTERS_HUT] = new BuildingConfig();
    buildingConfigs[MapObjectType::HUNTERS_HUT]->name = "Jagdhütte";
    buildingConfigs[MapObjectType::HUNTERS_HUT]->nameInSavefile = "hunters_hut";
    buildingConfigs[MapObjectType::HUNTERS_HUT]->structurePlacing = StructurePlacing::INDIVIDUALLY;
    buildingConfigs[MapObjectType::HUNTERS_HUT]->catchmentArea = new RectangleData<char>(17, 17);
    memcpy(buildingConfigs[MapObjectType::HUNTERS_HUT]->catchmentArea->data, "0000001111100000000001111111110000000111111111110000011111111111110001111111111111110011111111111111101111111111111111111111111111111111111111111111111111111111111111111111111111111111111011111111111111100111111111111111000111111111111100000111111111110000000111111111000000000011111000000", 289);
    buildingConfigs[MapObjectType::HUNTERS_HUT]->buildingCosts = { 50, 2, 2, 0 };
    buildingConfigs[MapObjectType::HUNTERS_HUT]->buildingProduction = {
        GoodsSlot(GoodsType::FOOD, 3),
        GoodsSlot(),
        GoodsSlot()
    };
    buildingConfigs[MapObjectType::HUNTERS_HUT]->productionRate = 2;
    buildingConfigs[MapObjectType::HUNTERS_HUT]->inhabitants = 1;
    mapMapObjectNameInSavefile[buildingConfigs[MapObjectType::HUNTERS_HUT]->nameInSavefile] = MapObjectType::HUNTERS_HUT;
    
    buildingConfigs[MapObjectType::CATHEDRAL] = new BuildingConfig();
    buildingConfigs[MapObjectType::CATHEDRAL]->name = "Kathedrale";
    buildingConfigs[MapObjectType::CATHEDRAL]->nameInSavefile = "cathedral";
    buildingConfigs[MapObjectType::CATHEDRAL]->structurePlacing = StructurePlacing::INDIVIDUALLY;
    buildingConfigs[MapObjectType::CATHEDRAL]->catchmentArea = new RectangleData<char>(50, 50);
    memcpy(buildingConfigs[MapObjectType::CATHEDRAL]->catchmentArea->data, "0000000000000000000011111111110000000000000000000000000000000000001111111111111111110000000000000000000000000000001111111111111111111111000000000000000000000000001111111111111111111111111100000000000000000000000111111111111111111111111111100000000000000000000111111111111111111111111111111110000000000000000011111111111111111111111111111111110000000000000001111111111111111111111111111111111110000000000000111111111111111111111111111111111111110000000000011111111111111111111111111111111111111110000000000111111111111111111111111111111111111111100000000011111111111111111111111111111111111111111100000001111111111111111111111111111111111111111111100000011111111111111111111111111111111111111111111000001111111111111111111111111111111111111111111111000011111111111111111111111111111111111111111111110001111111111111111111111111111111111111111111111110011111111111111111111111111111111111111111111111100111111111111111111111111111111111111111111111111001111111111111111111111111111111111111111111111110111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111110111111111111111111111111111111111111111111111111001111111111111111111111111111111111111111111111110011111111111111111111111111111111111111111111111100111111111111111111111111111111111111111111111111000111111111111111111111111111111111111111111111100001111111111111111111111111111111111111111111111000001111111111111111111111111111111111111111111100000011111111111111111111111111111111111111111111000000011111111111111111111111111111111111111111100000000011111111111111111111111111111111111111110000000000111111111111111111111111111111111111111100000000000111111111111111111111111111111111111110000000000000111111111111111111111111111111111111000000000000000111111111111111111111111111111111100000000000000000111111111111111111111111111111110000000000000000000011111111111111111111111111110000000000000000000000011111111111111111111111111000000000000000000000000001111111111111111111111000000000000000000000000000000111111111111111111000000000000000000000000000000000000111111111100000000000000000000", 2500);
    buildingConfigs[MapObjectType::CATHEDRAL]->buildingCosts = { 7500, 25, 25, 70 };
    buildingConfigs[MapObjectType::CATHEDRAL]->buildingProduction = { GoodsSlot(), GoodsSlot(), GoodsSlot() };
    buildingConfigs[MapObjectType::CATHEDRAL]->inhabitants = 5;
    mapMapObjectNameInSavefile[buildingConfigs[MapObjectType::CATHEDRAL]->nameInSavefile] = MapObjectType::CATHEDRAL;

    buildingConfigs[MapObjectType::TAVERN] = new BuildingConfig();
    buildingConfigs[MapObjectType::TAVERN]->name = "Wirtshaus";
    buildingConfigs[MapObjectType::TAVERN]->nameInSavefile = "tavern";
    buildingConfigs[MapObjectType::TAVERN]->structurePlacing = StructurePlacing::INDIVIDUALLY;
    buildingConfigs[MapObjectType::TAVERN]->catchmentArea = new RectangleData<char>(26, 25);
    memcpy(buildingConfigs[MapObjectType::TAVERN]->catchmentArea->data, "00000000011111111000000000000000011111111111100000000000011111111111111110000000001111111111111111110000000111111111111111111110000011111111111111111111110000111111111111111111111100011111111111111111111111100111111111111111111111111011111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111011111111111111111111111100111111111111111111111111000111111111111111111111100001111111111111111111111000001111111111111111111100000001111111111111111110000000001111111111111111000000000000111111111111000000000000000011111111000000000", 650);
    buildingConfigs[MapObjectType::TAVERN]->buildingCosts = { 250, 3, 4, 6 };
    buildingConfigs[MapObjectType::TAVERN]->buildingProduction = { GoodsSlot(), GoodsSlot(), GoodsSlot() };
    buildingConfigs[MapObjectType::TAVERN]->inhabitants = 3;
    mapMapObjectNameInSavefile[buildingConfigs[MapObjectType::TAVERN]->nameInSavefile] = MapObjectType::TAVERN;
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
