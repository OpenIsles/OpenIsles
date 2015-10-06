#include "game/CatchmentArea.h"
#include "map/MapObjectType.h"
#include "utils/CatchmentAreaIterator.h"

CatchmentAreaIterator::CatchmentAreaIterator(const Building& building, bool ignoreBuilding) {
    const MapObjectType* mapObjectType = building.getMapObjectType();

    // Kein Einzugsbereich. Is ja easy ;-)
    if (mapObjectType->catchmentArea == nullptr) {
        return;
    }

    RectangleData<char>& catchmentArea = *building.getMapObjectType()->catchmentArea;
    int catchmentAreaRadius = std::max(catchmentArea.width, catchmentArea.height); // TODO sehr optimierungsbedürftig, dafür funktionierts erstmal in allen Ansichten

    // TODO Sehr hässlich, aber tuts erstmal sicher, ohne Gefahr.
    const MapCoords& mapCoords = building.getMapCoords();
    for (int mapY = mapCoords.y() - catchmentAreaRadius; mapY <= mapCoords.y() + catchmentAreaRadius; mapY++) {
        for (int mapX = mapCoords.x() - catchmentAreaRadius; mapX <= mapCoords.x() + catchmentAreaRadius; mapX++) {

            if (ignoreBuilding) {
                if ((mapX >= building.getMapCoords().x()) &&
                    (mapX < building.getMapCoords().x() + building.getMapWidth()) &&
                    (mapY >= building.getMapCoords().y()) &&
                    (mapY < building.getMapCoords().y() + building.getMapHeight())) {

                    continue;
                }
            }

            MapCoords mapCoordsHere = {mapX, mapY};
            if (CatchmentArea::isInsideCatchmentArea(building, mapCoordsHere)) {
                mapCoordsInCatchmentArea.push_back(mapCoordsHere);
            }
        }
    }
}
