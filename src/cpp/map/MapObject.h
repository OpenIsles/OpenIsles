#ifndef _MAP_OBJECT_H
#define _MAP_OBJECT_H

#ifdef TESTS_ENABLED
#include <gtest/gtest.h>
#endif

#include "Context.h"
#include "game/Player.h"
#include "graphics/mgr/IGraphicsMgr.h"
#include "map/coords/MapCoords.h"
#include "map/Direction.h"
#include "map/MapCoordUtils.h"

/**
 * @brief Basisklasse für alles, was sich auf der Karte befinden kann
 */
class MapObject {
protected:
    /**
     * @brief Breite des Objekts in Map-Koordinaten.
     *
     * Info: Dies entspricht immer der Breite in der Ausrichtung des Gebäudes. Z.B. hat ein 3x2-Gebäude, was
     * auf Ost/West-Ansicht gedreht wurde, hat eine mapWidth von 2. Auf Süd/Nord-Ansicht hat es eine mapWidth von 3.
     * Dies ist analog der zugehörigen Grafik.
     */
    int mapWidth;

    /**
     * @brief Höhe des Objekts in Map-Koordinaten
     *
     * Info: Dies entspricht immer der Höhe in der Ausrichtung des Gebäudes. Z.B. hat ein 3x2-Gebäude, was
     * auf Ost/West-Ansicht gedreht wurde, hat eine mapHeight von 3. Auf Süd/Nord-Ansicht hat es eine mapHeight von 2.
     * Dies ist analog der zugehörigen Grafik.
     */
    int mapHeight;
    
    /**
     * @brief Spieler, dem das Objekt gehört, oder nullptr für spielerlose Objekte
     */
    Player* player = nullptr;

    /**
     * @brief Flags, die angeben, wie das Objekt gezeichnet wird.
     * Eine Kombination aus IGraphic::DRAWING_FLAG_-Konstanten wird erwartet.
     */
    int drawingFlags = 0;

    /**
     * @brief [Zeitpunkt](@ref gameTicks), wann dieses Objekt zuletzt aktualisiert wurde
     */
    unsigned long lastUpdateTicks;

public:
    MapObject() {
        // TODO Alle Objekte müssen lastUpdateTicks ordentlich gesetzt (mit Game#ticks initialisiert) haben. Eine Factory wäre hübsch
    }

    virtual ~MapObject() {
    }

    int getMapWidth() const {
        return mapWidth;
    }

    virtual void setMapWidth(int mapWidth) {
        this->mapWidth = mapWidth;
    }

    int getMapHeight() const {
        return mapHeight;
    }

    virtual void setMapHeight(int mapHeight) {
        this->mapHeight = mapHeight;
    }

    Player* getPlayer() const {
        return player;
    }

    void setPlayer(Player* player) {
        this->player = player;
    }

    int getDrawingFlags() const {
        return drawingFlags;
    }

    void setDrawingFlags(int drawingFlags) {
        this->drawingFlags = drawingFlags;
    }

    /**
     * @brief Liefert zurück, wie viele Game-Ticks seit dem letzten Update des Objekts vergangen sind.
     * @return Game-Ticks seit letzten Update
     */
    unsigned int getTicksSinceLastUpdate(const Context& context) const;

    // TODO Auf lastUpdateTicks sollte man eigentlich nicht von außen (maximal die Tests) zugreifen dürfen.
    unsigned long getLastUpdateTicks() const {
        return lastUpdateTicks;
    }

    void setLastUpdateTicks(unsigned long lastUpdateTicks) {
        this->lastUpdateTicks = lastUpdateTicks;
    }

    /**
     * @brief Aktualisiert das Objekt.
     * @param context (Dependency)
     * @return `true` im Normalfall. Wird `false` zurückgegeben, bedeutet das, dass die Update-Logik entschieden hat,
     * das Objekt müsse gelöscht werden.
     */
    bool update(const Context& context);

    /**
     * @brief Interne Update-Funktion, die von den Unterklassen implementiert werden muss und die klassen-spezifische
     * Update-Logik für das Objekt ausführt.
     *
     * @param context (Dependency)
     * @return `true` im Normalfall. Wird `false` zurückgegeben, bedeutet das, dass die Update-Logik entschieden hat,
     * das Objekt müsse gelöscht werden.
     */
    virtual bool updateObject(const Context& context) = 0;
};


/**
 * @brief Basisklasse für alles, was sich fix auf der Karte befinden kann. Fixe Objekte haben immer ganzzahlige
 * Map-Koordinaten
 */
class MapObjectFixed : public MapObject {

protected:
    /**
     * @brief Map-Koordinaten des Objekts
     */
    MapCoords mapCoords;

    /**
     * @brief Objekt-Typ
     */
    const MapObjectType* mapObjectType;

    /**
     * @brief Ausrichtung des Objekts
     */
    FourthDirection view;

public:
    /**
     * @brief Helper, der je nach Map-Objekt-Typ eine konkrete Instanz von `MapObjectFixed` erstellt.
     * @param mapObjectType Typ des zu erstellenden Map-Objekts
     * @return neue Instanz eines `MapObjectFixed` mit `type` bereits gesetzt
     */
    static MapObjectFixed* instantiate(const MapObjectType* mapObjectType);

public:
    virtual ~MapObjectFixed() {
    }

    void setMapCoords(const MapCoords& mapCoords) {
        this->mapCoords = mapCoords;
    }

    MapCoords& getMapCoords() {
        return mapCoords;
    }

    const MapCoords& getMapCoords() const {
        return mapCoords;
    }

    const MapObjectType* getMapObjectType() const {
        return mapObjectType;
    }

    void setMapObjectType(const MapObjectType* mapObjectType) {
        this->mapObjectType = mapObjectType;
    }

    const FourthDirection& getView() const {
        return view;
    }

    void setView(const FourthDirection& view) {
        this->view = view;
    }
};


/**
 * @brief Basisklasse für alles, was sich auf der Karte bewegen kann. Bewegliche Objekte haben immer
 * Gleitkomma-Map-Koordinaten, da sie sich fließend von Kachel zu Kachel bewegen.
 */
class MapObjectMoving : public MapObject {

#ifdef TESTS_ENABLED
    FRIEND_TEST(EconomicsMgrTest, updateCarrier); // Test soll die Update-Zeit via setLastUpdateTime() manipulieren dürfen.
#endif

protected:
    /**
     * @brief Map-Koordinaten des Objekts
     */
    DoubleMapCoords mapCoords;

public:
    virtual ~MapObjectMoving() {
    }

    void setMapCoords(const DoubleMapCoords& mapCoords) {
        this->mapCoords = mapCoords;
    }

    DoubleMapCoords& getMapCoords() {
        return mapCoords;
    }

    const DoubleMapCoords& getMapCoords() const {
        return mapCoords;
    }
};

#endif
