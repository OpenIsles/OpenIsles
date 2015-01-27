#ifndef _A_STAR_H
#define _A_STAR_H

#include <list>
#include "map/coords/MapCoords.h"
#include "Context.h"

class Building;

/**
 * @brief Datenstruktur, die eine Route speichert, die der A*-Algorithmus berechnet hat
 */
typedef std::list<MapCoords> Route;


/**
 * @brief Pathfinding-Algorithmus A*
 */
class AStar : public ContextAware {

#ifdef DEBUG_A_STAR
public:
    /**
     * @brief (nur zu Debugzwecken einkompiliert) Startkoordinate für die Visualisierung der berechneten Route
     */
    static MapCoords debugAStar_source;

    /**
     * @brief (nur zu Debugzwecken einkompiliert) Zielkoordinate für die Visualisierung der berechneten Route
     */
    static MapCoords debugAStar_destination;

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

    /**
     * @brief (nur zu Debugzwecken einkompiliert) `true`, um die Route ausschließlich für Wege zu plannen, `false` um
     * auch über Gras laufen zu können
     */
    static bool debugAStar_useStreetOnly;
#endif

private:
    /**
     * @brief berechnete Route
     */
    Route* route;

public:
    /**
     * @brief Berechnet eine Route von einer Kachel der Karte zu einer anderen.
     * @param context Dependency
     * @param source Ausgangspunkt
     * @param destination Ziel
     * @param buildingToUseCatchmentArea Zeiger auf ein Gebäude. Wenn gesetzt, dürfen für die Route nur Kacheln benutzt
     *                                   werden, die sich im Einzugsbereich dieses Gebäudes befinden. Wird nullptr
     *                                   verwendet, so dürfen für die Route beliebige Felder benutzt werden.
     * @param useStreetOnly `true`, um nur Straßen für die Route zu verwenden, `false` erlaubt auch über Gras zu laufen
     */
    AStar(const Context* const context, const MapCoords& source, const MapCoords& destination,
          Building* buildingToUseCatchmentArea, bool useStreetOnly);

    /**
     * @brief Kürzt ggf. die berechnete Route, wenn an Start- und/oder Endpunkt ein Gebäude liegt und mehrere
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
     * Wurde sowieso keine Route gefunden, ist diese Methode eine No-Op.
     */
    void cutRouteInsideBuildings();

    /**
     * @brief Liefert die berechnete Route zurück.
     * @return Zeiger auf die berechnete Route oder nullptr, wenn keine Route existiert.
     */
    Route* getRoute() const {
        return route;
    }

private:
    /**
     * @brief Prüft, ob eine Kachel betreten werden darf, d.h. als Knoten für den A*-Algorithmus betrachtet wird.
     * @param mapCoords zu überprüfende Map-Koordinate (IN)
     * @param sourceBuilding Zeiger auf das Gebäude, das im Startpunkt der Route liegt
     *                       oder nullptr, wenn kein Gebäude an dieser Stelle ist (IN)
     * @param destinationBuilding Zeiger auf das Gebäude, das im Zielpunkt der Route liegt
     *                            oder nullptr, wenn kein Gebäude an dieser Stelle ist (IN)
     * @param buildingToUseCatchmentArea Zeiger auf ein Gebäude. Wenn gesetzt, dürfen für die Route nur Kacheln benutzt
     *                                   werden, die sich im Einzugsbereich dieses Gebäudes befinden. Wird nullptr
     *                                   verwendet, so dürfen für die Route beliebige Felder benutzt werden. (IN)
     * @param useStreetOnly `true`, um nur Straßen für die Route zu verwenden, `false` erlaubt auch über Gras zu laufen (IN)
     * @param insideSourceOrDestinationBuilding erhält die Info, ob sich die angefragte Kachel im Start- oder
     *                                          Zielgebäude (wenn gesetzt) befindet. (OUT)
     * @return true wenn die Kachel betreten werden darf, sonst false
     */
    bool isTileWalkable(const MapCoords& mapCoords, Building* sourceBuilding,
                        Building* destinationBuilding, Building* buildingToUseCatchmentArea,
                        bool useStreetOnly, bool& insideSourceOrDestinationBuilding) const;

};

#endif
