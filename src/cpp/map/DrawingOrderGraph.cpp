#include <algorithm>
#include <iostream>
#include <set>
#include "map/DrawingOrderGraph.h"


void DrawingOrderGraph::addNode(MapObject* mapObject) {
    vertexes.push_back(mapObject);

    // Zeichenabhängigkeiten checken; Komplexität O(n)
    bool mapObjectHasIncomingEdge = false;
    for (auto iter = vertexes.cbegin(); iter != vertexes.cend(); iter++) {
        MapObject* otherMapObject = *iter;
        if (otherMapObject == mapObject) {
            continue;
        }

        int drawOrder = mustBeDrawnBefore(mapObject, otherMapObject);

        if (drawOrder == MUST_BE_DRAWN_AFTER) {
            outgoingEdges[otherMapObject].push_back(mapObject);
            incomingEdges[mapObject].push_back(otherMapObject);
            mapObjectHasIncomingEdge = true;
        }
        else if (drawOrder == MUST_BE_DRAWN_BEFORE) {
            outgoingEdges[mapObject].push_back(otherMapObject);
            incomingEdges[otherMapObject].push_back(mapObject);
            vertexesWithNoIngoingEdges.remove(otherMapObject);
        }
    }

    if (!mapObjectHasIncomingEdge) {
        vertexesWithNoIngoingEdges.push_back(mapObject);
    }

#ifdef DEBUG_DRAWING_ORDER
    debugOutputGraph();
#endif

    reorderMapObjects();
}

void DrawingOrderGraph::removeNode(MapObject* mapObject) {
    for (auto iter = outgoingEdges[mapObject].cbegin(); iter != outgoingEdges[mapObject].cend(); iter++) {
        MapObject* otherMapObject = *iter;

        // Waren wir die letzte eingehende Kante an diesen Knoten, müssen wir ihn in vertexesWithNoIngoingEdges hinzufügen
        if (incomingEdges[otherMapObject].size() == 1 && *(incomingEdges[otherMapObject].cbegin()) == mapObject) {
            vertexesWithNoIngoingEdges.push_back(otherMapObject);
        }

        incomingEdges[otherMapObject].remove(mapObject);
    }

    for (auto iter = incomingEdges[mapObject].cbegin(); iter != incomingEdges[mapObject].cend(); iter++) {
        MapObject* otherMapObject = *iter;

        outgoingEdges[otherMapObject].remove(mapObject);
    }

    vertexes.remove(mapObject);
    incomingEdges.erase(mapObject);
    outgoingEdges.erase(mapObject);
    vertexesWithNoIngoingEdges.remove(mapObject);

#ifdef DEBUG_DRAWING_ORDER
    debugOutputGraph();
#endif

    // Beim Entfernen von Knoten müssen wir die Objekte nicht neu sortieren :-)
}

void DrawingOrderGraph::clear() {
    vertexes.clear();
    incomingEdges.clear();
    outgoingEdges.clear();
    vertexesWithNoIngoingEdges.clear();
}

#ifdef DEBUG_DRAWING_ORDER
void DrawingOrderGraph::debugOutputGraph() {
    std::cout << "-- debugOutputGraph: ---------------------------------------" << std::endl;

    std::cout << "Vertexes: " << std::endl << "  ";
    for (auto iter = vertexes.cbegin(); iter != vertexes.cend(); iter++) {
        int mapX, mapY;
        (*iter)->getMapCoords(mapX, mapY);
        std::cout << "(" << mapX << ", " << mapY << ") ";
    }
    std::cout << std::endl << std::endl;

    std::cout << "Outgoing Edges: " << std::endl;
    for (auto iter = outgoingEdges.cbegin(); iter != outgoingEdges.cend(); iter++) {
        int mapX, mapY;
        (*iter).first->getMapCoords(mapX, mapY);
        std::cout << "  (" << mapX << ", " << mapY << "): ";

        for (auto iter2 = (*iter).second.cbegin(); iter2 != (*iter).second.cend(); iter2++) {
            int mapX2, mapY2;
            (*iter2)->getMapCoords(mapX2, mapY2);
            std::cout << "(" << mapX2 << ", " << mapY2 << ") ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Incoming Edges: " << std::endl;
    for (auto iter = incomingEdges.cbegin(); iter != incomingEdges.cend(); iter++) {
        int mapX, mapY;
        (*iter).first->getMapCoords(mapX, mapY);
        std::cout << "  (" << mapX << ", " << mapY << "): ";

        for (auto iter2 = (*iter).second.cbegin(); iter2 != (*iter).second.cend(); iter2++) {
            int mapX2, mapY2;
            (*iter2)->getMapCoords(mapX2, mapY2);
            std::cout << "(" << mapX2 << ", " << mapY2 << ") ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Vertexes with no incoming edges: " << std::endl << "  ";
    for (auto iter = vertexesWithNoIngoingEdges.cbegin(); iter != vertexesWithNoIngoingEdges.cend(); iter++) {
        int mapX, mapY;
        (*iter)->getMapCoords(mapX, mapY);
        std::cout << "(" << mapX << ", " << mapY << ") ";
    }
    std::cout << std::endl;

    std::cout << "------------------------------------------------------------" << std::endl;
}
#endif

int DrawingOrderGraph::mustBeDrawnBefore(MapObject* mo1, MapObject* mo2) {
    // mo2 ist Referenzgebäude

    int mo1mx, mo1my, mo1mw, mo1mh;
    int mo2mx, mo2my, mo2mw, mo2mh;
    mo1->getMapCoords(mo1mx, mo1my, mo1mw, mo1mh);
    mo2->getMapCoords(mo2mx, mo2my, mo2mw, mo2mh);

    // Mindestens eine Kachel von mo1 liegt im roten Bereich (s. doc/rendering-order.xcf): vorher zeichnen
    if ((mo1mx < mo2mx + mo2mw) && (mo1my < mo2my + mo2mh)) {
        return MUST_BE_DRAWN_BEFORE;
    }

    // Mindestens eine Kachel von mo1 liegt im grünen Bereich (s. doc/rendering-order.xcf): nachher zeichnen
    else if ((mo1mx + mo1mw > mo2mx) && (mo1my + mo1mh > mo2my)) {
        return MUST_BE_DRAWN_AFTER;
    }

    // Alle Kacheln liegen im "irrelevant"-Bereich
    else {
        return DRAW_ORDER_IRRELEVANT;
    }
}

void DrawingOrderGraph::reorderMapObjects() {

    int drawingOrderIndex = 0;

    /*
     * verticesNotYetDone:
     * Set mit allen Knoten, wo wir durch Herausnehmen die Knoten "durchstreichen", die wir bereits
     * in die Ergebnisliste aufgenommen haben.
     *
     * verticesDone:
     * Ergebnisliste. Wenn wir fertig sind, stehen da alle Objekte in der richtigen Reihenfolge drin.
     *
     * nextVerticesCandidates:
     * Set mit möglichen Knoten, die als nächstes in die Ergebnisliste kommen. Wir beginnen mit den
     * Knoten, die keine eingehenden Kanten haben und arbeiten uns dann im Graphen vorwärts.
     */
    std::set<MapObject*> verticesNotYetDone;
    std::copy(mapObjects->begin(), mapObjects->end(),
        std::insert_iterator<std::set<MapObject*>>(verticesNotYetDone, verticesNotYetDone.begin()));

    std::list<MapObject*> verticesDone;

    // Wir beginnen mit den Knoten, die keine eingehenden Kanten haben
    std::set<MapObject*> nextVerticesCandidates;
    std::copy(vertexesWithNoIngoingEdges.begin(), vertexesWithNoIngoingEdges.end(),
        std::insert_iterator<std::set<MapObject*>>(nextVerticesCandidates, nextVerticesCandidates.begin()));

    // Machen, bis wir alle Knoten erledigt haben
    while (!verticesNotYetDone.empty()) {
        for (auto iter = nextVerticesCandidates.cbegin(); iter != nextVerticesCandidates.cend(); iter++) {
            MapObject* nextVertexCandidate = *iter;

            // Knoten bereits in Ergebnisliste? Überspringen und aus dem Kandidaten-Set nehmen
            if (std::find(verticesDone.begin(), verticesDone.end(), nextVertexCandidate) != verticesDone.end()) {
                nextVerticesCandidates.erase(iter);
                continue;
            }

            // Prüfen, ob Abhängigkeiten erfüllt sind.
            std::list<MapObject*> incomingEdgesToCandidate = incomingEdges[nextVertexCandidate];
            bool allIncomingEdgesVisited = true;
            for (auto iter2 = incomingEdgesToCandidate.cbegin(); iter2 != incomingEdgesToCandidate.cend(); iter2++) {
                if (verticesNotYetDone.find(*iter2) != verticesNotYetDone.end()) {
                    // unerfüllte Abhängigkeit, diesen Knoten für diesen Durchgang ignorieren
                    allIncomingEdgesVisited = false;
                    break;
                }
            }

            // Nicht alle Abhängigkeiten erfüllt -> nächsten Kandidaten nehmen
            if (!allIncomingEdgesVisited) {
                continue;
            }

            // alle eingehenden Kanten erledigt -> Knoten in die Ergebnisliste aufnehmen
            verticesNotYetDone.erase(nextVertexCandidate);
            verticesDone.push_back(nextVertexCandidate);

            // unser eigentliches Ziel: drawingOrderIndex setzen
            nextVertexCandidate->setDrawingOrderIndex(drawingOrderIndex++);

            // Nachfolgeknoten des grade gefundenen Knoten, die wir noch nicht erledigt haben,
            // als neue Kandidaten vormerken
            std::list<MapObject*> outgoingEdgesFromCandidate = outgoingEdges[nextVertexCandidate];
            for (auto iter2 = outgoingEdgesFromCandidate.cbegin(); iter2 != outgoingEdgesFromCandidate.cend(); iter2++) {
                // Knoten aufnehmen, wenn er noch nicht bereits im Kandidaten-Set ist,
                // aber noch in der Todo-Liste ist (d.h. nicht schon in der Ergebnisliste ist)
                if ((nextVerticesCandidates.find(*iter2) == nextVerticesCandidates.end()) &&
                    (verticesNotYetDone.find(*iter2) != verticesNotYetDone.end())) {
                    nextVerticesCandidates.insert(*iter2);
                }
            }
        }
    }

    // Sortierung durchführen
    mapObjects->sort([] (MapObject* mo1, MapObject* mo2) {
        return (mo1->getDrawingOrderIndex() < mo2->getDrawingOrderIndex());
    });
}