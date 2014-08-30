#ifndef _DRAWING_ORDERER_H
#define _DRAWING_ORDERER_H

#include <list>

class MapObject;


/**
 * @brief Hilfsklasse zum Sortieren der Map-Objekte in die richtige Reihenfolge für das Zeichnen.
 */
class DrawingOrderer {     
    
private:
    /**
     * @brief Referenz auf die Liste der Map-Objekte, die zu sortieren ist
     */
    std::list<MapObject*>* mapObjects;

public:
    /**
     * @param mapObjects Referenz auf die Liste der Map-Objekte, die zu sortieren ist
     */
    DrawingOrderer(std::list<MapObject*>* mapObjects) : mapObjects(mapObjects) {}
    ~DrawingOrderer() {}
    
    /**
     * @brief Sortiert die Liste neu
     */
    void reorder();
    
private:
    
    /**
     * @brief Implementiert die Relation "mo1 muss VOR mo2 gezeichnet werden".
     * Leider ist die Relation nicht transitiv, weshalb sie nicht einfach in std::list.sort() benutzt werden kann.
     * @param mo1 erstes Map-Objekt
     * @param mo2 zweites Map-Objekt = Referenzgebäude
     */
    bool mustBeDrawnBefore(MapObject* mo1, MapObject* mo2);
    
};

#endif
