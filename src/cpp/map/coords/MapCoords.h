#ifndef _MAP_COORDS_H
#define _MAP_COORDS_H

#include "map/coords/XYCoords.h"


/**
 * @page coordsystems Koordinatensysteme
 *
 * Die Karte stellt sich wiefolgt dar:
 * <pre>
 *                 y=0
 *             --/  (0,0)   x=0
 *       y <--/      ---        \--
 *                --/   \--        \--> x
 *             --/         \--
 *          --/               \--(w-1,0)
 *    (0,h-1)  --\         /--
 *                --\   /--
 *                   ---
 *                (w-1,h-1)
 * </pre>
 *
 * Koordinatensysteme:
 * - Obenstehend sind die Kachel- oder Map-Koordinaten (MapCoords) dargestellt.
 * - Bildschirm-Koordinaten (screenCoords) sind durch ein Pixel-Koordinatensystem definiert. Hierbei wird die
 *   Kachel mit mapCoords (0, 0) auf screenCoords (0, 0) gelegt.
 * - Die Bildschirm-Koordinate einer Kachel bestimmt die Position des Pixels links-oben am Kachelrechteck.
 * - Beim Zoomen ändert sich nicht die Zuordnung von map- zu screenCoords. Beim Zeichnen wird verkleinert, d.h.
 *   mehr Inhalt dargestellt. Auf Zoomstufe Verkleinerung 4x z.B. liegt neben dem Pixel das screenCoords (0, 0)
 *   darstellt, die screenCoords (4, 0).
 * - Screen-Koordinaten sind immer auf den Boden bezogen. Kommt Elevation ins Spiel, müssen die y-Screen-Koordinate
 *   entsprechend angepasst werden.
 * - Wenn von Zeichen-Koordinaten (drawCoords) gesprochen wird, sind das Pixel-Koordinaten für ein bestimmtes
 *   Objekt, um es mit dem aktuellen Scrolling-Offset (Map::screenOffsetX und Map::screenOffsetY) und der
 *   aktuellen Zoom-Stufe (Map::screenZoom) zu zeichnen (= die obere linke Ecke). drawCoords berücksichtigen die
 *   Elevation. Derartige Koordinaten sind immer "fix und fertig", damit wir sofort zeichnen können.
 * - Fenster-Koordinaten sind Pixel-Koordinaten innerhalb des Anwendungsfensters. Sie entsprechen Zeichen-Koordinaten,
 *   nur ein anderer Begriff, der benutzt wird, wenn es nicht ums Zeichnen von etwas geht.
 *
 * Siehe hierzu auch @a doc/map-coords.xcf .
 */


/**
 * @brief Koordinatenpaar, das anzeigt, dass diese Map-Koordinaten keine echten Koordinaten auf der Karte sind,
 * sondern nur ein Delta, was zum Rechnen benutzt wird. Wir definieren die Arithmetik so, dass nur Rechnen mit
 * Deltas erlaubt ist. Zwei MapCoords zu addieren macht keinen Sinn.
 */
class MapCoordsDelta : public XYCoords<int> {

public:
    /**
     * @brief Default-Konstruktor, der ein Null-Delta anlegt
     */
    MapCoordsDelta() : XYCoords<int>(0, 0) {
    }

    /**
     * @brief Konstruktor
     * @param x X-Map-Koordinaten-Delta
     * @param y Y-Map-Koordinaten-Delta
     */
    MapCoordsDelta(int x, int y) : XYCoords<int>(x, y) {
    }

    /**
     * @brief Additionsoperator für zwei Deltas
     * @param mapCoordsDelta anderer Operand
     * @return Additionsergebnis
     */
    inline MapCoordsDelta operator+ (const MapCoordsDelta& mapCoordsDelta) const {
        return MapCoordsDelta(this->_x + mapCoordsDelta._x, this->_y + mapCoordsDelta._y);
    }
};



/**
 * @brief Gleitkomma-Map-Koordinaten. Siehe @ref coordsystems.
 */
class DoubleMapCoords : public XYCoords<double> {

public:
    /**
     * @brief Default-Konstruktor, der eine (0, 0)-Koordinate anlegt
     */
    DoubleMapCoords() : XYCoords<double>(0.0, 0.0) {
    }

    /**
     * @brief Konstruktor
     * @param x X-Map-Koordinate
     * @param y Y-Map-Koordinate
     */
    DoubleMapCoords(double x, double y) : XYCoords<double>(x, y) {
    }

    /**
     * @brief Ändert die Koordinaten.
     * @param mapCoords neue Koordinaten
     */
    void setMapCoords(const DoubleMapCoords& mapCoords) {
        _x = mapCoords._x;
        _y = mapCoords._y;
    }

    /**
     * @brief Gleichheitsoperator
     * @param mapCoords anderer Operand
     * @return `true`, wenn diese Koordinaten gleich sind, sonst `false`
     */
    inline bool operator== (const DoubleMapCoords& mapCoords) const {
        return ((this->_x == mapCoords._x) && (this->_y == mapCoords._y));
    }

    /**
     * @brief Ungleichheitsoperator
     * @param mapCoords anderer Operand
     * @return `true`, wenn diese Koordinaten ungleich sind, sonst `false`
     */
    inline bool operator!= (const DoubleMapCoords& mapCoords) const {
        return (!(*this == mapCoords));
    }

};



/**
 * @brief Ganzzahl-Map-Koordinaten. Siehe @ref coordsystems.
 */
class MapCoords : public XYCoords<int> {

public:
    /**
     * @brief Default-Konstruktor, der eine (0, 0)-Koordinate anlegt
     */
    MapCoords() : XYCoords<int>(0, 0) {
    }

    /**
     * @brief Konstruktor
     * @param x X-Map-Koordinate
     * @param y Y-Map-Koordinate
     */
    MapCoords(int x, int y) : XYCoords<int>(x, y) {
    }

    /**
     * @brief Gleichheitsoperator
     * @param mapCoords anderer Operand
     * @return `true`, wenn diese Koordinaten gleich sind, sonst `false`
     */
    inline bool operator== (const MapCoords& mapCoords) const {
        return ((this->_x == mapCoords._x) && (this->_y == mapCoords._y));
    }

    /**
     * @brief Ungleichheitsoperator
     * @param mapCoords anderer Operand
     * @return `true`, wenn diese Koordinaten ungleich sind, sonst `false`
     */
    inline bool operator!= (const MapCoords& mapCoords) const {
        return (!(*this == mapCoords));
    }

    /**
     * @brief Additionsoperator. Es darf nur ein Delta hinzugefügt werden
     * @param mapCoordsDelta anderer Operand
     * @return Additionsergebnis
     */
    inline MapCoords operator+ (const MapCoordsDelta& mapCoordsDelta) const {
        return MapCoords(this->_x + mapCoordsDelta.x(), this->_y + mapCoordsDelta.y());
    }

    /**
     * @brief Cast-Operator. Erlaubt das Umwandeln von int-Koordinaten in double-Koordinaten
     * @return Koordinaten als DoubleMapCoords
     */
    inline operator DoubleMapCoords() const {
        return DoubleMapCoords((double) _x, (double) _y);
    }

};


namespace std {

    /**
     * @brief Kleiner-Relation (= totale Ordnungsrelation), um MapCoords in unordered_map verwenden zu können.
     * @param a erste Koordinate
     * @param b zweite Koordinate
     * @return `true`, wenn a "kleiner" ist als b; sonst `false`
     */
    inline bool operator< (const MapCoords& a, const MapCoords& b) {
        return ((a.y() < b.y()) ||
                ((a.y() == b.y()) && (a.x() < b.x())));
    }

}

#endif