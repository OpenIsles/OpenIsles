#ifndef _XY_COORDS_H
#define _XY_COORDS_H


/**
 * @brief Koordinaten-Paar (x, y). Die Koordinaten haben keine spezielle Bedeutung.
 * Es sollte stattdessen immer eine abgeleitete Klasse benutzt werden, damit klar ist, *was* für Koordinaten wir haben.
 * Diese Klasse sollte nur als Basisklasse für spezielle Koordinatenklassen dienen.
 */
class XYCoords {

protected:
    /**
     * @brief X-Koordinate
     */
    int _x;

    /**
     * @brief Y-Koordinate
     */
    int _y;

public:
    /**
     * @brief Default-Konstruktor, der eine Null-Koordinate anlegt
     */
    XYCoords() : XYCoords(0, 0) {
    }

    /**
     * @brief Konstruktor
     * @param x X-Map-Koordinaten-Delta
     * @param y Y-Map-Koordinaten-Delta
     */
    XYCoords(int x, int y) : _x(x), _y(y) {
    }

    int x() const {
        return _x;
    }

    int y() const {
        return _y;
    }

};

#endif