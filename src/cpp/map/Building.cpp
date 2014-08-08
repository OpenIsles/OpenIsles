#include "config/BuildingConfigMgr.h"
#include "map/Building.h"
#include "map/Map.h"
#include "map/MapUtils.h"

// Aus main.cpp importiert
extern BuildingConfigMgr* buildingConfigMgr;
extern Map* map;


void Building::onClick(int mouseXInBuilding, int mouseYInBuilding) {
	std::cout << "Klick auf Building " << std::to_string(this->object) << "@(" << std::to_string(mouseXInBuilding)
			<< ", " << std::to_string(mouseYInBuilding) << ")" << std::endl;

	map->setSelectedMapObject(this);
}

bool Building::isInsideCatchmentArea(int mapX, int mapY) {
    const BuildingConfig* buildingConfig = buildingConfigMgr->getConfig(object);
    const RectangleData<char>* catchmentAreaData = buildingConfig->GetCatchmentArea(); 
    
    // Gebäude hat keinen Einzugsbereich?
    if (catchmentAreaData == nullptr) {
        return false;
    }
    
    // Koordinaten innerhalb von buildingConfig.catchmentArea.data ermitteln
    int x = (mapX - this->mapX) + ((catchmentAreaData->width - this->mapWidth) / 2);
    int y = (mapY - this->mapY) + ((catchmentAreaData->height - this->mapHeight) / 2);
    
    // Außerhalb des definierten Rechtsecks? Sicher außerhalb Einzugsbereich
    if (x < 0 || y < 0 || x >= catchmentAreaData->width || y >= catchmentAreaData->height) {
        return false;
    }
    
    // TODO String-'1' durch echte 1 ersetzen, wenn wir das über die Config einlesen
    return (catchmentAreaData->data[y * catchmentAreaData->width + x] == '1'); 
}

bool Building::isInsideCatchmentArea(MapObject* mapObject) {
    int testX, testY, testWidth, testHeight;
    mapObject->getMapCoords(testX, testY, testWidth, testHeight);

    for (int y = testY; y < testY + testHeight; y++) {
        for (int x = testX; x < testX + testWidth; x++) {
            if (isInsideCatchmentArea(x, y)) {
                return true;
            }
        }
    }
    return false;
}