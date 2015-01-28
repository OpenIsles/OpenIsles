#include "config/ConfigMgr.h"
#include "game/Game.h"


bool Building::isInsideCatchmentArea(const ConfigMgr* configMgr, const MapCoords& mapCoords) const {
    const BuildingConfig* buildingConfig = configMgr->getBuildingConfig(structureType);
    const RectangleData<char>* catchmentArea = buildingConfig->getCatchmentArea();
    
    // Gebäude hat keinen Einzugsbereich?
    if (catchmentArea == nullptr) {
        return false;
    }
    
    // Koordinaten innerhalb von buildingConfig.catchmentArea.data ermitteln
    int x = (mapCoords.x() - this->mapCoords.x()) + ((catchmentArea->width - this->mapWidth) / 2);
    int y = (mapCoords.y() - this->mapCoords.y()) + ((catchmentArea->height - this->mapHeight) / 2);
    
    // Außerhalb des definierten Rechtsecks? Sicher außerhalb Einzugsbereich
    if (x < 0 || y < 0 || x >= catchmentArea->width || y >= catchmentArea->height) {
        return false;
    }
    
    // TODO String-'1' durch echte 1 ersetzen, wenn wir das über die Config einlesen
    return (catchmentArea->data[y * catchmentArea->width + x] == '1');
}

bool Building::isInsideCatchmentArea(const ConfigMgr* configMgr, const MapObjectFixed& otherMapObject) const {
    const MapCoords& otherMapObjectCoords = otherMapObject.getMapCoords();
    int otherMapObjectWidth = otherMapObject.getMapWidth();
    int otherMapObjectHeight = otherMapObject.getMapHeight();

    for (int mapY = otherMapObjectCoords.y(); mapY < otherMapObjectCoords.y() + otherMapObjectHeight; mapY++) {
        for (int mapX = otherMapObjectCoords.x(); mapX < otherMapObjectCoords.x() + otherMapObjectWidth; mapX++) {
            if (isInsideCatchmentArea(configMgr, MapCoords(mapX, mapY))) {
                return true;
            }
        }
    }
    return false;
}