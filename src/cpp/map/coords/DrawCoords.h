#ifndef _DRAW_COORDS_H
#define _DRAW_COORDS_H

#include "map/coords/XYCoords.h"


/**
 * @brief Draw-Koordinaten. Das sind Pixel-Koordianten, wohin etwas gezeichnet werden soll.
 * Siehe @ref coordsystems.
 */
class DrawCoords : public XYCoords<int> {

public:
    /**
     * @brief Default-Konstruktor, der eine (0, 0)-Koordinate anlegt
     */
    DrawCoords() : XYCoords<int>(0, 0) {
    }

    /**
     * @brief Konstruktor
     * @param x X-Pixel-Koordinate
     * @param y Y-Pixel-Koordinate
     */
    DrawCoords(int x, int y) : XYCoords<int>(x, y) {
    }
};

#endif