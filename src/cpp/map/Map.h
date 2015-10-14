#ifndef _MAP_H
#define _MAP_H

#include <algorithm>
#include <array>
#include <iostream>
#include <list>
#include <string.h>
#include "Context.h"
#include "defines.h"
#include "config/ConfigMgr.h"
#include "graphics/graphic/Animation.h"
#include "graphics/renderer/IRenderer.h"
#include "gui/GuiMgr.h"
#include "map/coords/MapCoords.h"
#include "map/coords/ScreenCoords.h"
#include "map/Building.h"
#include "map/Isle.h"
#include "map/MapObject.h"
#include "utils/Consts.h"
#include "utils/RectangleData.h"

/**
 * @brief Stellt eine Kachel auf der Karte dar. Wir müssen uns mehrere Informationen hierzu merken.
 * Diese Klasse fasst alles zusammen, was wir zu einer Map-Koordinate wissen müssen.
 */
struct MapTile {

public:
    /**
     * @brief [Zeitspanne](@ref gameTicks), die eine Map-Kachel nach dem "Abernten" braucht, um sich zu regenerieren.
     */
    static const unsigned long HARVEST_REFRESH_TICKS = 1000 * 40;

private:
    /**
     * @brief Konfigurationsobjekt für diese Gelände-Kachel
     */
    const MapTileConfig* mapTileConfig;

    /**
     * @brief Animationen, die pro Ansicht für die Kachel gezeichnet werden muss.
     * Der Index entspricht EighthDirection/2.
     */
    std::array<const Animation*, 4> tileAnimations;

public:
    /**
     * @brief Zeiger auf die Insel (durch Map.isles verwaltet), die sich an dieser Stelle befindet
     * oder nullptr, wenn dort Ozean ist.
     */
    Isle* isle;
    
    /**
     * @brief Zeiger auf den Spieler (durch Game.players verwaltet), dem die Kachel Land gehört
     * oder nullptr, wenns keinem gehört
     */
    Player* player;

    /**
     * @brief Zeiger auf ein MapObjectFixed (durch Map.mapObjects verwaltet), das sich auf dieser Kachel befindet
     * oder nullptr, wenns nix da is.
     * Es kann sich immer nur maximal ein fixes Objekt (Gebäude) auf der Kachel befinden. Träger sind in
     * `mapObjectsMoving`.
     */
    MapObjectFixed* mapObjectFixed;

    /**
     * @brief Liste von Zeigern auf MapObjectMoving (durch Map.mapObjects verwaltet), die sich auf dieser Kachel
     * befinden.
     * Es können mehrere Objekte auf derselben Kachel sein.
     */
    std::list<MapObjectMoving*> mapObjectsMoving;
    // TODO Bei Bewegung von Träger wird das noch nicht aktualisiert. Wir brauchen die Referenz dann von mehreren Kacheln aus, wenn sich ein Objekt "dazwischen" befindet.

    /**
     * @brief [Zeitpunkt](@ref gameTicks), wann diese Map-Kachel das nächste Mal "abgeerntet" werden kann.
     *
     * Map-Kacheln haben natürliche Resourcen, die von bestimmten Objekten abgeerntet werden können, z.&nbsp;B.
     * - Rinder und Schafe fressen auf einer Gras-Kachel und produzieren damit Schlachtvieh und Wolle.
     * - Fischerboote fangen auf einer Wasser-Kachel Fische und produzieren damit Nahrung.
     */
    unsigned long nextHarvestTicks;

    /**
     * @brief Flag, um zu markieren, dass bereits jemand auf dieser Kachel ist, der aberntet bzw. dorthin unterwegs
     * ist. Hiermit wird verhindert, dass zwei oder noch mehr Träger zur selben Kachel laufen.
     */
    bool harvestBusy;

    
    MapTile(const MapTileConfig* mapTileConfig, std::array<const Animation*, 4> tileAnimations) {
        setTile(mapTileConfig, tileAnimations);
        isle = nullptr;
        player = nullptr;
        mapObjectFixed = nullptr;

        nextHarvestTicks = 0; // am Anfang kann man immer sofort ernten
        harvestBusy = false;
    }

    void setTile(const MapTileConfig* mapTileConfig, std::array<const Animation*, 4> tileAnimations) {
        this->mapTileConfig = mapTileConfig;
        this->tileAnimations = tileAnimations;
    }

    const MapTileConfig* getMapTileConfig() const {
        return mapTileConfig;
    }

    const Animation* getTileAnimationForView(int viewIndex) const {
        return tileAnimations[viewIndex / 2];
    }
};



/**
 * @brief Stellt die Datenstruktur der Karte dar, die alle Inseln und Objekte in der Welt enthält.
 *
 * Diese Klasse enthält auch die Einstellungen, welcher Scroll-Offset und welche Zoomstufe der Spieler eingestellt hat.
 * TODO Später müssen diese Infos am Spieler selber hängen und den Zeichenprozeduren als Parameter übergeben werden,
 * wenn wir ein Mehr(mensch)spieler-Game machen wollen.
 */
class Map : public ContextAware {

    friend class GameIO; // GameIO soll zum Laden/Speichern einfach auf die Karte zugreifen können

private:
    /**
     * @brief Breite der Karte in Kacheln
     * TODO Aktuell gehen wir davon aus, das immer gilt: width == height
     */
    int width;

    /**
     * @brief Höhe der Karte in Kacheln
     * TODO Aktuell gehen wir davon aus, das immer gilt: width == height
     */
    int height;
    
    /**
     * @brief Daten aller Kacheln auf der Karte
     */
    RectangleData<MapTile*>* mapTiles = nullptr;

    /**
     * @brief Liste der Inseln auf der Karte
     */
    std::list<Isle*> isles;

    /**
     * @brief Liste von Objekten (z.B. Häusern) auf der Karte.
     */
    std::list<MapObject*> mapObjects;

    /**
     * @brief ausgewähltes Objekt oder @c nullptr, wenn nichts ausgewählt ist
     */
    const MapObject* selectedMapObject = nullptr;

    /**
     * @brief Map-Koordinaten, die sich aktuell in der Mitte befinden
     */
    MapCoords mapCoordsCentered;
    
    /**
     * @brief Zoom-Level (1, 2, 4), entspricht Verkleinerungsfaktor
     */
    int screenZoom;

    /**
     * @brief Richtung aus der die Karte betrachtet wird
     */
    FourthDirection screenView;

public:
    Map(const Context* const context);

    VIRTUAL_ONLY_IN_TESTS
    ~Map();

    VIRTUAL_ONLY_IN_TESTS
    int getHeight() const {
        return height;
    }

    VIRTUAL_ONLY_IN_TESTS
    int getWidth() const {
        return width;
    }

    /**
     * @brief Liefert das MapTile (Infos zu einer Kachel) an einer bestimmten Stelle zurück
     * @param mapCoords Map-Koordianten
     * @return Zeiger auf MapTile oder nullptr, wenn die Koordinaten außerhalb der Karte sind
     */
    MapTile* getMapTileAt(const MapCoords& mapCoords) const;

    /**
     * @brief Liefert das Map-Objekt zurück, die sich an den angegebenen Koordinaten befindet.
     * @param mapCoords Map-Koordianten
     * @return Zeiger auf das Map-Objekt oder nullptr, wenn dort kein Map-Objekt ist
     */
    MapObjectFixed* getMapObjectFixedAt(const MapCoords& mapCoords) const;

    const std::list<MapObject*>& getMapObjects() const {
        return mapObjects;
    }

    const MapCoords& getMapCoordsCentered() const {
        return mapCoordsCentered;
    }

    void setMapCoordsCentered(const MapCoords& mapCoordsCentered) {
        this->mapCoordsCentered = mapCoordsCentered;
    }

    int getScreenZoom() const {
        return screenZoom;
    }

    void setScreenZoom(int screenZoom) {
        this->screenZoom = screenZoom;
    }

    const FourthDirection& getScreenView() const {
        return screenView;
    }

    void setScreenView(const FourthDirection& screenView) {
        this->screenView = screenView;
    }

    void rotateViewClockwise() {
        Direction::rotate90DegreesClockwise(this->screenView);
    }

    void rotateViewCounterclockwise() {
        Direction::rotate90DegreesCounterclockwise(this->screenView);
    }

    const MapObject* getSelectedMapObject() const {
        return selectedMapObject;
    }

    /**
     * @brief Setzt das aktuell gewählte MapObject um.
     *
     * @param selectedMapObject Das neue MapObject, was gewählt sein soll oder `nullptr`, um abzuwählen
     */
    void setSelectedMapObject(const MapObject* selectedMapObject);

    /**
     * @brief Scrollt die Karte.
     *
     * Die Koordinaten werden in Südansicht angegeben. Befindet sich die Karte in
     * einer anderen Ansicht, werden sie entsprechend konveriert.
     * D.h. Ein Scrollen um (1, 0) in Südansicht scrollt die Karte nach rechts,
     * während in Nordansicht die Karte nach links scrollt.
     *
     * @param xDelta X-Offset in Map-Koordinaten, in die gescrollt werden soll.
     *               Positive Werte scrollen nach rechts, negative nach links.
     * @param yDelta Y-Offset in Map-Koordinaten, in die gescrollt werden soll.
     *               Positive Werte scrollen nach unten, negative nach oben.
     */
    void scroll(int xDelta, int yDelta);

    /**
     * @brief Fügt ein neues Map-Objekt der Karte hinzu
     * @param mapObject Map-Objekt
     */
    void addMapObject(MapObject* mapObject);

    /**
     * @brief Ändert die Position eines beleglichen Map-Objekts. Der Aufruf dieser Methode ist wichtig, um die MapTile-
     * Struktur aktuell zu halten
     *
     * @param mapObject Map-Objekt
     * @param newMapCoords neue Map-Koordinaten, die das Map-Objekt erhalten soll
     */
    void moveMapObject(MapObjectMoving* mapObject, const DoubleMapCoords newMapCoords);

    /**
     * @brief Entfernt ein Map-Objekt von der Karte
     * @param mapObject Map-Objekt
     */
    void deleteMapObject(MapObject* mapObject);
    
    /**
     * @brief Aktualisiert die internen Strukturen der Karte (mapTiles) beim Neuerrichten eines Kontors
     * @param building Kontor-Gebäude
     */
    void addOfficeCatchmentAreaToMap(const Building& building);

    /**
     * @brief Räumt alles weg und gibt Speicher wieder frei
     */
    void clearMap();

    /**
     * @brief Ermittelt, ob an einer bestimmten Stelle ein Weg liegt.
     * @param mapCoords Map-Koordianten
     * @return `true`, wenn ein Weg (Feldweg oder Pflasterstraße) da ist; sonst `false`.
     */
    bool isStreetAt(const MapCoords& mapCoords) const;

    /**
     * @brief Ermittelt, ob ein Marktkarren eine bestimmte Stelle befahren darf.
     * @param mapCoords Map-Koordianten
     * @return `true`, wenn ein Marktkarren dort fahren darf, d.&nbsp;h. ein Feldweg, Pflasterstraße oder Platz da ist;
     * sonst `false`.
     */
    bool isWalkableForCartAt(const MapCoords& mapCoords) const;

private:
    /**
     * @brief Initialisiert alles, wenn die Karte sich ändert.
     * Es werden alle Objekte von der Karte geräumt, der Speicher und die Map-Breite/Höhe (neu) initialisiert, 
     * sowie sonstige Zustände resettet.
     * @param newWidth neue Breite der Karte
     * @param height neue Höhe der Karte
     */
    void initNewMap(int newWidth, int newHeight);

};

#endif
