#include "config/ConfigMgr.h"
#include "game/Colony.h"
#include "game/Game.h"

// Aus main.cpp importiert
extern ConfigMgr* configMgr;
extern Game* game;


void Building::onClick(int mouseXInBuilding, int mouseYInBuilding) {
    game->getMap()->setSelectedMapObject(this);
}

bool Building::isInsideCatchmentArea(int mapX, int mapY) const {
    const BuildingConfig* buildingConfig = configMgr->getBuildingConfig(structureType);
    const RectangleData<char>* catchmentArea = buildingConfig->getCatchmentArea();
    
    // Gebäude hat keinen Einzugsbereich?
    if (catchmentArea == nullptr) {
        return false;
    }
    
    // Koordinaten innerhalb von buildingConfig.catchmentArea.data ermitteln
    int x = (mapX - this->mapX) + ((catchmentArea->width - this->mapWidth) / 2);
    int y = (mapY - this->mapY) + ((catchmentArea->height - this->mapHeight) / 2);
    
    // Außerhalb des definierten Rechtsecks? Sicher außerhalb Einzugsbereich
    if (x < 0 || y < 0 || x >= catchmentArea->width || y >= catchmentArea->height) {
        return false;
    }
    
    // TODO String-'1' durch echte 1 ersetzen, wenn wir das über die Config einlesen
    return (catchmentArea->data[y * catchmentArea->width + x] == '1');
}

bool Building::isInsideCatchmentArea(MapObject* mapObject) const {
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

void Building::addInhabitants(char amount) {
    if ((int) inhabitants + amount < 0) {
        throw new std::runtime_error("Cannot have negative inhabitants");
    }

    inhabitants += amount;

    Colony* colony = game->getColony(this);
    colony->population += amount;
}