#ifndef _A_STAR_H
#define _A_STAR_H

#include <list>


/**
 * @brief Datenstruktur, die eine mapX- und mapY-Koordinate enthält
 */
typedef
struct MapCoordinate {

    /**
     * @brief Map-X-Koordinate
     */
    int mapX;

    /**
     * @brief Map-Y-Koordinate
     */
    int mapY;


    /**
     * @brief leerer Konstruktur, der eine ungültige Map-Koordinate (-1, -1) darstellt
     */
    MapCoordinate() : mapX(-1), mapY(-1) {
    }

    /**
     * @brief Konstruktur
     */
    MapCoordinate(int mapX, int mapY) : mapX(mapX), mapY(mapY) {
    }

    /**
     * @brief Gleichheitsoperator
     * @param otherOp anderer Operand
     * @return true, wenn dieses Objekt und der andere Operand in beiden Koordinaten übereinstimmen
     */
    inline bool operator == (const MapCoordinate& otherOp) const {
        return ((mapX == otherOp.mapX) && (mapY == otherOp.mapY));
    }

} MapCoordinate;


/**
 * @brief Datenstruktur, die eine Route speichert, die der A*-Algorithmus berechnet hat
 */
typedef std::list<MapCoordinate> Route;


/**
 * @brief Pathfinding-Algorithmus A*
 */
class AStar {

#ifdef DEBUG_A_STAR
public:
    /**
     * @brief (nur zu Debugzwecken einkompiliert) Startkoordinate für die Visualisierung der berechneten Route
     */
    static MapCoordinate debugAStar_source;

    /**
     * @brief (nur zu Debugzwecken einkompiliert) Zielkoordinate für die Visualisierung der berechneten Route
     */
    static MapCoordinate debugAStar_destination;

    /**
     * @brief (nur zu Debugzwecken einkompiliert) Route, die visualisiert werden soll oder nullptr, wenn noch keine
     * Route vorliegt
     */
    static Route* debugAStar_route;
#endif

public:
    /**
     * @brief Berechnet eine Route von einer Kachel der Karte zu einer anderen.
     * @param source Ausgangspunkt
     * @param destination Ziel
     * @return Zeiger auf die berechnete Route oder nullptr, wenn keine Route existiert
     */
    static Route* findRoute(MapCoordinate source, MapCoordinate destination);

};

#endif
