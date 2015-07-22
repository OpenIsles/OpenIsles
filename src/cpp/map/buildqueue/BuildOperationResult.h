#ifndef _BUILD_OPERATION_RESULT_H
#define _BUILD_OPERATION_RESULT_H

#include <memory>
#include "map/buildqueue/MapObjectToBuild.h"
#include "map/coords/MapCoords.h"
#include "map/MapObject.h"


/**
 * @brief Unterstruktur von `BuildOperationResult`, die die Daten einer einzelnen Kachel enthält.
 */
struct BuildOperationResultBit {

    /**
     * @brief `true`, wenn an mindestens einer Kachel was im Weg ist, was nicht überbaut werden kann.
     */
    bool somethingInTheWay;

    /**
     * @brief `false`, wenn die Resourcen nicht mehr reichen, ob dieses Objekt zu bauen.
     */
    bool resourcesEnoughToBuildThis;

    /**
     * @brief MapObject, das beim Abschluss der Bauoperation das bestehende Map-Objekt ersetzt
     * (bzw. hinzugefügt werden soll). `nullptr` bedeutet "keine Ersetzung". Um das Objekt zu
     * löschen, muss stattdessen `deleteMapObjectThere` auf `true` gesetzt werden.
     */
    std::shared_ptr<MapObjectFixed> mapObjectToReplaceWith = nullptr;

    /**
     * @brief MapObject, das im aktuell gezeichnet werden soll.
     *
     * Dies muss nicht dasselbe wie `mapObjectToReplaceWith` sein. Beispielsweise wenn der Spieler einen
     * Straßenzug quer zu einem anderem Straßenzug bauen will, so wird die Kachel in der Mitte als
     * `mapObjectToDraw` eine Kreuzung haben, während `mapObjectToReplaceWith` ein T-Stück sein kann, wenn die
     * Resourcen nur genau bis dahin reichen.
     */
    std::shared_ptr<MapObjectFixed> mapObjectToDraw = nullptr;

    /**
     * @brief auf `true` gesetzt wird beim Abschluss der Bauoperation das bestehende Map-Objekt gelöscht.
     * Wird für Abreißen und Überbauen von kleineren Objekten durch größere benutzt benutzt.
     */
    bool deleteMapObjectThere = false;

};


/**
 * @brief Stellt das Ergebnis einer `BuildOperation` dar
 */
class BuildOperationResult : public std::unordered_map<const MapCoords, std::shared_ptr<BuildOperationResultBit>> {

public:
    /**
     * @brief gibt an, ob die Bauoperation grundsätzlich erlaubt oder was im Weg ist.
     * "Grundsätzlich erlaubt" kann auch bedeuten, dass die Rohstoffe nicht vollständig oder gar nicht reichen.
     */
    enum : unsigned char {
        OK,
        SOMETHING_IN_THE_WAY
    } result;

};


#endif
