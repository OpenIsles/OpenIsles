#include "map/coords/MapCoords.h"


std::ostream& operator<< (std::ostream& outputStream, const MapCoords& mapCoords) {
    return outputStream << "(" << mapCoords.x() << ", " << mapCoords.y() << ")";
}

std::ostream& operator<< (std::ostream& outputStream, const DoubleMapCoords& mapCoords) {
    return outputStream << "(" << mapCoords.x() << ", " << mapCoords.y() << ")";
}