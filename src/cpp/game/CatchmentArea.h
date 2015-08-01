#ifndef _CATCHMENT_AREA_H
#define _CATCHMENT_AREA_H

#include "map/Building.h"
#include "map/buildqueue/MapObjectToBuild.h"
#include "map/coords/MapCoords.h"


/**
 * @brief Statische Helper für den Einzugsbereich.
 * TODO später mal Instanzen dieser Klasse für den Einzugsbereich im MapObjectType anlegen.
 */
class CatchmentArea {

public:
    /**
     * @brief Testet, ob eine bestimmte Kachel innerhalb des Einzugsgebiets eines Gebäudes liegt
     *
     * @param building Gebäude, dessen Einzugsbereich für den Test verwendet wird
     * @param mapCoords Map-Koordinaten, die getestet werden
     * @return `true`, wenn die Kachel innerhalb des Einzugsgebiets des Gebäudes liegt; sonst `false`
     */
    static bool isInsideCatchmentArea(const Building& building, const MapCoords& mapCoords);

    /**
     * @brief Testet, ob eine bestimmte Kachel innerhalb des Einzugsgebiets eines zu bauenden Gebäudes liegt
     *
     * @param mapObjectToBuild zu bauendes Gebäude, dessen Einzugsbereich für den Test verwendet wird
     * @param mapCoords Map-Koordinaten, die getestet werden
     * @return `true`, wenn die Kachel innerhalb des Einzugsgebiets des zu bauenden Gebäudes liegt; sonst `false`
     */
    static bool isInsideCatchmentArea(const MapObjectToBuild& mapObjectToBuild, const MapCoords& mapCoords);

    /**
     * @brief Testet, ob ein bestimmtes Map-Objekt innerhalb des Einzugsgebiets eines Gebäudes liegt. Dies ist immer
     * dann der Fall, wenn mindestens eine Kachel des Mapobjekts im Einzuggebiet des Gebäudes liegt.
     *
     * @param building Gebäude, dessen Einzugsbereich für den Test verwendet wird
     * @param otherMapObject Map-Objekt, was getestet wird
     * @return `true`, wenn das Map-Objekt innerhalb des Einzugsgebiets liegt; sonst `false`
     */
    static bool isInsideCatchmentArea(const Building& building, const MapObjectFixed& otherMapObject);

private:
    /**
     * @brief Helper-Methode, die die eigentliche Logik übernimmt, ob eine Map-Koordinate innerhalb eines
     * Einzugsbereichs eines anderen Gebäudes liegt.
     *
     * @param catchmentArea Einzugsbereich, mit dem wir den Test durchführen
     * @param view Ausrichtung des Gebäudes, mit dem wir den Test durchführen
     * @param buildingMapCoords Koordinaten des Gebäudes, mit dem wir den Test durchführen
     * @param buildingMapWidth Map-Koordinaten-Breite des Gebäudes, mit dem wir den Test durchführen
     *                         (bereits entsprechend `view` korrigiert)
     * @param buildingMapHeight Map-Koordinaten-Höhe des Gebäudes, mit dem wir den Test durchführen
     *                          (bereits entsprechend `view` korrigiert)
     * @param mapCoords zu testende Koordinaten
     * @return `true`, wenn die Koordinaten innerhalb des Einzugsgebiets des Gebäudes liegt; sonst `false`
     */
    static bool isInsideCatchmentArea(const RectangleData<char>& catchmentArea, const FourthDirection& view,
                                      const MapCoords& buildingMapCoords,
                                      const int buildingMapWidth, const int buildingMapHeight,
                                      const MapCoords& mapCoords);
};

#endif
