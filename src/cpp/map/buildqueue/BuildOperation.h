#ifndef _BUILD_OPERATION_H
#define _BUILD_OPERATION_H

#include <cassert>
#include <forward_list>
#include "map/buildqueue/BuildOperation.h"
#include "map/buildqueue/BuildOperationResult.h"
#include "map/buildqueue/MapObjectToBuild.h"
#include "map/MapObject.h"
#include "game/Colony.h"

class BuildOperation {

private:
    /**
     * Spieler der bauen will
     */
    const Player& player;

    /**
     * Dependency
     */
    const Game& game;


    /**
     * Bauaufträge ("Build-Queue")
     */
    std::forward_list<MapObjectToBuild> mapObjectsToBuild;

    /**
     * Berechnetes Ergebnis
     */
    BuildOperationResult result;

public:
    BuildOperation(const Player& player, const Game& game) : player(player), game(game) {
    }

    const std::forward_list<MapObjectToBuild>& getMapObjectsToBuild() const {
        return mapObjectsToBuild;
    }

    const BuildOperationResult& getResult() const {
        return result;
    }

    /**
     * @brief Fügt dem Bauauftrag ein neues Objekt hinzu, was gebaut werden soll.
     *
     * Würde sich das neue Objekt mit einem bestehenden Objekt im Bauauftrag überlappen, so ist dies eine No-Op.
     * Das hat den Hintergrund, dass wir nicht wollen, dass der Aufrufer diese Kollisionerkennung durchführt.
     *
     * @param mapCoords Koordinaten, an der das Objekt positioniert werden soll.
     * @param mapObjectType Typ des zu positionierenden Objekts
     * @param view Ausrichtung des zu positionierenden Objekts
     */
    void requestBuild(const MapCoords& mapCoords, const MapObjectType* mapObjectType, const FourthDirection& view);

    /**
     * @brief Fügt dem Bauauftrag ein neues Objekt hinzu, was gebaut werden soll.
     * Wie `requestBuild()` allerdings ist es auch eine No-Op, wenn kein Platz für das Objekt auf der Karte ist.
     *
     * @param mapCoords Koordinaten, an der das Objekt positioniert werden soll.
     * @param mapObjectType Typ des zu positionierenden Objekts
     * @param view Ausrichtung des zu positionierenden Objekts
     */
    void requestBuildWhenNothingInTheWay(
        const MapCoords& mapCoords, const MapObjectType* mapObjectType, const FourthDirection& view);

    // TODO Objekt abreißen
    /**
     * @brief Fügt dem Bauauftrag ein Objekt hinzu, was abgerissen werden soll.
     * @param mapObjectFixed Objekt, das abgerissen werden soll.
     */
    void requestRemoval(const MapObjectFixed& mapObjectFixed) {
        mapObjectsToBuild.push_front({ mapObjectFixed.getMapCoords() });
        rebuildResult();
    }

    /**
     * @brief Wird aufgerufen, wenn sich die verfügbaren Baumaterialen geändert haben (könnten) und wir deshalb
     * eine Neuberechnung des Result-Objekts vornehmen sollen.
     */
    void updateBuildMaterials() {
        rebuildResult();
    }

    /**
     * @brief Führt die geplante Bauoperation wirklich aus.
     */
    void doBuild();

private:
    /**
     * @brief Aktualisiert `result` nach einer Änderung der Build-Queue
     */
    void rebuildResult();

    /**
     * @brief Überprüft, ob ein geplantes Objekt in die Karte gesetzt werden kann oder was im Weg ist.
     * @param mapObjectToBuild zu platzierendes Objekt
     * @return `true`, wenn alles frei ist (TODO BUILDOPERATION oder überbar ist); sonst `false`.
     */
    bool isSomethingInTheWayOnTheMap(const MapObjectToBuild& mapObjectToBuild);

};


#endif

