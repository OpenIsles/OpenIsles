#ifndef _DRAWING_ORDER_GRAPH_H
#define _DRAWING_ORDER_GRAPH_H

#include <list>
#include <map>
#include "map/MapObject.h"
#include "map/MapUtils.h"


class MapObject;


/**
 * @brief Repräsentiert den Graphen zum Sortieren der Map-Objekte in die richtige Reihenfolge für das Zeichnen.
 * Diese Klasse hält eine Referenz auf die Liste der MapObjects und sortiert sie bei Bedarf um.
 */
class DrawingOrderGraph {
    
private:
    /**
     * @brief Referenz auf die Liste der MapObjects, die wir eigentlich sortieren wollen.
     */
    std::list<MapObject*>* mapObjects;

    /**
     * @brief Liste aller Knoten, d.h. aller Objekte
     */
    std::list<MapObject*> vertexes;

    /**
     * @brief Ausgehende Kanten eines Knoten, in Form einer Liste von Zielknoten.
     * D.h. diese Map ordnet jedem Objekt eine Liste von Objekten zu, die DANACH gezeichnet werden müssen
     */
    std::map<MapObject*, std::list<MapObject*>> outgoingEdges;

    /**
     * @brief Eingehende Kanten eines Knoten, in Form einer Liste von Quellknoten.
     * D.h. diese Map ordnet jedem Objekt eine Liste von Objekten zu, die DAVOR gezeichnet werden müssen
     */
    std::map<MapObject*, std::list<MapObject*>> incomingEdges;

    /**
     * @brief Liste der Knoten, die keine eingehenden Kanten haben.
     * D.h. Liste der Objekte, die zuerst gezeichnet werden können, weil sie ohne Abhängigkeit sind
     */
    std::list<MapObject*> vertexesWithNoIngoingEdges;

public:
    DrawingOrderGraph(std::list<MapObject*>* mapObjects) : mapObjects(mapObjects) {}
    ~DrawingOrderGraph() {}
    
    void addNode(MapObject* mapObject);
    void removeNode(MapObject* mapObject);
    void clear();

private:
    const int MUST_BE_DRAWN_BEFORE = 1;
    const int MUST_BE_DRAWN_AFTER = -1;
    const int DRAW_ORDER_IRRELEVANT = 0;
    
    /**
     * @brief Implementiert die Relation "mo1 muss VOR mo2 gezeichnet werden".
     *
     * Die Relation ist symmetrisch, d.h. für
     *   mustBeDrawnBefore(a, b) == MUST_BE_DRAWN_BEFORE gilt
     *   mustBeDrawnBefore(b, a) == MUST_BE_DRAWN_AFTER
     * und für
     *   mustBeDrawnBefore(a, b) == DRAW_ORDER_IRRELEVANT gilt
     *   mustBeDrawnBefore(b, a) == DRAW_ORDER_IRRELEVANT
     * .
     *
     * Leider ist die Relation nicht transitiv, weshalb sie nicht einfach in std::list.sort() benutzt werden kann.
     *
     * @param mo1 erstes Map-Objekt
     * @param mo2 zweites Map-Objekt = Referenzgebäude
     * @return MUST_BE_DRAWN_BEFORE, wenn mo1 VOR mo2 gezeichnet werden muss,
     *         MUST_BE_DRAWN_AFTER, wenn mo1 NACH mo2 gezeichnet werden muss,
     *         DRAW_ORDER_IRRELEVANT, wenn die Zeichenreihenfolge egal ist.
     */
    int mustBeDrawnBefore(MapObject* mo1, MapObject* mo2);

#ifdef DEBUG_DRAWING_ORDER
    /**
     * @brief Gibt den Graphen aus. Nur für Debugzwecke
     */
    void debugOutputGraph();
#endif

    /**
     * @brief Verwendet den aktuellen Graphen, um an den MapObjects die Sortiernummer drawingIndex zu setzen und danach
     * die Liste zu sortieren.
     */
    void reorderMapObjects();
};

#endif
