#include "map/coords/MapCoords.h"


std::ostream& operator<< (std::ostream& outputStream, const MapCoords& mapCoords) {
    return outputStream << "(" << mapCoords.x() << ", " << mapCoords.y() << ")";
}