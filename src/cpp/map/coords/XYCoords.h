#ifndef _XY_COORDS_H
#define _XY_COORDS_H


/**
 * @brief Koordinaten-Paar (x, y). Die Koordinaten haben keine spezielle Bedeutung.
 * Es sollte stattdessen immer eine abgeleitete Klasse benutzt werden, damit klar ist, *was* für Koordinaten wir haben.
 * Diese Klasse sollte nur als Basisklasse für spezielle Koordinatenklassen dienen.
 */
template<class T>
class XYCoords {

protected:
    /**
     * @brief X-Koordinate
     */
    T _x;

    /**
     * @brief Y-Koordinate
     */
    T _y;

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
    XYCoords(T x, T y) : _x(x), _y(y) {
    }

    T x() const {
        return _x;
    }

    T y() const {
        return _y;
    }

    /**
     * @brief Addiert einen Offset auf die X-Koordinate
     * @param offset zu addierender Offset
     */
    void addX(T offset) {
        _x += offset;
    }

    /**
     * @brief Subtrahiert einen Offset von der X-Koordinate
     * @param offset zu subtrahierender Offset
     */
    void subX(T offset) {
        _x -= offset;
    }

    /**
     * @brief Addiert einen Offset auf die Y-Koordinate
     * @param offset zu addierender Offset
     */
    void addY(T offset) {
        _y += offset;
    }

    /**
     * @brief Subtrahiert einen Offset von der Y-Koordinate
     * @param offset zu subtrahierender Offset
     */
    void subY(T offset) {
        _y -= offset;
    }

};

#endif