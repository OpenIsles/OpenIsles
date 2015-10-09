#include "game/CatchmentArea.h"
#include "map/MapObjectType.h"
#include "utils/CatchmentAreaIterator.h"

CatchmentAreaIterator::CatchmentAreaIterator(const Building& building, bool ignoreBuilding) :
    mapCoordsCentered(building.getMapCoords()),
    colony(building.getColony())
{
    
    const MapObjectType* mapObjectType = building.getMapObjectType();

    // Kein Einzugsbereich. Is ja easy ;-)
    if (mapObjectType->catchmentArea == nullptr) {
        return;
    }

    RectangleData<char>& catchmentArea = *building.getMapObjectType()->catchmentArea;
    int catchmentAreaRadius = std::max(catchmentArea.width, catchmentArea.height); // TODO sehr optimierungsbedürftig, dafür funktionierts erstmal in allen Ansichten

    // TODO Sehr hässlich, aber tuts erstmal sicher, ohne Gefahr.
    for (int mapY = mapCoordsCentered.y() - catchmentAreaRadius;
         mapY <= mapCoordsCentered.y() + catchmentAreaRadius; 
         mapY++) {
        
        for (int mapX = mapCoordsCentered.x() - catchmentAreaRadius;
             mapX <= mapCoordsCentered.x() + catchmentAreaRadius; 
             mapX++) {

            if (ignoreBuilding) {
                if ((mapX >= mapCoordsCentered.x()) &&
                    (mapX < mapCoordsCentered.x() + building.getMapWidth()) &&
                    (mapY >= mapCoordsCentered.y()) &&
                    (mapY < mapCoordsCentered.y() + building.getMapHeight())) {

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
