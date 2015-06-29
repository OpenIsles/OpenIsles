#ifndef _SCREEN_COORDS_H
#define _SCREEN_COORDS_H

#include <iostream>
#include "map/coords/XYCoords.h"


/**
 * @brief Screen-Koordinaten. Das sind Pixel-Koordianten (Zoomstufe 4x), die ihren Ursprung in der
 * Map-Koordinate (0, 0) haben.
*
 * Siehe @ref coordsystems.
 */
class ScreenCoords : public XYCoords<int> {

public:
    /**
     * @brief Default-Konstruktor, der eine (0, 0)-Koordinate anlegt
     */
    ScreenCoords() : XYCoords<int>(0, 0) {
    }

    /**
     * @brief Konstruktor
     * @param x X-Screen-Koordinate
     * @param y Y-Screen-Koordinate
     */
    ScreenCoords(int x, int y) : XYCoords<int>(x, y) {
    }

    /**
     * @brief Ändert die Koordinaten.
     * @param x neue X-Screen-Koordinate
     * @param y neue Y-Screen-Koordinate
     */
    void setTo(int x, int y) {
        _x = x;
        _y = y;
    }

    /**
     * @brief Gleichheitsoperator
     * @param screenCoords anderer Operand
     * @return `true`, wenn diese Koordinaten gleich sind, sonst `false`
     */
    inline bool operator==(const ScreenCoords& screenCoords) const {
        return ((this->_x == screenCoords._x) && (this->_y == screenCoords._y));
    }

    /**
     * @brief Operator zum Addieren einer anderen Screen-Koordinaten
     * @param summand Summand
     */
    inline void operator+=(const ScreenCoords& summand) {
        _x += summand._x;
        _y += summand._y;
    }

    /**
     * @brief Operator zum Abziehen einer anderen Screen-Koordinaten
     * @param subtrahend Subtrahend
     */
    inline void operator-=(const ScreenCoords& subtrahend) {
        _x -= subtrahend._x;
        _y -= subtrahend._y;
    }
};

/**
 * @brief Ausgabe-Operator, um die Screen-Koordinaten auszugeben
 * @param outputStream Ausgabestream, auf den geschrieben wird
 * @param screenCoords Objekt, was ausgegeben werden soll
 * @return Ausgabestream, sodass das Chaining funktioniert
 */
std::ostream& operator<<(std::ostream& outputStream, const ScreenCoords& screenCoords);


#endif