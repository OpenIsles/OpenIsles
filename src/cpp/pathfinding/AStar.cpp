#include <cassert>
#include <list>
#include <map>
#include <set>
#include "config/ConfigMgr.h"
#include "game/Game.h"
#include "map/Map.h"
#include "pathfinding/AStar.h"

#ifdef DEBUG_A_STAR
MapCoords AStar::debugAStar_source = MapCoords(-1, -1);
MapCoords AStar::debugAStar_destination = MapCoords(-1, -1);
Building* AStar::debugAStar_buildingToUseCatchmentArea = nullptr;
Route* AStar::debugAStar_route = nullptr;
bool AStar::debugAStar_useStreetOnly = false;
#endif


AStar::AStar(const Context* const context, const MapCoords& source, const MapCoords& destination,
             Building* buildingToUseCatchmentArea, bool useStreetOnly) : ContextAware(context) {

    Map* map = context->game->getMap();

    // Ermitteln, aus und in welches Gebäude wir die Route berechnen wollen. Dies ist optional, eine Route muss nicht
    // notwendigerweise ein Gebäude am Anfang/Ende haben. In diesem Fall ist der Building-Zeiger nullptr.
    Building* sourceBuilding = dynamic_cast<Building*>(map->getMapTileAt(source)->mapObject);
    Building* destinationBuilding = dynamic_cast<Building*>(map->getMapTileAt(destination)->mapObject);


    /********************* A*-Implementierung. Siehe https://de.wikipedia.org/wiki/A*-Algorithmus *********************/

    // Datenstruktur für den Algorithmus
    struct AStarNode {
        MapCoords mapCoords;           // Koordinaten des Knoten
        double f;                      // f-Wert für den Algorithmus = geschätzte Kosten von diesem Knoten zum Ziel
    };

    // Open-List: Zu untersuchende Knoten priorisiert. Der Knoten mit niedrigstem f-Wert steht "vorne".
    std::list<AStarNode> openList;

    // Closed-List: Abschließend untersuchte Knoten
    std::set<MapCoords> closedList;

    // g-Werte für den Algorithmus. Sie enthalten die gesicherten Kosten, um einen bestimmten Knoten zu erreichen
    std::map<MapCoords, double> g;

    // Map, die jedem Knoten einen Vorgängerknoten im Pfad zuweist. Wir rekonstruieren damit am Ende die Route.
    std::map<MapCoords, MapCoords> predecessorInPath;



    // Initialisierung: Es geht mit dem Startknoten los
    openList.push_back({ source, 0 });

    // Schleife: Machen, bis wir entweder am Ziel sind (= vorzeitiger Abbruch)
    //           oder keine Lösung existiert (= die openList leer is)
    do {
        // vielversprechensten Knoten aus der openList nehmen, d.h. den mit minimalsten f-Wert
        AStarNode currentNode = openList.front();
        openList.pop_front();

        // Ziel erreicht? Dann fertig :-)
        if (currentNode.mapCoords == destination) {
            break;
        }

        // Knoten abhaken, d.h. zur closedList hinzufügen
        closedList.insert(currentNode.mapCoords);

        // Nachfolgeknoten auf die openList setzen. Das sind alle Kacheln um die aktuelle Kachel herum
        for (int mapXOffset = -1; mapXOffset <= 1; mapXOffset++) {
            for (int mapYOffset = -1; mapYOffset <= 1; mapYOffset++) {
                if (mapXOffset == 0 && mapYOffset == 0) {
                    continue;
                }

                MapCoords successorMapCoordinate(
                    currentNode.mapCoords.x() + mapXOffset, currentNode.mapCoords.y() + mapYOffset);

                // Prüfen, ob diese Koordinate überhaupt betreten werden darf. Steht da ein Gebäude oder ist sie
                // außerhalb des zulässigen Einzugsbereichs (TODO), stellt sie keinen Knoten im Algorithmus dar und
                // wird einfach ignoriert.
                bool insideSourceOrDestinationBuilding;
                if (!isTileWalkable(successorMapCoordinate, sourceBuilding,
                                    destinationBuilding, buildingToUseCatchmentArea,
                                    useStreetOnly, insideSourceOrDestinationBuilding)) {
                    continue;
                }

                // Wenn wir quer über die Kacheln laufen, d.h. mapX und mapY gleichzeitig ändern, müssen wir prüfen,
                // ob die beiden angrenzenden Kacheln auch frei sind. Ist mindestens eine belegt, ist diese Abkürzung
                // nicht möglich.
                bool walkCrossTiles = (abs(mapXOffset) + abs(mapYOffset) == 2);
                if (walkCrossTiles) {
                    bool unusedVar; // Dummy-Variable für insideSourceOrDestinationBuilding

                    MapCoords mapCoordinateNextTo1(
                        currentNode.mapCoords.x(), currentNode.mapCoords.y() + mapYOffset);
                    if (!isTileWalkable(mapCoordinateNextTo1, sourceBuilding, destinationBuilding,
                                        buildingToUseCatchmentArea, useStreetOnly, unusedVar)) {
                        continue;
                    }

                    MapCoords mapCoordinateNextTo2(
                        currentNode.mapCoords.x() + mapXOffset, currentNode.mapCoords.y());
                    if (!isTileWalkable(mapCoordinateNextTo2, sourceBuilding, destinationBuilding,
                                        buildingToUseCatchmentArea, useStreetOnly, unusedVar)) {
                        continue;
                    }
                }

                // Wenn Knoten auf closedList ist, ignorieren, mit dem sind wir schon fertig.
                if (closedList.find(successorMapCoordinate) != closedList.cend()) {
                    continue;
                }

                /*
                 * Das Kantengewicht c berechnet sich wie folgt:
                 *
                 *   c = 0: Innerhalb von Start- und Zielgebäude können wir uns mit Kosten 0 bewegen, da es egal ist,
                 *          an welcher Stelle wir ein Gebäude betreten und verlassen.
                 *
                 *   c = 1: Bewegen wir uns normal, ist das Kantengewicht immer 1, da wir alle anliegenden Felder
                 *          gleich entfernt betrachten.
                 *
                 * c = 1.5: Für Querlaufen über die Kacheln haben wir ein Kantengewicht von 1.5, d.h. teurer als
                 *          eine normale Bewegung, aber günstiger als zwei normale Kacheln.
                 *          Dies ist wichtig, sonst würden wir statt einer geraden Linie zwischendrin quer laufen,
                 *          nur weil es Richtung Ziel geht (siehe doc/pathfinding-cross-tiles.png für diesen Fall).
                 */
                double c = (!insideSourceOrDestinationBuilding) ? (walkCrossTiles ? 1.5 : 1) : 0;

                // g-Wert für den neuen Weg = g(Vorgängerknoten) + c(Vorgängerknoten, aktuellem Knoten)
                double tentativeG = g[currentNode.mapCoords] + c;

                // Haben wir schon einen Weg zu successorMapCoordinate? Ist der Weg besser als der grade gefundene,
                // dann nix zu tun.
                std::list<AStarNode>::iterator nodeAlreadyInOpenListIter = openList.end();
                for (std::list<AStarNode>::iterator iter = openList.begin(); iter != openList.end(); iter++) {
                    if ((*iter).mapCoords == successorMapCoordinate) {
                        nodeAlreadyInOpenListIter = iter;
                        break;
                    }
                }

                if (nodeAlreadyInOpenListIter != openList.cend() && tentativeG >= g[successorMapCoordinate]) {
                    continue;
                }

                // g-Wert merken
                g[successorMapCoordinate] = tentativeG;

                // f(successor) = tentativeG + h(successor). h = Manhattan-Distanz zum Ziel
                double h = abs(destination.x() - successorMapCoordinate.x()) +
                           abs(destination.y() - successorMapCoordinate.y());
                double f = tentativeG + h;

                // Zeiger zum Vorgängerknoten setzen
                predecessorInPath[successorMapCoordinate] = currentNode.mapCoords;

                // Wenn schon in der Liste: rausnehmen, um an die richtige Stelle einzufügen
                if (nodeAlreadyInOpenListIter != openList.cend()) {
                    openList.erase(nodeAlreadyInOpenListIter);
                }

                // An die richtige Stelle in der OpenList einfügen
                AStarNode successorNode = { successorMapCoordinate, f };

                std::list<AStarNode>::iterator positionToInsert = openList.end();
                for (std::list<AStarNode>::iterator iter = openList.begin(); iter != openList.end(); iter++) {
                    if ((*iter).f > successorNode.f) {
                        positionToInsert = iter;
                        break;
                    }
                }
                openList.insert(positionToInsert, successorNode);
            }
        }
    } while (!openList.empty());



    // openList ist leer, d.h. es existiert keine Route
    if (openList.empty()) {
        route = nullptr;
        return;
    }

    // Ergebnisroute zusammenbauen, indem die Knoten zurückverfolgt werden. Wir bauen die Route damit von hinten her auf.
    MapCoords currentMapCoord = destination;

    route = new Route();
    for(;;) {
        route->push_front(currentMapCoord);

        auto iter = predecessorInPath.find(currentMapCoord);
        if (iter == predecessorInPath.end()) {
            break; // existiert kein Eintrag, haben wir die Route komplett
        }

        currentMapCoord = (*iter).second;
    }
}

void AStar::cutRouteInsideBuildings() {
    if (route == nullptr) {
        return;
    }

    Map* map = context->game->getMap();

    // Erst von vorne
    int hopsToDeleteFromFront = 0;
    Building* buildingAtFront = nullptr;
    for (auto iter = route->cbegin(); iter != route->cend(); iter++) {
        MapCoords mapCoords = *iter;
        MapTile* mapTile = map->getMapTileAt(mapCoords);
        assert(mapTile != nullptr); // Route hätte nicht berechnet werden können, wenn außerhalb der Karte

        Building* buildingThere = dynamic_cast<Building*>(mapTile->mapObject);
        if (buildingAtFront == nullptr) { // Erster Routenknoten...
            if (buildingThere == nullptr) {
                break; // ... kein Gebäude, nix zu tun
            } else {
                buildingAtFront = buildingThere; // ... Gebäude merken
            }
        } else { // Weiterer Routenknoten...
            if (buildingAtFront == buildingThere) {
                hopsToDeleteFromFront++;
            } else {
                break; // andere Gebäude hier, fertig.
            }
        }
    }

    // Dann von hinten
    int hopsToDeleteFromBack = 0;
    Building* buildingAtBack = nullptr;
    for (auto iter = route->crbegin(); iter != route->crend(); iter++) {
        MapCoords mapCoords = *iter;
        MapTile* mapTile = map->getMapTileAt(mapCoords);
        assert(mapTile != nullptr); // Route hätte nicht berechnet werden können, wenn außerhalb der Karte

        Building* buildingThere = dynamic_cast<Building*>(mapTile->mapObject);
        if (buildingAtBack == nullptr) { // Erster Routenknoten...
            if (buildingThere == nullptr) {
                break; // ... kein Gebäude, nix zu tun
            } else {
                buildingAtBack = buildingThere; // ... Gebäude merken
            }
        } else { // Weiterer Routenknoten...
            if (buildingAtBack == buildingThere) {
                hopsToDeleteFromBack++;
            } else {
                break; // andere Gebäude hier, fertig.
            }
        }
    }

    assert(buildingAtFront == nullptr || buildingAtBack == nullptr ||
           (buildingAtFront != buildingAtBack)); // Start- und Endgebäude dürfen nicht gleich sein

    // Route entsprechend beschneiden
    while (hopsToDeleteFromFront-- > 0) {
        route->pop_front();
    }
    while (hopsToDeleteFromBack-- > 0) {
        route->pop_back();
    }
}

bool AStar::isTileWalkable(const MapCoords& mapCoords, Building* sourceBuilding,
                           Building* destinationBuilding, Building* buildingToUseCatchmentArea,
                           bool useStreetOnly, bool& insideSourceOrDestinationBuilding) const {

    Map* map = context->game->getMap();

    // Kachel überprüfen
    MapTile* mapTile = map->getMapTileAt(mapCoords);
    if (mapTile == nullptr) {
        return false; // außerhalb der Karte
    }

    const MapTileConfig* mapTileConfig = mapTile->getMapTileConfig();
    if (!mapTileConfig->isWalkableAndBuildable) {
        return false; // nur auf Grass darf man laufen
    }

    // Steht ein Gebäude im Weg?
    insideSourceOrDestinationBuilding = false;
    if (mapTile->mapObject != nullptr) {
        Building* building = dynamic_cast<Building*>(mapTile->mapObject);
        if (building != nullptr) {
            if (building == sourceBuilding || building == destinationBuilding) {
                insideSourceOrDestinationBuilding = true; // in Start- oder Zielgebäude bewegen is OK
            } else {
                return false; // anderes Gebäude, Kachel nicht betretbar
            }
        }
    }

    // Gelände prüfen, kann man darf laufen?
    if (useStreetOnly) {
        if (!insideSourceOrDestinationBuilding && !map->isStreetAt(mapCoords)) {
            return false; // keine Straße da
        }
    }

    // Einschränkung durch Einzugsbereich?
    if (buildingToUseCatchmentArea != nullptr) {
        if (!buildingToUseCatchmentArea->isInsideCatchmentArea(context->configMgr, mapCoords)) {
            return false;
        }
    }

    return true;
}