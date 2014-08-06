#include "config/BuildingConfigMgr.h"


BuildingConfigMgr::BuildingConfigMgr() {
    configs = new BuildingConfig*[6];
    
    configs[0] = new BuildingConfig();
    configs[0]->buildingIndex = 0;
    configs[0]->name = "Kapelle";
    
    configs[1] = new BuildingConfig();
    configs[1]->buildingIndex = 1;
    configs[1]->name = "Schmied";
    
    configs[2] = new BuildingConfig();
    configs[2]->buildingIndex = 2;
    configs[2]->name = "Signalturm";
    
    configs[3] = new BuildingConfig();
    configs[3]->buildingIndex = 3;
    configs[3]->name = "Kräuterfeld";
    
    configs[4] = new BuildingConfig();
    configs[4]->buildingIndex = 4;
    configs[4]->name = "Steinbruch";
    
    configs[5] = new BuildingConfig();
    configs[5]->buildingIndex = 5;
    configs[5]->name = "Steinbruch (gedreht)";
}

BuildingConfigMgr::~BuildingConfigMgr() {
    for(int i = 0; i < 6; i++) {
        delete configs[i];
    }
    delete[] configs;
}