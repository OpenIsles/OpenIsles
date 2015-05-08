#ifndef _MAP_COORDS_H
#define _MAP_COORDS_H

#include <iostream>
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
 * - Bildschirm-Koordinaten (screenCoords) sind durch ein Pixel-Koordinatensystem definiert. screenCoords (0, 0)
 *   befinden sich immer in der Bildschirmmitte. Auf screenCoords (0, 0) befindet sich außerdem das oben-mittige
 *   Pixel der Kachel Map::mapCoordsCentered,
 * - Beim Zoomen ändert sich nicht die Zuordnung von map- zu screenCoords. Beim Zeichnen wird verkleinert, d.h.
 *   mehr Inhalt dargestellt. Auf Zoomstufe Verkleinerung 4x z.B. liegt neben dem Pixel das screenCoords (0, 0)
 *   darstellt, die screenCoords (4, 0).
 * - Screen-Koordinaten sind unabhängig von der Drehung. In der Mitte ist immer (0, 0), nach unten-rechts am
 *   Bildschirm werden die Koordinaten beide positiver. Je nach Ansicht befindet sich dort aber nun andere
 *   Map-Koordinaten.
 * - Screen-Koordinaten sind immer auf den Boden bezogen. Kommt Elevation ins Spiel, müssen die y-Screen-Koordinate
 *   entsprechend angepasst werden.
 * - Wenn von Zeichen-Koordinaten (drawCoords) gesprochen wird, sind das Pixel-Koordinaten für ein bestimmtes
 *   Objekt, um es mit dem aktuellen Scrolling-Offset (via Map::mapCoordsCentered) und der
 *   aktuellen Zoom-Stufe (Map::screenZoom) zu zeichnen (= die obere linke Ecke). drawCoords berücksichtigen die
 *   Elevation. Derartige Koordinaten sind immer "fix und fertig", damit wir sofort zeichnen können.
 * - Fenster-Koordinaten sind Pixel-Koordinaten innerhalb des Anwendungsfensters. Sie entsprechen Zeichen-Koordinaten,
 *   nur ein anderer Begriff, der benutzt wird, wenn es nicht ums Zeichnen von etwas geht.
 *
 * TODO map-coords.xcf anpassen
 * Vorsicht: doc/map-coords.xcf ist veraltet, dort sind die Screen-Koordinaten noch im alten Schema eingezeichnet.
 */


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
    inline bool operator==(const DoubleMapCoords& mapCoords) const {
        return ((this->_x == mapCoords._x) && (this->_y == mapCoords._y));
    }

    /**
     * @brief Ungleichheitsoperator
     * @param mapCoords anderer Operand
     * @return `true`, wenn diese Koordinaten ungleich sind, sonst `false`
     */
    inline bool operator!=(const DoubleMapCoords& mapCoords) const {
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
    inline bool operator==(const MapCoords& mapCoords) const {
        return ((this->_x == mapCoords._x) && (this->_y == mapCoords._y));
    }

    /**
     * @brief Ungleichheitsoperator
     * @param mapCoords anderer Operand
     * @return `true`, wenn diese Koordinaten ungleich sind, sonst `false`
     */
    inline bool operator!=(const MapCoords& mapCoords) const {
        return (!(*this == mapCoords));
    }

    /**
     * @brief Cast-Operator. Erlaubt das Umwandeln von int-Koordinaten in double-Koordinaten
     * @return Koordinaten als DoubleMapCoords
     */
    inline operator DoubleMapCoords() const {
        return DoubleMapCoords((double) _x, (double) _y);
    }

};

/**
 * @brief Ausgabe-Operator, um die Map-Koordinaten auszugeben
 * @param outputStream Ausgabestream, auf den geschrieben wird
 * @param mapCoords Objekt, was ausgegeben werden soll
 * @return Ausgabestream, sodass das Chaining funktioniert
 */
std::ostream& operator<<(std::ostream& outputStream, const MapCoords& mapCoords);

/**
 * @brief Ausgabe-Operator, um die Map-Koordinaten auszugeben
 * @param outputStream Ausgabestream, auf den geschrieben wird
 * @param mapCoords Objekt, was ausgegeben werden soll
 * @return Ausgabestream, sodass das Chaining funktioniert
 */
std::ostream& operator<<(std::ostream& outputStream, const DoubleMapCoords& mapCoords);


namespace std {

    /**
     * @brief Kleiner-Relation (= totale Ordnungsrelation), um MapCoords in unordered_map verwenden zu können.
     * @param a erste Koordinate
     * @param b zweite Koordinate
     * @return `true`, wenn a "kleiner" ist als b; sonst `false`
     */
    inline bool operator<(const MapCoords& a, const MapCoords& b) {
        return ((a.y() < b.y()) ||
                ((a.y() == b.y()) && (a.x() < b.x())));
    }

    template<>
    struct hash<const MapCoords> {
        /**
         * @brief Generiert den Hashwert für eine Map-Koordinate
         * @param mapCoords Map-Koordinate
         * @return Hashwert
         */
        std::size_t operator()(const MapCoords& mapCoords) const {
            std::hash<int> intHash;
            return (3 * intHash(mapCoords.x())) ^ (5 * intHash(mapCoords.y()));
        }
    };

}

#endif