#include "pathfinding/AStar.h"

#ifdef DEBUG_A_STAR
MapCoordinate AStar::debugAStar_source = MapCoordinate(-1, -1);
MapCoordinate AStar::debugAStar_destination = MapCoordinate(-1, -1);
Route* AStar::debugAStar_route = nullptr;
#endif


Route* AStar::findRoute(MapCoordinate source, MapCoordinate destination) {

    Route* route = new Route();

    // TODO A* mit konkreter Map implementieren

    // fixe Route, damit wir die visuelle Ausgabe testen können
    // source = (17, 18), destination = (20, 12)
    route->push_back(MapCoordinate(17, 18));
    route->push_back(MapCoordinate(17, 17));
    route->push_back(MapCoordinate(17, 16));
    route->push_back(MapCoordinate(18, 16));
    route->push_back(MapCoordinate(19, 16));
    route->push_back(MapCoordinate(19, 15));
    route->push_back(MapCoordinate(19, 14));
    route->push_back(MapCoordinate(19, 13));
    route->push_back(MapCoordinate(20, 13));
    route->push_back(MapCoordinate(20, 12));


    return route;
}