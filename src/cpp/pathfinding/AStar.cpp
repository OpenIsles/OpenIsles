#include <list>
#include <map>
#include <set>
#include "game/Game.h"
#include "pathfinding/AStar.h"

// Aus main.cpp importiert
extern Game* game;

#ifdef DEBUG_A_STAR
MapCoordinate AStar::debugAStar_source = MapCoordinate(-1, -1);
MapCoordinate AStar::debugAStar_destination = MapCoordinate(-1, -1);
Route* AStar::debugAStar_route = nullptr;
#endif


Route* AStar::findRoute(MapCoordinate source, MapCoordinate destination) {
    Map* map = game->getMap();

    // Ermitteln, aus und in welches Gebäude wir die Route berechnen wollen. Dies ist optional, eine Route muss nicht
    // notwendigerweise ein Gebäude am Anfang/Ende haben. In diesem Fall ist der Building-Zeiger nullptr.
    Building* sourceBuilding = dynamic_cast<Building*>(
        map->getMapTileAt(source.mapX, source.mapY)->mapObject);

    Building* destinationBuilding = dynamic_cast<Building*>(
        map->getMapTileAt(destination.mapX, destination.mapY)->mapObject);


    /********************* A*-Implementierung. Siehe https://de.wikipedia.org/wiki/A*-Algorithmus *********************/

    // Datenstruktur für den Algorithmus
    struct AStarNode {
        MapCoordinate mapCoordinate;           // Koordinaten des Knoten
        int f;                                 // f-Wert für den Algorithmus = geschätzte Kosten von diesem Knoten zum Ziel
    };

    // MapCoordinate-Komparator, damit das Set und die Map richtig funktionieren
    struct MapCoordinateComparator {
        bool operator() (const MapCoordinate& node1, const MapCoordinate& node2) const {
            return ((node1.mapY < node2.mapY) ||
                    ((node1.mapY == node2.mapY) && (node1.mapX < node2.mapX)));
        }
    };

    // Open-List: Zu untersuchende Knoten priorisiert. Der Knoten mit niedrigstem f-Wert steht "vorne".
    std::list<AStarNode> openList;

    // Closed-List: Abschließend untersuchte Knoten
    std::set<MapCoordinate, MapCoordinateComparator> closedList;

    // g-Werte für den Algorithmus. Sie enthalten die gesicherten Kosten, um einen bestimmten Knoten zu erreichen
    std::map<MapCoordinate, int, MapCoordinateComparator> g;

    // Map, die jedem Knoten einen Vorgängerknoten im Pfad zuweist. Wir rekonstruiieren damit am Ende die Route.
    std::map<MapCoordinate, MapCoordinate, MapCoordinateComparator> predecessorInPath;



    // Initialisierung: Es geht mit dem Startknoten los
    openList.push_back({ source, 0 });

    // Schleife: Machen, bis wir entweder am Ziel sind (= vorzeitiger Abbruch)
    //           oder keine Lösung existiert (= die openList leer is)
    do {
        // vielversprechensten Knoten aus der openList nehmen, d.h. den mit minimalsten f-Wert
        AStarNode currentNode = openList.front();
        openList.pop_front();

        // Ziel erreicht? Dann fertig :-)
        if (currentNode.mapCoordinate == destination) {
            break;
        }

        // Knoten abhaken, d.h. zur closedList hinzufügen
        closedList.insert(currentNode.mapCoordinate);

        // Nachfolgeknoten auf die openList setzen. Das sind alle Kacheln um die aktuelle Kachel herum
        for (int mapXOffset = -1; mapXOffset <= 1; mapXOffset++) {
            for (int mapYOffset = -1; mapYOffset <= 1; mapYOffset++) {
                if (mapXOffset == 0 && mapYOffset == 0) {
                    continue;
                }

                MapCoordinate successorMapCoordinate(
                    currentNode.mapCoordinate.mapX + mapXOffset, currentNode.mapCoordinate.mapY + mapYOffset);

                // Prüfen, ob diese Koordinate überhaupt betreten werden darf. Steht da ein Gebäude oder ist sie
                // außerhalb des zulässigen Einzugsbereichs (TODO), stellt sie keinen Knoten im Algorithmus dar und
                // wird einfach ignoriert.
                MapTile* mapTile = map->getMapTileAt(successorMapCoordinate.mapX, successorMapCoordinate.mapY);
                if (mapTile == nullptr) {
                    continue; // außerhalb der Karte
                }

                bool insideSourceOrDestinationBuilding = false;
                if (mapTile->mapObject != nullptr) {
                    Building* building = dynamic_cast<Building*>(mapTile->mapObject);
                    if (building != nullptr) {
                        if (building == sourceBuilding || building == destinationBuilding) {
                            insideSourceOrDestinationBuilding = true; // in Start- oder Zielgebäude bewegen is OK
                        } else {
                            continue; // anderes Gebäude, Kachel nicht betretbar
                        }
                    }
                }

                // Wenn Knoten auf closedList ist, ignorieren, mit dem sind wir schon fertig.
                if (closedList.find(successorMapCoordinate) != closedList.cend()) {
                    continue;
                }

                // g-Wert für den neuen Weg = g(Vorgängerknoten) + c(Vorgängerknoten, aktuellem Knoten)
                // Unsere Kantengewichte c sind immer 1, da wir alle anliegenden Felder gleich entfernt betrachten.
                // Ausnahme: Innerhalb von Start- und Zielgebäude können wir uns mit Kosten 0 bewegen, da es egal ist,
                // an welcher Stelle wir ein Gebäude betreten und verlassen.
                int c = (!insideSourceOrDestinationBuilding) ? 1 : 0;
                int tentativeG = g[currentNode.mapCoordinate] + c;

                // Haben wir schon einen Weg zu successorMapCoordinate? Ist der Weg besser als der grade gefundene,
                // dann nix zu tun.
                std::list<AStarNode>::iterator nodeAlreadyInOpenListIter = openList.end();
                for (std::list<AStarNode>::iterator iter = openList.begin(); iter != openList.end(); iter++) {
                    if ((*iter).mapCoordinate == successorMapCoordinate) {
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
                int h = abs(destination.mapX - successorMapCoordinate.mapX) +
                        abs(destination.mapY - successorMapCoordinate.mapY);
                int f = tentativeG + h;

                // Zeiger zum Vorgängerknoten setzen
                predecessorInPath[successorMapCoordinate] = currentNode.mapCoordinate;

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
        return nullptr;
    }

    // Ergebnisroute zusammenbauen, indem die Knoten zurückverfolgt werden. Wir bauen die Route damit von hinten her auf.
    MapCoordinate currentMapCoord = destination;

    Route* route = new Route();
    do {
        route->push_front(currentMapCoord);
        currentMapCoord = predecessorInPath[currentMapCoord]; // existiert kein Eintrag, wird implizit ein (-1, -1) angelegt
    } while(currentMapCoord.mapX != -1);

    return route;
}