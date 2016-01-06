#ifndef _A_STAR_H
#define _A_STAR_H

#include <list>
#include "global.h"
#include "map/coords/MapCoords.h"
#include "utils/CatchmentAreaIterator.h"

#ifdef DEBUG_A_STAR
#include <memory>
#endif

class Building;

/**
 * @brief Datenstruktur, die eine Route speichert, die der A*-Algorithmus berechnet hat
 */
class Route : public std::list<MapCoords> {
public:
    /**
     * @brief Ermittelt, ob überhaupt eine Route existiert.
     * @return `true`, wenn eine Route existiert; `false`, wenn keine Route existiert.
     */
    bool routeExists() const {
        return (!empty());
    }
};


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
     * @brief (nur zu Debugzwecken einkompiliert) Wenn gesetzt, wird die Route auf einen bestimmten Einzugsbereich
     * restringiert.
     */
    static std::unique_ptr<CatchmentAreaIterator> debugAStar_catchmentAreaIterator;

    /**
     * @brief (nur zu Debugzwecken einkompiliert) Route, die visualisiert werden soll
     */
    static Route debugAStar_route;

    /**
     * @brief (nur zu Debugzwecken einkompiliert) `true`, um die Route ausschließlich für Wege zu plannen, `false` um
     * auch über Gras laufen zu können
     */
    static bool debugAStar_useStreetOnly;

    /**
     * @brief (nur zu Debugzwecken einkompiliert) `true`, um eine Route mit ausschließlich rechten Winkeln zu finden
     */
    static bool debugAStar_rightAnglesOnly;
#endif

private:
    /**
     * @brief Zeiger auf einen Einzugsbereich. Wenn gesetzt, dürfen für die Route nur Kacheln benutzt werden, die sich
     * in diesem Einzugsbereich befinden. Wird `nullptr` verwendet, so dürfen für die Route beliebige Felder benutzt
     * werden.
     */
    CatchmentAreaIterator* catchmentAreaIterator;

    /**
     * @brief `true`, um die Route zu kürzen, wenn sie innerhalb von Gebäuden verläuft. Es genügt, die Route nur zum
     * ersten Feld des Gebäude zu führen.
     */
    bool cutRoute;

    /**
     * @brief `true`, um nur Straßen/Plätze für die Route zu verwenden, `false` erlaubt auch über Gras zu laufen
     */
    bool useStreetOnly;

    /**
     * @brief `true`, um ausschließlich rechte Winkel für die Route zu verwenden
     */
    bool rightAnglesOnly;

public:
    /**
     * @brief Konstruiert einen A*-Algorithmus mit bestimmten Parametern
     * @param context Dependency
     * @param catchmentAreaIterator Zeiger auf einen Einzugsbereich. Wenn gesetzt, dürfen für die Route nur Kacheln
     *                              benutzt werden, die sich in diesem Einzugsbereich befinden. Wird `nullptr`
     *                              verwendet, so dürfen für die Route beliebige Felder benutzt werden.
     * @param cutRoute `true`, um die Route zu kürzen, wenn sie innerhalb von Gebäuden verläuft
     * @param useStreetOnly `true`, um nur Straßen für die Route zu verwenden, `false` erlaubt auch über Gras zu laufen
     * @param rightAnglesOnly `true`, um ausschließlich rechte Winkel für die Route zu verwenden
     */
    AStar(const Context& context, CatchmentAreaIterator* catchmentAreaIterator,
          bool cutRoute, bool useStreetOnly, bool rightAnglesOnly);

    /**
     * @brief Berechnet eine Route.
     * @param source Ausgangspunkt
     * @param destination Ziel
     * @return berechnete Route. Die Route ist leer, wenn keine Route existiert.
     */
    Route getRoute(const MapCoords& source, const MapCoords& destination) const;

private:
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
     *
     * @param route Route, die verkürzt werden soll
     */
    void cutRouteInsideBuildings(Route& route) const;

    /**
     * @brief Prüft, ob eine Kachel betreten werden darf, d.h. als Knoten für den A*-Algorithmus betrachtet wird.
     * @param mapCoords zu überprüfende Map-Koordinate (IN)
     * @param sourceBuilding Zeiger auf das Gebäude, das im Startpunkt der Route liegt
     *                       oder nullptr, wenn kein Gebäude an dieser Stelle ist (IN)
     * @param destinationBuilding Zeiger auf das Gebäude, das im Zielpunkt der Route liegt
     *                            oder nullptr, wenn kein Gebäude an dieser Stelle ist (IN)
     * @param catchmentAreaIterator Zeiger auf einen Einzugsbereich. Wenn gesetzt, dürfen für die Route nur Kacheln
     *                              benutzt werden, die sich in diesem Einzugsbereich befinden. Wird `nullptr`
     *                              verwendet, so dürfen für die Route beliebige Felder benutzt werden.
     * @param useStreetOnly `true`, um nur Straßen/Plätze für die Route zu verwenden,
     *                      `false` erlaubt auch über Gras zu laufen (IN)
     * @param insideSourceOrDestinationBuilding erhält die Info, ob sich die angefragte Kachel im Start- oder
     *                                          Zielgebäude (wenn gesetzt) befindet. (OUT)
     * @return true wenn die Kachel betreten werden darf, sonst false
     */
    bool isTileWalkable(const MapCoords& mapCoords, Building* sourceBuilding,
                        Building* destinationBuilding, CatchmentAreaIterator* catchmentAreaIterator,
                        bool useStreetOnly, bool& insideSourceOrDestinationBuilding) const;

};

#endif
