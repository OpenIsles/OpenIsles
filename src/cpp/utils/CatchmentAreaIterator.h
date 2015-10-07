#ifndef _CATCHMENT_AREA_ITERATOR_H
#define _CATCHMENT_AREA_ITERATOR_H

#include <algorithm>
#include <functional>
#include "map/Building.h"
#include "map/coords/MapCoords.h"

/**
 * @brief Iterator, der alle Kacheln iteriert, die in einem Einzugsbereich um ein Gebäude liegen.
 */
class CatchmentAreaIterator {

private:
    /**
     * @brief ermittelte Map-Kacheln im Einzugsbereich
     */
    std::vector<MapCoords> mapCoordsInCatchmentArea;

public:
    /**
     * @brief Konstruktur.
     *
     * @param building Gebäude, dessen Einzugsbereich iteriert werden soll
     * @param ignoreBuilding `true`, um die Kacheln, auf denen das Gebäude steht, auszunehmen
     */
    CatchmentAreaIterator(const Building& building, bool ignoreBuilding);

    /**
     * @brief Führt das Iterieren durch. Als Parameter wird ein Callback erwartet, der pro Durchlauf aufgerufen wird.
     * @param function Callback-Funktion, die pro Durchlauf mit den Map-Koordinaten aufgerufen wird.
     */
    void iterate(std::function<void(const MapCoords&)> function) {
        for (const MapCoords& mapCoords : mapCoordsInCatchmentArea) {
            function(mapCoords);
        }
    }

    // TODO Abwarten, wie häufig wir das brauchen. Aktuell hat diese Methode O(n). Wirds zu langsam,
    // sollten wir zusätzlich zum Vektor ein Set aufbauen, damit der Lookup hier schneller geht.
    /**
     * @brief Überprüft, ob eine bestimmte Map-Koordinate im Einzugsbereich liegt
     * @param mapCoords gesuchte Map-Koordinate
     * @return `true`, wenn `mapCoords` im Einzugsbereich liegt, sonst `false`
     */
    bool contains(const MapCoords& mapCoords) {
        auto iter = std::find(mapCoordsInCatchmentArea.cbegin(), mapCoordsInCatchmentArea.cend(), mapCoords);
        return (iter != mapCoordsInCatchmentArea.cend());
    }
};

#endif