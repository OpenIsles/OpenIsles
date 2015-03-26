#ifndef _WINDOW_COORDS_H
#define _WINDOW_COORDS_H

#include "map/coords/XYCoords.h"


/**
 * @brief Fenster-Koordinaten. Das sind Pixel-Koordianten im Fenster.
 * Siehe @ref coordsystems.
 */
class WindowCoords : public XYCoords<int> {

public:
    /**
     * @brief Default-Konstruktor, der eine (0, 0)-Koordinate anlegt
     */
    WindowCoords() : XYCoords<int>(0, 0) {
    }

    /**
     * @brief Konstruktor
     * @param x X-Fenster-Koordinate
     * @param y Y-Fenster-Koordinate
     */
    WindowCoords(int x, int y) : XYCoords<int>(x, y) {
    }
};

#endif