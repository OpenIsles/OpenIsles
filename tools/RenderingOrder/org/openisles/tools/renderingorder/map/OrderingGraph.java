package org.openisles.tools.renderingorder.map;

import edu.uci.ics.jung.graph.DirectedSparseGraph;
import org.openisles.tools.renderingorder.RenderingOrder;

import java.util.HashSet;
import java.util.List;
import java.util.Set;

public class OrderingGraph extends DirectedSparseGraph<MapObject, Object> {

    private List<MapObject> mapObjects;

    public OrderingGraph(List<MapObject> mapObjects) {
        this.mapObjects = mapObjects;

        // Knoten hinzufügen
        for (MapObject mapObject : mapObjects) {
            addVertex(mapObject);
        }

        // Abhängigkeiten aufbauen
        for (MapObject mo1 : mapObjects) {
            for (MapObject mo2 : mapObjects) {
                if (mo1 == mo2) {
                    continue;
                }

                Boolean mustBeDrawnBefore = mustBeDrawnBefore(mo1, mo2);
                if (mustBeDrawnBefore != null && mustBeDrawnBefore) {
                    addEdge(new Object(), mo1, mo2);
                }
            }
        }
    }

    private static Boolean mustBeDrawnBefore(MapObject mo1, MapObject mo2) {
        // mo2 ist Referenzgebäude

        Graphic graphic1 = RenderingOrder.graphicsMgr.get(mo1.getGraphicType());
        int mo1mx = mo1.getMapX();
        int mo1my = mo1.getMapY();
        int mo1mw = graphic1.getMapWidth();
        int mo1mh = graphic1.getMapHeight();

        Graphic graphic2 = RenderingOrder.graphicsMgr.get(mo2.getGraphicType());
        int mo2mx = mo2.getMapX();
        int mo2my = mo2.getMapY();
        int mo2mw = graphic2.getMapWidth();
        int mo2mh = graphic2.getMapHeight();

        // Mindestens eine Kachel von mo1 liegt im roten Bereich (s. doc/rendering-order.xcf): vorher zeichnen
        if ((mo1mx < mo2mx + mo2mw) && (mo1my < mo2my + mo2mh)) {
            return true;
        }

        // Mindestens eine Kachel von mo1 liegt im grünen Bereich (s. doc/rendering-order.xcf): nachher zeichnen
        else if ((mo1mx + mo1mw > mo2mx) && (mo1my + mo1mh > mo2my)) {
            return false;
        }

        // Alle Kacheln liegen im "irrelevant"-Bereich
        else {
            return null;
        }
    }

    /**
     * Nimmt den Graphen her und sortiert die Liste der mapObjects damit
     *
     */
    public void orderMapObjects() {
        Set<MapObject> verticesDone = new HashSet<MapObject>(); // Elemente hindrin sind richtig sortiert
        Set<MapObject> verticesWithAllDependenciesFulfilled = new HashSet<MapObject>(); // Knoten, die nun gezeichnet werden können, da alle ihre Vorgänger gezeichnet wurden

        Set<MapObject> verticesToDo = new HashSet<MapObject>(); // Knoten, die noch nicht gezeichnet wurden
        verticesToDo.addAll(mapObjects);

        // Algorithmus-Start: Objekte ohne Eingangskante (eins gibts mindestens immer!) können sofort gezeichnet werden
        for (MapObject mapObject : getVertices()) {
            if (getPredecessorCount(mapObject) == 0) {
                verticesWithAllDependenciesFulfilled.add(mapObject);
            }
        }

        // Algorithmus:
        int drawingIndex = 0;
        while (!verticesToDo.isEmpty()) {
            Set<MapObject> verticesWithAllDependenciesFulfilledNextIteration = new HashSet<MapObject>(); // Knoten, die in der nächsten Runde gezeichnet werden können

            // alle Knoten in verticesWithAllDependenciesFulfilled zeichnen
            for (MapObject mapObjectCanBeDrawnNow : verticesWithAllDependenciesFulfilled) {
                mapObjectCanBeDrawnNow.setDrawingIndex(drawingIndex++);

                verticesDone.add(mapObjectCanBeDrawnNow);
                verticesToDo.remove(mapObjectCanBeDrawnNow);

                // dann gucken, welche Knoten deshalb anschließend gezeichnet werden können
                for (MapObject successors : getSuccessors(mapObjectCanBeDrawnNow)) {
                    if (!verticesToDo.contains(successors)) {
                        continue; // der is schon gezeichnet
                    }

                    // gucken, ob wir seine Vorgänger nun alle erfüllen
                    boolean canDrawSuccessorNow = true;
                    for (MapObject predecessorsOfSuccessor : getPredecessors(successors)) {
                        if (!verticesDone.contains(predecessorsOfSuccessor)) {
                            canDrawSuccessorNow = false;
                            break; // schade, doch noch nicht
                        }
                    }

                    if (canDrawSuccessorNow) {
                        verticesWithAllDependenciesFulfilledNextIteration.add(successors);
                    }
                }
            }
            verticesWithAllDependenciesFulfilled = verticesWithAllDependenciesFulfilledNextIteration;
        }
    }
}
