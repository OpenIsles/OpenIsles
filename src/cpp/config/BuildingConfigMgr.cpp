#include <string.h>
#include "config/BuildingConfigMgr.h"


BuildingConfigMgr::BuildingConfigMgr() {
    configs = new BuildingConfig*[6];
    
    configs[0] = new BuildingConfig();
    configs[0]->buildingIndex = 0;
    configs[0]->name = "Kapelle";
    configs[0]->catchmentArea = new RectangleData<char>(6, 6);
    memcpy(configs[0]->catchmentArea->data, "011110111111111111111111111111011110", 36);
    
    configs[1] = new BuildingConfig();
    configs[1]->buildingIndex = 1;
    configs[1]->name = "Schmied";
    configs[1]->catchmentArea = new RectangleData<char>(4, 4);
    memcpy(configs[1]->catchmentArea->data, "1111111111111111", 16);
    
    configs[2] = new BuildingConfig();
    configs[2]->buildingIndex = 2;
    configs[2]->name = "Signalturm";
    configs[2]->catchmentArea = new RectangleData<char>(5, 5);
    memcpy(configs[2]->catchmentArea->data, "0010001110111110111000100", 25);
    
    configs[3] = new BuildingConfig();
    configs[3]->buildingIndex = 3;
    configs[3]->name = "Kräuterfeld";
    configs[3]->catchmentArea = new RectangleData<char>(11, 11);
    memcpy(configs[3]->catchmentArea->data, "0000111000000011111000011111111100111111111011111111111111111111111111111111101111111110011111111100001111100000001110000", 121);
    
    configs[4] = new BuildingConfig();
    configs[4]->buildingIndex = 4;
    configs[4]->name = "Steinbruch";
    configs[4]->catchmentArea = new RectangleData<char>(10, 6);
    memcpy(configs[4]->catchmentArea->data, "001111110001111111101111111111111111111101111111100011111100", 60);
    
    configs[5] = new BuildingConfig();
    configs[5]->buildingIndex = 5;
    configs[5]->name = "Steinbruch (gedreht)";
    configs[5]->catchmentArea = new RectangleData<char>(6, 10);
    memcpy(configs[5]->catchmentArea->data, "001100011110111111111111111111111111111111111111011110001100", 60);
}

BuildingConfigMgr::~BuildingConfigMgr() {
    for(int i = 0; i < 6; i++) {
        delete configs[i];
    }
    delete[] configs;
}