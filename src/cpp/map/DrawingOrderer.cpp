#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include "map/DrawingOrderer.h"
#include "map/MapObject.h"


// TODO Der Algorithmus ist sehr langsam und sollte später optimiert werden
// Wichtig wäre, wenn nur EIN Gebäude hinzukommt/wegfällt, nicht wieder ALLE Gebäude neuzuordnen
void DrawingOrderer::reorder() {
    /* 
     * Schritt 1: Graph aufbauen, welches Map-Objekt vor welchem anderen gezeichnet werden muss
     * 
     * Beispiel:
     * 
     *     A --> B --> D       E --> F
     *     |         /^
     *     |  -------                            --> = muss VOR ... gezeichnet werden
     *     v /
     *     C
     * 
     * incomingEdges:
     *   A -> {}, B -> {A}, C -> {A}, D -> {B, C}, E -> {}, F -> {E}
     * outgoingEdges:
     *   A -> {B, C}, B -> {D}, C -> {D}, D -> {}, E -> {E}, F -> {} 
     * haveNoDependencySet:
     *   {A, E}
     */
    std::map<MapObject*, std::set<MapObject*>> incomingEdges;
    std::map<MapObject*, std::set<MapObject*>> outgoingEdges;
    std::set<MapObject*> verticesWithNoIncomingEdge;
    
    for (auto iter2 = mapObjects->cbegin(); iter2 != mapObjects->cend(); iter2++) {
		MapObject* mapObject2 = *iter2;
        
        for (auto iter1 = mapObjects->cbegin(); iter1 != mapObjects->cend(); iter1++) {
            MapObject* mapObject1 = *iter1;
            
            if (mapObject1 == mapObject2) {
                continue;
            }
            
            if (mustBeDrawnBefore(mapObject1, mapObject2)) {
                incomingEdges[mapObject2].insert(mapObject1);
                outgoingEdges[mapObject1].insert(mapObject2);
            }
        }
        
        if (incomingEdges[mapObject2].empty()) {
            verticesWithNoIncomingEdge.insert(mapObject2);
        }
    }
    
    /* 
     * Schritt 2: Alle Knoten in ein Set werfen und den Graphen ablaufen.
     * 
     * verticesNotYetDone:
     * Set mit allen Knoten, wo wir durch Herausnehmen die Knoten "durchstreichen", die wir bereits
     * in die Ergebnisliste aufgenommen haben.
     * 
     * resultList:
     * Ergebnisliste. Wenn wir fertig sind, stehen da alle Objekte in der richtigen Reihenfolge drin.
     * 
     * nextVerticesCandidates:
     * Set mit möglichen Knoten, die als nächstes in die Ergebnisliste kommen. Wir beginnen mit den
     * Knoten, die keine eingehenden Kanten haben und arbeiten uns dann im Graphen vorwärts.
     */
    std::set<MapObject*> verticesNotYetDone;
    std::copy(mapObjects->begin(), mapObjects->end(), 
            std::insert_iterator<std::set<MapObject*>>(verticesNotYetDone, verticesNotYetDone.begin()));
    
    std::list<MapObject*> resultList;
    
    // Wir beginnen mit den Knoten, die keine eingehenden Kanten haben
    std::set<MapObject*> nextVerticesCandidates;
    std::copy(verticesWithNoIncomingEdge.begin(), verticesWithNoIncomingEdge.end(), 
            std::insert_iterator<std::set<MapObject*>>(nextVerticesCandidates, nextVerticesCandidates.begin()));
    
    // Machen, bis wir alle Knoten erledigt haben
    while (!verticesNotYetDone.empty()) {
        for (auto iter = nextVerticesCandidates.cbegin(); iter != nextVerticesCandidates.cend(); iter++) {
            MapObject* nextVertexCandidate = *iter;
            
            // Knoten bereits in Ergebnisliste? Überspringen und aus dem Kandidaten-Set nehmen
            if (std::find(resultList.begin(), resultList.end(), nextVertexCandidate) != resultList.end()) {
                nextVerticesCandidates.erase(iter);
                continue;
            }
            
            // Prüfen, ob Abhängigkeiten erfüllt sind.
            std::set<MapObject*> incomingEdgesToCandidate = incomingEdges[nextVertexCandidate];
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
            resultList.push_back(nextVertexCandidate);

            // Nachfolgeknoten des grade gefundenen Knoten, die wir noch nicht erledigt haben, 
            // als neue Kandidaten vormerken
            std::set<MapObject*> outgoingEdgesFromCandidate = outgoingEdges[nextVertexCandidate];
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
    
    /*
     * Schritt 3: Ergebnisliste setzen
     */
    mapObjects->swap(resultList);
}

bool DrawingOrderer::mustBeDrawnBefore(MapObject* mo1, MapObject* mo2) {
    // mo2 ist Referenzgebäude

    int mo1mx, mo1my, mo1mw, mo1mh;
    int mo2mx, mo2my, mo2mw, mo2mh;
    mo1->getMapCoords(mo1mx, mo1my, mo1mw, mo1mh);
    mo2->getMapCoords(mo2mx, mo2my, mo2mw, mo2mh);

    bool result = false;

    // Mindestens eine Kachel von mo1 liegt im roten Bereich (s. doc/rendering-order.xcf): vorher zeichnen
    if ((mo1mx < mo2mx + mo2mw) && (mo1my < mo2my + mo2mh)) {
        result = true;
    }

    // Mindestens eine Kachel von mo1 liegt im grünen Bereich (s. doc/rendering-order.xcf): nachher zeichnen
    else if ((mo1mx + mo1mw > mo2mx) && (mo1my + mo1mh > mo2my)) {
        result = false;
    }

    // Alle Kacheln liegen im "irrelevant"-Bereich: nachher zeichnen
    else {
        result = false;
    }

    return result;
}
