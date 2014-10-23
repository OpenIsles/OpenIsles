#ifndef _A_STAR_H
#define _A_STAR_H

#include <list>

class Building;

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
     * @brief Konstruktur, der direkt mapX/mapY-Koordinaten bekommt
     * @param mapX X-Map-Koordiante
     * @param mapY Y-Map-Koordiante
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
     * @brief (nur zu Debugzwecken einkompiliert) Wenn gesetzt, wird die Route auf den Einzugsbereichs dieses Gebäudes
     * restringiert.
     */
    static Building* debugAStar_buildingToUseCatchmentArea;

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
     * @param buildingToUseCatchmentArea Zeiger auf ein Gebäude. Wenn gesetzt, dürfen für die Route nur Kacheln benutzt
     *                                   werden, die sich im Einzugsbereich dieses Gebäudes befinden. Wird nullptr
     *                                   verwendet, so dürfen für die Route beliebige Felder benutzt werden.
     * @return Zeiger auf die berechnete Route oder nullptr, wenn keine Route existiert
     */
    static Route* findRoute(MapCoordinate source, MapCoordinate destination, Building* buildingToUseCatchmentArea);

    /**
     * @brief Kürzt ggf. eine gegebene Route, wenn an Start- und/oder Endpunkt ein Gebäude liegt und mehrere
     * Routenschritte innerhalb desselben Gebäudes liegen. Die Route wird so verändert, dass nur genau eine Kachel
     * des Gebäudes in der Route liegt.
     *
     * Beispiel:
     * <pre>
     * O------>                            Zielgebäude
     * |    mapX                         |-------------|
     * |                                 | (6)  |      |
     * | mapY                            |-------------|
     * v                                 | (5)  |      |
     *                                   |-------------|
     *                            (4)
     *
     *      Startgebäude    (3)
     *     |-------------|
     *     | (1)  |  (2) |
     *     |-------------|
     *     |      |      |
     *     |-------------|
     *
     * </pre>
     * Verwendet man die Map-Koordinaten eines Gebäudes als Start- und Endpunkte, ergibt sich obiges Beispiel.
     * Diese Methode entfernt die Knoten (1) und (6) aus der Route.
     *
     * @param route Route, die bearbeitet wird
     */
    static void cutRouteInsideBuildings(Route* route);

private:
    /**
     * @brief Prüft, ob eine Kachel betreten werden darf, d.h. als Knoten für den A*-Algorithmus betrachtet wird.
     * @param mapCoordinate zu überprüfende Map-Koordinate (IN)
     * @param sourceBuilding Zeiger auf das Gebäude, das im Startpunkt der Route liegt
     *                       oder nullptr, wenn kein Gebäude an dieser Stelle ist (IN)
     * @param destinationBuilding Zeiger auf das Gebäude, das im Zielpunkt der Route liegt
     *                            oder nullptr, wenn kein Gebäude an dieser Stelle ist (IN)
     * @param buildingToUseCatchmentArea Zeiger auf ein Gebäude. Wenn gesetzt, dürfen für die Route nur Kacheln benutzt
     *                                   werden, die sich im Einzugsbereich dieses Gebäudes befinden. Wird nullptr
     *                                   verwendet, so dürfen für die Route beliebige Felder benutzt werden. (IN)
     * @param insideSourceOrDestinationBuilding erhält die Info, ob sich die angefragte Kachel im Start- oder
     *                                          Zielgebäude (wenn gesetzt) befindet. (OUT)
     * @return true wenn die Kachel betreten werden darf, sonst false
     */
    static bool isTileWalkable(MapCoordinate mapCoordinate, Building* sourceBuilding,
                               Building* destinationBuilding, Building* buildingToUseCatchmentArea,
                               bool& insideSourceOrDestinationBuilding);

};

#endif
