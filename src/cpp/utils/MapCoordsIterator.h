#ifndef _MAP_COORDS_ITERATOR_H
#define _MAP_COORDS_ITERATOR_H

#include <functional>
#include "map/coords/MapCoords.h"
#include "map/Direction.h"

/**
 * @brief Iterator, der es uns ermöglicht, ein Rechteck von Map-Kacheln je Ansicht in der richtigen Reihenfolge
 * für das Rendering (auf dem Bildschirm oben nach unten) zu iterieren.
 */
class MapCoordsIterator {

private:
    int mapXMin, mapYMin;
    int mapXMax, mapYMax;
    const FourthDirection screenView;

public:
    /**
     * @brief Konstruktur. Die Ecken müssen nicht notwendigerweise oben/links und unten/rechts sein. Wir tauschen
     * die Koordinaten automatisch.
     *
     * @param mapCoords1 Ecke 1
     * @param mapCoords2 Ecke 2
     * @param screenView Ansicht, die zum Iterieren benutzt werden soll
     */
    MapCoordsIterator(
        const MapCoords& mapCoords1, const MapCoords& mapCoords2,
        const FourthDirection& screenView) : screenView(screenView) {

        mapXMin = std::min(mapCoords1.x(), mapCoords2.x());
        mapYMin = std::min(mapCoords1.y(), mapCoords2.y());

        mapXMax = std::max(mapCoords1.x(), mapCoords2.x());
        mapYMax = std::max(mapCoords1.y(), mapCoords2.y());
    }

    /**
     * @brief Führt das Iterieren durch. Als Parameter wird ein Callback erwartet, der pro Durchlauf aufgerufen wird.
     * @param function Callback-Funktion, die pro Durchlauf mit den Map-Koordinaten aufgerufen wird.
     */
    void iterate(std::function<void(const MapCoords&)> function) {
        if (screenView == Direction::SOUTH) {
            iterateInSouthView(function);
        } else if (screenView == Direction::EAST) {
            iterateInEastView(function);
        } else if (screenView == Direction::NORTH) {
            iterateInNorthView(function);
        } else if (screenView == Direction::WEST) {
            iterateInWestView(function);
        }
    }

private:
    /**
     * @brief Führt das Iterieren in Süd-Ansicht durch
     * @param function Callback-Funktion, die pro Durchlauf mit den Map-Koordinaten aufgerufen wird.
     */
    void iterateInSouthView(std::function<void(const MapCoords&)>& function) {
        MapCoords mapCoords;
        for (int y = mapYMin; y <= mapYMax; y++) {
            for (int x = mapXMin; x <= mapXMax; x++) {
                mapCoords.setTo(x, y);
                function(mapCoords);
            }
        }
    }

    /**
     * @brief Führt das Iterieren in Ost-Ansicht durch
     * @param function Callback-Funktion, die pro Durchlauf mit den Map-Koordinaten aufgerufen wird.
     */
    void iterateInEastView(std::function<void(const MapCoords&)>& function) {
        MapCoords mapCoords;
        for (int x = mapXMin; x <= mapXMax; x++) {
            for (int y = mapYMax; y >= mapYMin; y--) {
                mapCoords.setTo(x, y);
                function(mapCoords);
            }
        }
    }

    /**
     * @brief Führt das Iterieren in Nord-Ansicht durch
     * @param function Callback-Funktion, die pro Durchlauf mit den Map-Koordinaten aufgerufen wird.
     */
    void iterateInNorthView(std::function<void(const MapCoords&)>& function) {
        MapCoords mapCoords;
        for (int y = mapYMax; y >= mapYMin; y--) {
            for (int x = mapXMax; x >= mapXMin; x--) {
                mapCoords.setTo(x, y);
                function(mapCoords);
            }
        }
    }

    /**
     * @brief Führt das Iterieren in West-Ansicht durch
     * @param function Callback-Funktion, die pro Durchlauf mit den Map-Koordinaten aufgerufen wird.
     */
    void iterateInWestView(std::function<void(const MapCoords&)>& function) {
        MapCoords mapCoords;
        for (int x = mapXMax; x >= mapXMin; x--) {
            for (int y = mapYMin; y <= mapYMax; y++) {
                mapCoords.setTo(x, y);
                function(mapCoords);
            }
        }
    }

};

#endif