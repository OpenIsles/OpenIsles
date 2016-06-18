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
     * @brief `true`, wenn hier gebaut / abgerissen werden darf.
     *
     * `false`,
     * - wenn an mindestens einer Kachel was im Weg ist, was nicht überbaut werden kann
     * - wenn das Terrain nicht passt
     * - uns das Gebiet nicht gehört
     */
    bool buildAllowed = false;

    /**
     * @brief `false`, wenn die Resourcen nicht mehr reichen, ob dieses Objekt zu bauen.
     * In diesem Fall blinkt das Objekt in der GUI.
     *
     * Ersetzungen (`costsNothingBecauseOfChange == true`) blinken aus Prinzip nicht. Dieses Flag
     * hier kann aber trotzdem `false` gesetzt sein, wenn die Resourcen für die Straße bis zu dieser
     * Kachel nicht mehr reichen, sodass die Ersetzung auch nicht gebaut wird.
     */
    bool resourcesEnoughToBuildThis = false;

    /**
     * @brief `true`, wenn diese Kachel kein Baumaterial kostet, da ein bestehendes Objekt ersetzt wird.
     *
     * Dies ist z.&nbsp;B. der Fall, wenn ein gerades Straßenstück durch ein T-Stück ersetzt wird.
     *
     * Allerdings gilt das nicht für alle Ersetzungen. Wird ein Feldweg durch eine Pflasterstraße
     * ersetzt, so kostet das den Spieler und dieses Flag ist `false` gesetzt.
     */
    bool costsNothingBecauseOfChange = false;

    /**
     * @brief MapObject, das beim Abschluss der Bauoperation das bestehende Map-Objekt ersetzt
     * (bzw. hinzugefügt werden soll).
     *
     * `nullptr` bedeutet "keine Ersetzung". Z.&nbsp;B. wenn die Resourcen nicht reichen, ist nur
     *  mapObjectToDraw gesetzt und `mapObjectToReplaceWith == nullptr`.
     *
     * Um das Objekt zu löschen, muss stattdessen `deleteMapObjectThere` auf `true` gesetzt werden.
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
     * Wird für Abreißen benutzt.
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
        /**
         * @brief Es kann gebaut werden (d.&nbsph. nichts im Weg oder kann überbaut werden).
         */
        OK,

        /**
         * @brief Es kann nicht gebaut werden. Dies kann mehrere Gründe haben:
         * - Es ist was im Weg, was nicht überbaut werden kann.
         * - Das Terrain passt nicht (z.&nbsp;B. Straße ins Wasser bauen)
         * - Das Zielgebiet ist nicht erschlossen oder gehört dem falschen Spieler
         */
        NOT_OK

    } result = OK;

    /**
     * @brief Resourcen-Kosten gesamt
     */
    BuildingCosts buildingCosts;

};


#endif
