#include "game/CatchmentArea.h"
#include "map/Direction.h"


bool CatchmentArea::isInsideCatchmentArea(const Building& building, const MapCoords& mapCoords) {
    const MapObjectType* mapObjectType = building.getMapObjectType();
    
    // Gebäude hat keinen Einzugsbereich?
    if (!mapObjectType->catchmentArea) {
        return false;
    }

    const RectangleData<char>& catchmentArea = *mapObjectType->catchmentArea;
    const FourthDirection& view = building.getView();
    const MapCoords& buildingMapCoords = building.getMapCoords();
    const int buildingMapWidth = building.getMapWidth();
    const int buildingMapHeight = building.getMapHeight();

    return isInsideCatchmentArea(
        catchmentArea, view, buildingMapCoords, buildingMapWidth, buildingMapHeight, mapCoords);
}

bool CatchmentArea::isInsideCatchmentArea(const MapObjectToBuild& mapObjectToBuild, const MapCoords& mapCoords) {
    const MapObjectType* mapObjectType = mapObjectToBuild.mapObjectType;

    // Gebäude hat keinen Einzugsbereich?
    if (!mapObjectType->catchmentArea) {
        return false;
    }

    const RectangleData<char>& catchmentArea = *mapObjectType->catchmentArea;
    const FourthDirection& view = mapObjectToBuild.view;
    const MapCoords& buildingMapCoords = mapObjectToBuild.mapCoords;
    const int buildingMapWidth = (view == Direction::NORTH || view == Direction::SOUTH) ?
                                 mapObjectType->mapWidth : mapObjectType->mapHeight;
    const int buildingMapHeight = (view == Direction::NORTH || view == Direction::SOUTH) ?
                                  mapObjectType->mapHeight : mapObjectType->mapWidth;

    return isInsideCatchmentArea(
        catchmentArea, view, buildingMapCoords, buildingMapWidth, buildingMapHeight, mapCoords);
}

bool CatchmentArea::isInsideCatchmentArea(
    const RectangleData<char>& catchmentArea, const FourthDirection& view,
    const MapCoords& buildingMapCoords, const int buildingMapWidth, const int buildingMapHeight,
    const MapCoords& mapCoords) {

    // Koordinaten innerhalb von mapObjectConfig.catchmentArea.data ermitteln
    int x, y;
    if (view == Direction::SOUTH) {
        x = (mapCoords.x() - buildingMapCoords.x()) + ((catchmentArea.width - buildingMapWidth) / 2);
        y = (mapCoords.y() - buildingMapCoords.y()) + ((catchmentArea.height - buildingMapHeight) / 2);
        return (catchmentArea.getData(x, y, '0') == '1'); // TODO String-'1' durch echte 1 ersetzen, wenn wir das über die Config einlesen

    } else if (view == Direction::EAST) {
        x = (mapCoords.x() - buildingMapCoords.x()) + ((catchmentArea.height - buildingMapWidth) / 2);
        y = (mapCoords.y() - buildingMapCoords.y()) + ((catchmentArea.width - buildingMapHeight) / 2);
        return (catchmentArea.getData(y, (catchmentArea.height - 1) - x, '0') == '1'); // TODO String-'1' durch echte 1 ersetzen, wenn wir das über die Config einlesen

    } else if (view == Direction::NORTH) {
        x = (mapCoords.x() - buildingMapCoords.x()) + ((catchmentArea.width - buildingMapWidth) / 2);
        y = (mapCoords.y() - buildingMapCoords.y()) + ((catchmentArea.height - buildingMapHeight) / 2);
        return (catchmentArea.getData((catchmentArea.width - 1) - x, (catchmentArea.height - 1) - y, '0') == '1'); // TODO String-'1' durch echte 1 ersetzen, wenn wir das über die Config einlesen

    } else if (view == Direction::WEST) {
        x = (mapCoords.x() - buildingMapCoords.x()) + ((catchmentArea.height - buildingMapWidth) / 2);
        y = (mapCoords.y() - buildingMapCoords.y()) + ((catchmentArea.width - buildingMapHeight) / 2);
        return (catchmentArea.getData((catchmentArea.width - 1) - y, x, '0') == '1'); // TODO String-'1' durch echte 1 ersetzen, wenn wir das über die Config einlesen
    }
    else {
        assert(false);
        return false;
    }
}

bool CatchmentArea::isInsideCatchmentArea(const Building& building, const MapObjectFixed& otherMapObject) {
    const MapCoords& otherMapObjectCoords = otherMapObject.getMapCoords();
    int otherMapObjectWidth = otherMapObject.getMapWidth();
    int otherMapObjectHeight = otherMapObject.getMapHeight();

    for (int mapY = otherMapObjectCoords.y(); mapY < otherMapObjectCoords.y() + otherMapObjectHeight; mapY++) {
        for (int mapX = otherMapObjectCoords.x(); mapX < otherMapObjectCoords.x() + otherMapObjectWidth; mapX++) {
            if (isInsideCatchmentArea(building, MapCoords(mapX, mapY))) {
                return true;
            }
        }
    }
    return false;
}