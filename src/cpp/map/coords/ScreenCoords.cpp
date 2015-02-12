#include "map/coords/ScreenCoords.h"


std::ostream& operator<< (std::ostream& outputStream, const ScreenCoords& screenCoords) {
    return outputStream << "(" << screenCoords.x() << ", " << screenCoords.y() << ")";
}