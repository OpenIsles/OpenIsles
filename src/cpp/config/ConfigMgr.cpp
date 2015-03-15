#include <cstring>
#include <iostream>
#include <stdexcept>
#include "config/ConfigMgr.h"
#include "utils/StringFormat.h"


ConfigMgr::ConfigMgr() {
    loadBuildingConfig();
    loadTilesConfig();
}

ConfigMgr::~ConfigMgr() {
    for(int i = 0; i < StructureType::MAX_STRUCTURE; i++) {
        if (buildingConfigs[i] != nullptr) {
            delete buildingConfigs[i];
        }
    }
    delete[] buildingConfigs;
}

void ConfigMgr::loadBuildingConfig() {
    buildingConfigs = new BuildingConfig*[StructureType::MAX_STRUCTURE];
    memset(buildingConfigs, 0, StructureType::MAX_STRUCTURE * sizeof(BuildingConfig*));

    for (int i = StructureType::COBBLED_STREET_STRAIGHT_0; i <= StructureType::COBBLED_STREET_CROSS; i++) {
        StructureType structureType = (StructureType) i;

        buildingConfigs[structureType] = new BuildingConfig();
        buildingConfigs[structureType]->name = "Pflasterstraße";
        buildingConfigs[structureType]->catchmentArea = nullptr;
        buildingConfigs[structureType]->buildingCosts = { 5, 0, 0, 0 };
        buildingConfigs[structureType]->buildingProduction = { GoodsSlot(), GoodsSlot(), GoodsSlot() };
    }
    buildingConfigs[StructureType::COBBLED_STREET_STRAIGHT_0]->nameInSavefile = "cobbled_street_straight_0";
    buildingConfigs[StructureType::COBBLED_STREET_STRAIGHT_90]->nameInSavefile = "cobbled_street_straight_90";
    buildingConfigs[StructureType::COBBLED_STREET_CURVE_0]->nameInSavefile = "cobbled_street_curve_0";
    buildingConfigs[StructureType::COBBLED_STREET_CURVE_90]->nameInSavefile = "cobbled_street_curve_90";
    buildingConfigs[StructureType::COBBLED_STREET_CURVE_180]->nameInSavefile = "cobbled_street_curve_180";
    buildingConfigs[StructureType::COBBLED_STREET_CURVE_270]->nameInSavefile = "cobbled_street_curve_270";
    buildingConfigs[StructureType::COBBLED_STREET_TEE_0]->nameInSavefile = "cobbled_street_tee_0";
    buildingConfigs[StructureType::COBBLED_STREET_TEE_90]->nameInSavefile = "cobbled_street_tee_90";
    buildingConfigs[StructureType::COBBLED_STREET_TEE_180]->nameInSavefile = "cobbled_street_tee_180";
    buildingConfigs[StructureType::COBBLED_STREET_TEE_270]->nameInSavefile = "cobbled_street_tee_270";
    buildingConfigs[StructureType::COBBLED_STREET_CROSS]->nameInSavefile = "cobbled_street_cross";
    for (int i = StructureType::COBBLED_STREET_STRAIGHT_0; i <= StructureType::COBBLED_STREET_CROSS; i++) {
        StructureType structureType = (StructureType) i;
        mapBuildingNameInSavefile[buildingConfigs[structureType]->nameInSavefile] = structureType;
    }

    buildingConfigs[StructureType::CHAPEL] = new BuildingConfig();
    buildingConfigs[StructureType::CHAPEL]->name = "Kapelle";
    buildingConfigs[StructureType::CHAPEL]->nameInSavefile = "chapel";
    buildingConfigs[StructureType::CHAPEL]->catchmentArea = new RectangleData<char>(18, 17);
    memcpy(buildingConfigs[StructureType::CHAPEL]->catchmentArea->data, "000000111111000000000011111111110000000111111111111000001111111111111100011111111111111110011111111111111110111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111011111111111111110011111111111111110001111111111111100000111111111111000000011111111110000000000111111000000", 306);
    buildingConfigs[StructureType::CHAPEL]->buildingCosts = { 100, 2, 5, 0 };
    buildingConfigs[StructureType::CHAPEL]->buildingProduction = { GoodsSlot(), GoodsSlot(), GoodsSlot() };
    buildingConfigs[StructureType::CHAPEL]->inhabitants = 1;
    mapBuildingNameInSavefile[buildingConfigs[StructureType::CHAPEL]->nameInSavefile] = StructureType::CHAPEL;

    for (int i = 1; i <= 5; i++) {
        StructureType structureType = (StructureType) (StructureType::PIONEERS_HOUSE1 + i - 1);

        buildingConfigs[structureType] = new BuildingConfig();
        buildingConfigs[structureType]->name = "Haus (Pioniere)";
        buildingConfigs[structureType]->nameInSavefile = std::string("pioneers_house") + toString(i);
        buildingConfigs[structureType]->catchmentArea = new RectangleData<char>(14, 14);
        memcpy(buildingConfigs[structureType]->catchmentArea->data, "0000111111000000011111111000001111111111000111111111111011111111111111111111111111111111111111111111111111111111111111111111111111111111111101111111111110001111111111000001111111100000001111110000", 196);
        buildingConfigs[structureType]->buildingCosts = { 0, 0, 3, 0 };
        buildingConfigs[structureType]->buildingProduction = { GoodsSlot(), GoodsSlot(), GoodsSlot() };
        mapBuildingNameInSavefile[buildingConfigs[structureType]->nameInSavefile] = structureType;
    }

    buildingConfigs[StructureType::STONEMASON] = new BuildingConfig();
    buildingConfigs[StructureType::STONEMASON]->name = "Steinbruch";
    buildingConfigs[StructureType::STONEMASON]->nameInSavefile = "stonemason";
    buildingConfigs[StructureType::STONEMASON]->catchmentArea = new RectangleData<char>(16, 16);
    memcpy(buildingConfigs[StructureType::STONEMASON]->catchmentArea->data, "0000011111100000000111111111100000111111111111000111111111111110011111111111111011111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111101111111111111100111111111111110001111111111110000011111111110000000011111100000", 256);
    buildingConfigs[StructureType::STONEMASON]->buildingCosts = { 100, 5, 5, 0 };
    buildingConfigs[StructureType::STONEMASON]->buildingProduction = {
        GoodsSlot(GoodsType::BRICKS, 8),
        GoodsSlot(),
        GoodsSlot()
    };
    buildingConfigs[StructureType::STONEMASON]->productionRate = 2.9;
    buildingConfigs[StructureType::STONEMASON]->inhabitants = 2;
    mapBuildingNameInSavefile[buildingConfigs[StructureType::STONEMASON]->nameInSavefile] = StructureType::STONEMASON;

    buildingConfigs[StructureType::OFFICE1] = new BuildingConfig();
    buildingConfigs[StructureType::OFFICE1]->name = "Kontor";
    buildingConfigs[StructureType::OFFICE1]->nameInSavefile = "office1";
    buildingConfigs[StructureType::OFFICE1]->catchmentArea = new RectangleData<char>(38, 38);
    memcpy(buildingConfigs[StructureType::OFFICE1]->catchmentArea->data, "0000000000000001111111100000000000000000000000000111111111111111100000000000000000000111111111111111111110000000000000000011111111111111111111110000000000000011111111111111111111111111000000000001111111111111111111111111111000000000111111111111111111111111111111000000001111111111111111111111111111110000000111111111111111111111111111111110000011111111111111111111111111111111110000111111111111111111111111111111111100011111111111111111111111111111111111100111111111111111111111111111111111111001111111111111111111111111111111111110011111111111111111111111111111111111101111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111011111111111111111111111111111111111100111111111111111111111111111111111111001111111111111111111111111111111111110011111111111111111111111111111111111100011111111111111111111111111111111110000111111111111111111111111111111111100000111111111111111111111111111111110000000111111111111111111111111111111000000001111111111111111111111111111110000000001111111111111111111111111111000000000001111111111111111111111111100000000000000111111111111111111111100000000000000000111111111111111111110000000000000000000011111111111111110000000000000000000000000011111111000000000000000", 1444);
    buildingConfigs[StructureType::OFFICE1]->buildingCosts = { 100, 3, 6, 0 };
    buildingConfigs[StructureType::OFFICE1]->buildingProduction = { GoodsSlot(), GoodsSlot(), GoodsSlot() };
    buildingConfigs[StructureType::OFFICE1]->inhabitants = 2;
    mapBuildingNameInSavefile[buildingConfigs[StructureType::OFFICE1]->nameInSavefile] = StructureType::OFFICE1;

    buildingConfigs[StructureType::MARKETPLACE] = new BuildingConfig();
    buildingConfigs[StructureType::MARKETPLACE]->name = "Marktplatz";
    buildingConfigs[StructureType::MARKETPLACE]->nameInSavefile = "marketplace";
    buildingConfigs[StructureType::MARKETPLACE]->catchmentArea = new RectangleData<char>(38, 38);
    memcpy(buildingConfigs[StructureType::MARKETPLACE]->catchmentArea->data, "0000000000000001111111100000000000000000000000000111111111111111100000000000000000000111111111111111111110000000000000000011111111111111111111110000000000000011111111111111111111111111000000000001111111111111111111111111111000000000111111111111111111111111111111000000001111111111111111111111111111110000000111111111111111111111111111111110000011111111111111111111111111111111110000111111111111111111111111111111111100011111111111111111111111111111111111100111111111111111111111111111111111111001111111111111111111111111111111111110011111111111111111111111111111111111101111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111011111111111111111111111111111111111100111111111111111111111111111111111111001111111111111111111111111111111111110011111111111111111111111111111111111100011111111111111111111111111111111110000111111111111111111111111111111111100000111111111111111111111111111111110000000111111111111111111111111111111000000001111111111111111111111111111110000000001111111111111111111111111111000000000001111111111111111111111111100000000000000111111111111111111111100000000000000000111111111111111111110000000000000000000011111111111111110000000000000000000000000011111111000000000000000", 1444);
    buildingConfigs[StructureType::MARKETPLACE]->buildingCosts = { 200, 4, 10, 0 };
    buildingConfigs[StructureType::MARKETPLACE]->buildingProduction = { GoodsSlot(), GoodsSlot(), GoodsSlot() };
    buildingConfigs[StructureType::MARKETPLACE]->inhabitants = 3;
    mapBuildingNameInSavefile[buildingConfigs[StructureType::MARKETPLACE]->nameInSavefile] = StructureType::MARKETPLACE;

    buildingConfigs[StructureType::FORESTERS] = new BuildingConfig();
    buildingConfigs[StructureType::FORESTERS]->name = "Forsthaus";
    buildingConfigs[StructureType::FORESTERS]->nameInSavefile = "foresters";
    buildingConfigs[StructureType::FORESTERS]->catchmentArea = new RectangleData<char>(8, 8);
    memcpy(buildingConfigs[StructureType::FORESTERS]->catchmentArea->data, "0011110001111110111111111111111111111111111111110111111000111100", 64);
    buildingConfigs[StructureType::FORESTERS]->buildingCosts = { 50, 2, 0, 0 };
    buildingConfigs[StructureType::FORESTERS]->buildingProduction = {
        GoodsSlot(GoodsType::WOOD, 10),
        GoodsSlot(),
        GoodsSlot()
    };
    buildingConfigs[StructureType::FORESTERS]->productionRate = 3.5;
    buildingConfigs[StructureType::FORESTERS]->inhabitants = 1;
    mapBuildingNameInSavefile[buildingConfigs[StructureType::FORESTERS]->nameInSavefile] = StructureType::FORESTERS;

    buildingConfigs[StructureType::SHEEP_FARM] = new BuildingConfig();
    buildingConfigs[StructureType::SHEEP_FARM]->name = "Schaffarm";
    buildingConfigs[StructureType::SHEEP_FARM]->nameInSavefile = "sheep_farm";
    buildingConfigs[StructureType::SHEEP_FARM]->catchmentArea = new RectangleData<char>(8, 8);
    memcpy(buildingConfigs[StructureType::SHEEP_FARM]->catchmentArea->data, "0011110001111110111111111111111111111111111111110111111000111100", 64);
    buildingConfigs[StructureType::SHEEP_FARM]->buildingCosts = { 200, 2, 4, 0 };
    buildingConfigs[StructureType::SHEEP_FARM]->buildingProduction = {
        GoodsSlot(GoodsType::WOOL, 3),
        GoodsSlot(),
        GoodsSlot()
    };
    buildingConfigs[StructureType::SHEEP_FARM]->productionRate = 30;
    buildingConfigs[StructureType::SHEEP_FARM]->inhabitants = 2;
    mapBuildingNameInSavefile[buildingConfigs[StructureType::SHEEP_FARM]->nameInSavefile] = StructureType::SHEEP_FARM;

    buildingConfigs[StructureType::WEAVING_MILL1] = new BuildingConfig();
    buildingConfigs[StructureType::WEAVING_MILL1]->name = "Webstube";
    buildingConfigs[StructureType::WEAVING_MILL1]->nameInSavefile = "weaving_mill1";
    buildingConfigs[StructureType::WEAVING_MILL1]->catchmentArea = new RectangleData<char>(32, 32);
    memcpy(buildingConfigs[StructureType::WEAVING_MILL1]->catchmentArea->data, "0000000000001111111100000000000000000000011111111111111000000000000000011111111111111111100000000000001111111111111111111100000000000111111111111111111111100000000011111111111111111111111100000001111111111111111111111111100000111111111111111111111111111100001111111111111111111111111111000111111111111111111111111111111001111111111111111111111111111110011111111111111111111111111111101111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111011111111111111111111111111111100111111111111111111111111111111001111111111111111111111111111110001111111111111111111111111111000011111111111111111111111111110000011111111111111111111111111000000011111111111111111111111100000000011111111111111111111110000000000011111111111111111111000000000000011111111111111111100000000000000001111111111111100000000000000000000011111111000000000000", 1024);
    buildingConfigs[StructureType::WEAVING_MILL1]->buildingCosts = { 200, 3, 6, 0 };
    buildingConfigs[StructureType::WEAVING_MILL1]->buildingProduction = {
        GoodsSlot(GoodsType::CLOTH, 7),
        GoodsSlot(GoodsType::WOOL, 7),
        GoodsSlot()
    };
    buildingConfigs[StructureType::WEAVING_MILL1]->productionRate = 10.0;
    buildingConfigs[StructureType::WEAVING_MILL1]->inputConsumptionRate = 5.0;
    buildingConfigs[StructureType::WEAVING_MILL1]->inhabitants = 2;
    mapBuildingNameInSavefile[buildingConfigs[StructureType::WEAVING_MILL1]->nameInSavefile] = StructureType::WEAVING_MILL1;

    buildingConfigs[StructureType::CATTLE_FARM] = new BuildingConfig();
    buildingConfigs[StructureType::CATTLE_FARM]->name = "Rinderfarm";
    buildingConfigs[StructureType::CATTLE_FARM]->nameInSavefile = "cattle_farm";
    buildingConfigs[StructureType::CATTLE_FARM]->catchmentArea = new RectangleData<char>(6, 6);
    memcpy(buildingConfigs[StructureType::CATTLE_FARM]->catchmentArea->data, "011110111111111111111111111111011110", 36);
    buildingConfigs[StructureType::CATTLE_FARM]->buildingCosts = { 100, 1, 4, 0 };
    buildingConfigs[StructureType::CATTLE_FARM]->buildingProduction = {
        GoodsSlot(GoodsType::CATTLE, 4),
        GoodsSlot(),
        GoodsSlot()
    };
    buildingConfigs[StructureType::CATTLE_FARM]->productionRate = 7.5;
    buildingConfigs[StructureType::CATTLE_FARM]->inhabitants = 2;
    mapBuildingNameInSavefile[buildingConfigs[StructureType::CATTLE_FARM]->nameInSavefile] = StructureType::CATTLE_FARM;

    buildingConfigs[StructureType::BUTCHERS] = new BuildingConfig();
    buildingConfigs[StructureType::BUTCHERS]->name = "Fleischerei";
    buildingConfigs[StructureType::BUTCHERS]->nameInSavefile = "butchers";
    buildingConfigs[StructureType::BUTCHERS]->catchmentArea = new RectangleData<char>(22, 22);
    memcpy(buildingConfigs[StructureType::BUTCHERS]->catchmentArea->data, "0000000111111110000000000001111111111110000000001111111111111100000001111111111111111000001111111111111111110001111111111111111111100111111111111111111110111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111110111111111111111111110011111111111111111111000111111111111111111000001111111111111111000000011111111111111000000000111111111111000000000000111111110000000", 484);
    buildingConfigs[StructureType::BUTCHERS]->buildingCosts = { 150, 3, 6, 0 };
    buildingConfigs[StructureType::BUTCHERS]->buildingProduction = {
        GoodsSlot(GoodsType::FOOD, 4),
        GoodsSlot(GoodsType::CATTLE, 4),
        GoodsSlot()
    };
    buildingConfigs[StructureType::BUTCHERS]->productionRate = 5;
    buildingConfigs[StructureType::BUTCHERS]->inputConsumptionRate = 10;
    buildingConfigs[StructureType::BUTCHERS]->inhabitants = 2;
    mapBuildingNameInSavefile[buildingConfigs[StructureType::BUTCHERS]->nameInSavefile] = StructureType::BUTCHERS;

    buildingConfigs[StructureType::TOOLSMITHS] = new BuildingConfig();
    buildingConfigs[StructureType::TOOLSMITHS]->name = "Werkzeugschmiede";
    buildingConfigs[StructureType::TOOLSMITHS]->nameInSavefile = "toolsmiths";
    buildingConfigs[StructureType::TOOLSMITHS]->catchmentArea = new RectangleData<char>(32, 32);
    memcpy(buildingConfigs[StructureType::TOOLSMITHS]->catchmentArea->data, "0000000000001111111100000000000000000000011111111111111000000000000000011111111111111111100000000000001111111111111111111100000000000111111111111111111111100000000011111111111111111111111100000001111111111111111111111111100000111111111111111111111111111100001111111111111111111111111111000111111111111111111111111111111001111111111111111111111111111110011111111111111111111111111111101111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111011111111111111111111111111111100111111111111111111111111111111001111111111111111111111111111110001111111111111111111111111111000011111111111111111111111111110000011111111111111111111111111000000011111111111111111111111100000000011111111111111111111110000000000011111111111111111111000000000000011111111111111111100000000000000001111111111111100000000000000000000011111111000000000000", 1024);
    buildingConfigs[StructureType::TOOLSMITHS]->buildingCosts = { 150, 3, 2, 5 };
    buildingConfigs[StructureType::TOOLSMITHS]->buildingProduction = {
        GoodsSlot(GoodsType::TOOLS, 5),
        GoodsSlot(),
        GoodsSlot()
    };
    buildingConfigs[StructureType::TOOLSMITHS]->productionRate = 2.5;
    buildingConfigs[StructureType::TOOLSMITHS]->inhabitants = 3;
    mapBuildingNameInSavefile[buildingConfigs[StructureType::TOOLSMITHS]->nameInSavefile] = StructureType::TOOLSMITHS;

    buildingConfigs[StructureType::HUNTERS_HUT] = new BuildingConfig();
    buildingConfigs[StructureType::HUNTERS_HUT]->name = "Jagdhütte";
    buildingConfigs[StructureType::HUNTERS_HUT]->nameInSavefile = "hunters_hut";
    buildingConfigs[StructureType::HUNTERS_HUT]->catchmentArea = new RectangleData<char>(17, 17);
    memcpy(buildingConfigs[StructureType::HUNTERS_HUT]->catchmentArea->data, "0000001111100000000001111111110000000111111111110000011111111111110001111111111111110011111111111111101111111111111111111111111111111111111111111111111111111111111111111111111111111111111011111111111111100111111111111111000111111111111100000111111111110000000111111111000000000011111000000", 289);
    buildingConfigs[StructureType::HUNTERS_HUT]->buildingCosts = { 50, 2, 2, 0 };
    buildingConfigs[StructureType::HUNTERS_HUT]->buildingProduction = {
        GoodsSlot(GoodsType::FOOD, 3),
        GoodsSlot(),
        GoodsSlot()
    };
    buildingConfigs[StructureType::HUNTERS_HUT]->productionRate = 2;
    buildingConfigs[StructureType::HUNTERS_HUT]->inhabitants = 1;
    mapBuildingNameInSavefile[buildingConfigs[StructureType::HUNTERS_HUT]->nameInSavefile] = StructureType::HUNTERS_HUT;
    
    buildingConfigs[StructureType::CATHEDRAL] = new BuildingConfig();
    buildingConfigs[StructureType::CATHEDRAL]->name = "Kathedrale";
    buildingConfigs[StructureType::CATHEDRAL]->nameInSavefile = "cathedral";
    buildingConfigs[StructureType::CATHEDRAL]->catchmentArea = new RectangleData<char>(50, 50);
    memcpy(buildingConfigs[StructureType::CATHEDRAL]->catchmentArea->data, "0000000000000000000011111111110000000000000000000000000000000000001111111111111111110000000000000000000000000000001111111111111111111111000000000000000000000000001111111111111111111111111100000000000000000000000111111111111111111111111111100000000000000000000111111111111111111111111111111110000000000000000011111111111111111111111111111111110000000000000001111111111111111111111111111111111110000000000000111111111111111111111111111111111111110000000000011111111111111111111111111111111111111110000000000111111111111111111111111111111111111111100000000011111111111111111111111111111111111111111100000001111111111111111111111111111111111111111111100000011111111111111111111111111111111111111111111000001111111111111111111111111111111111111111111111000011111111111111111111111111111111111111111111110001111111111111111111111111111111111111111111111110011111111111111111111111111111111111111111111111100111111111111111111111111111111111111111111111111001111111111111111111111111111111111111111111111110111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111110111111111111111111111111111111111111111111111111001111111111111111111111111111111111111111111111110011111111111111111111111111111111111111111111111100111111111111111111111111111111111111111111111111000111111111111111111111111111111111111111111111100001111111111111111111111111111111111111111111111000001111111111111111111111111111111111111111111100000011111111111111111111111111111111111111111111000000011111111111111111111111111111111111111111100000000011111111111111111111111111111111111111110000000000111111111111111111111111111111111111111100000000000111111111111111111111111111111111111110000000000000111111111111111111111111111111111111000000000000000111111111111111111111111111111111100000000000000000111111111111111111111111111111110000000000000000000011111111111111111111111111110000000000000000000000011111111111111111111111111000000000000000000000000001111111111111111111111000000000000000000000000000000111111111111111111000000000000000000000000000000000000111111111100000000000000000000", 2500);
    buildingConfigs[StructureType::CATHEDRAL]->buildingCosts = { 7500, 25, 25, 70 };
    buildingConfigs[StructureType::CATHEDRAL]->buildingProduction = { GoodsSlot(), GoodsSlot(), GoodsSlot() };
    buildingConfigs[StructureType::CATHEDRAL]->inhabitants = 5;
    mapBuildingNameInSavefile[buildingConfigs[StructureType::CATHEDRAL]->nameInSavefile] = StructureType::CATHEDRAL;

    buildingConfigs[StructureType::TAVERN] = new BuildingConfig();
    buildingConfigs[StructureType::TAVERN]->name = "Wirtshaus";
    buildingConfigs[StructureType::TAVERN]->nameInSavefile = "tavern";
    buildingConfigs[StructureType::TAVERN]->catchmentArea = new RectangleData<char>(26, 25);
    memcpy(buildingConfigs[StructureType::TAVERN]->catchmentArea->data, "00000000011111111000000000000000011111111111100000000000011111111111111110000000001111111111111111110000000111111111111111111110000011111111111111111111110000111111111111111111111100011111111111111111111111100111111111111111111111111011111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111011111111111111111111111100111111111111111111111111000111111111111111111111100001111111111111111111111000001111111111111111111100000001111111111111111110000000001111111111111111000000000000111111111111000000000000000011111111000000000", 650);
    buildingConfigs[StructureType::TAVERN]->buildingCosts = { 250, 3, 4, 6 };
    buildingConfigs[StructureType::TAVERN]->buildingProduction = { GoodsSlot(), GoodsSlot(), GoodsSlot() };
    buildingConfigs[StructureType::TAVERN]->inhabitants = 3;
    mapBuildingNameInSavefile[buildingConfigs[StructureType::TAVERN]->nameInSavefile] = StructureType::TAVERN;
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
