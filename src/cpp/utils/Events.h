#ifndef _EVENTS_H
#define	_EVENTS_H

#include "map/coords/MapCoords.h"

/**
 * @brief Event-ID-Offset für das Event "Maus über andere Map-Koordinate bewegt".
 */
#define USER_EVENT_MOUSEMOTION_MAPCOORDS 0

/**
 * @brief Höchster Event-ID-Offset, den wir verwenden.
 * Dieser Wert muss angepasst werden, wenn ein neues Event hinzukommt.
 */
#define USER_EVENT_MAXEVENT USER_EVENT_MOUSEMOTION_MAPCOORDS


/**
 * @brief Hilfsklasse, die Konstanten über die ganze Anwendung hält
 */
struct MouseMotionMapCoordsEvent {

    /**
     * @brief Map-Koordinaten, wo der Mauszeiger vorher stand
     */
    MapCoords mapCoordsBefore;

    /**
     * @brief Map-Koordinaten, wo sich der Mauszeiger jetzt befindet
     */
    MapCoords mapCoordsNow;

};

#endif