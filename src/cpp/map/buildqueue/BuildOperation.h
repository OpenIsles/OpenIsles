#ifndef _BUILD_OPERATION_H
#define _BUILD_OPERATION_H

#include <cassert>
#include <list>
#include "map/buildqueue/BuildOperation.h"
#include "map/buildqueue/BuildOperationResult.h"
#include "map/buildqueue/MapObjectToBuild.h"
#include "map/MapObject.h"
#include "map/Street.h"
#include "game/Colony.h"

class BuildOperation : public ContextAware {

private:
    /**
     * @brief Spieler der bauen will
     */
    const Player& player;


    /**
     * @brief Bauaufträge ("Build-Queue")
     */
    std::list<MapObjectToBuild> mapObjectsToBuild;

    /**
     * @brief Kolonie, in der gebaut wird.
     *
     * Es kann immer nur in einer Kolonie gebaut werden. Dieses Feld ist erst gesetzt, wenn
     * der erste Bauauftrag in der Queue ist.
     */
    const Colony* colony = nullptr;

    /**
     * @brief Berechnetes Ergebnis
     */
    BuildOperationResult result;

public:
    BuildOperation(const Context* const context, const Player& player) : ContextAware(context), player(player) {
    }

    const std::list<MapObjectToBuild>& getMapObjectsToBuild() const {
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
        mapObjectsToBuild.push_back({ mapObjectFixed.getMapCoords() });
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
     * @brief Überprüft, ob ein geplantes Objekt in die Karte gesetzt werden kann oder was im Weg ist.
     * @param mapObjectToBuild zu platzierendes Objekt
     * @return `true`, wenn alles frei ist oder überbar ist; sonst `false`.
     */
    bool isSomethingInTheWayOnTheMap(const MapObjectToBuild& mapObjectToBuild) const;

    /**
     * @brief Überprüft, ob ein Map-Objekt über ein anderes Map-Objekt gebaut werden darf.
     *
     * "Überbauen erlaubt" gilt auch "Straße über Straße". Diese Art von Überbauen ist gratis, muss aber
     * von dieser Methode explizit erlaubt werden, dass die Baulogik es erlaubt.
     *
     * @param mapObjectTypeThere Typ von Map-Objekt, was bereits da ist
     * @param mapObjectTypeToBuildOver Typ von Map-Objekt, der drüber gebaut werden soll
     * @return `true` wenn Überbauen erlaubt ist.
     */
    bool mayBuildOver(const MapObjectType* mapObjectTypeThere, const MapObjectType* mapObjectTypeToBuildOver) const;

    /**
     * @brief Berechnet ein StreetConnections-Objekt für ausgewählte Koordinaten
     * @param mapCoords Map-Koordinaten, an dessen Stelle die Straße ist, für die das
     *                  StreetConnections-Objekt berechnet werden soll
     * @return befülltes StreetConnections-Objekt
     */
    StreetConnections calculateStreetConnections(const MapCoords& mapCoords) const;

    /**
     * @brief Fügt ggf. weitere Replacements ins BuildOperationResult hinzu, die Straßenersetzungen entsprechen.
     * @param streetToAdeptAround Straße, um die herum ggf. Ersetzungen hinzugefügt werden sollen.
     */
    void adeptExistingStreets(const Street& streetToAdeptAround);

    /**
     * @brief Aktualisiert `result` nach einer Änderung der Build-Queue
     */
    void rebuildResult();

};


#endif

