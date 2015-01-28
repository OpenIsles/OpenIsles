#ifndef _MAP_H
#define _MAP_H

#include <algorithm>
#include <array>
#include <iostream>
#include <list>
#include <string.h>
#include "Context.h"
#include "config/ConfigMgr.h"
#include "graphics/graphic/Animation.h"
#include "graphics/renderer/IRenderer.h"
#include "gui/GuiMgr.h"
#include "map/Building.h"
#include "map/Isle.h"
#include "utils/Consts.h"
#include "utils/RectangleData.h"

/**
 * @brief Stellt eine Kachel auf der Karte dar. Wir müssen uns mehrere Informationen hierzu merken.
 * Diese Klasse fasst alles zusammen, was wir zu einer Map-Koordinate wissen müssen.
 */
struct MapTile {

private:
    /**
     * @brief Konfigurationsobjekt für diese Gelände-Kachel
     */
    const MapTileConfig* mapTileConfig;

	/**
	 * @brief Animationen, die pro Ansicht für die Kachel gezeichnet werden muss.
     * Der Index entspricht dem ViewIndex in FourDirectionsView.
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
     * @brief Zeiger auf ein MapObject (durch Map.mapObjects verwaltet), das sich auf dieser Kachel befindet
     * oder nullptr, wenns nix da is
     */
    MapObjectFixed* mapObject;
    // TODO aktuell haben wir eh nur fixe Dinger auf der Karte. Die Träger loopen wir aktuell böse durch alles durch, d.h. die sind eh noch nicht an der Kachel.
    // TODO Wir brauchen dann wohl eine Art Liste von beweglichen Dingern, da auch mal mehrere Träger, oder n Träger und n Schaf auf derselben Kachel sein können.

    
    MapTile(const MapTileConfig* mapTileConfig, std::array<const Animation*, 4> tileAnimations) {
		setTile(mapTileConfig, tileAnimations);
        isle = nullptr;
        player = nullptr;
        mapObject = nullptr;
    }

	void setTile(const MapTileConfig* mapTileConfig, std::array<const Animation*, 4> tileAnimations) {
        this->mapTileConfig = mapTileConfig;
        this->tileAnimations = tileAnimations;
	}

	const MapTileConfig* getMapTileConfig() const {
		return mapTileConfig;
	}

	const Animation* getTileAnimationForView(int viewIndex) const {
		return tileAnimations[viewIndex];
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
	 */
	int width;

	/**
	 * @brief Höhe der Karte in Kacheln
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
	 *
	 * Die Objekte sind in dieser Liste immer so geordnet, dass sie in Rendering-Reihenfolge liegen:
	 * "hinten im Bild" liegende Objekte kommen in der Liste zuerst.
	 */
	std::list<MapObject*> mapObjects;

	/**
	 * @brief ausgewähltes Objekt oder @c nullptr, wenn nichts ausgewählt ist
	 */
	MapObject* selectedMapObject = nullptr;

	/**
	 * @brief X-Offset in Screen-Koordinaten, um Scrolling zu ermöglichen
	 */
	int screenOffsetX;

	/**
	 * @brief Y-Offset in Screen-Koordinaten, um Scrolling zu ermöglichen
	 */
	int screenOffsetY;
    
    /**
     * @brief Zoom-Level (1, 2, 4), entspricht Verkleinerungsfaktor
     */
    int screenZoom;

public:
	Map(const Context* const context);
	~Map();

	int getHeight() const {
		return height;
	}

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
	MapObjectFixed* getMapObjectAt(const MapCoords& mapCoords) const;

	const std::list<MapObject*>& getMapObjects() const {
		return mapObjects;
	}

	int getScreenOffsetX() const {
		return screenOffsetX;
	}

	int getScreenOffsetY() const {
		return screenOffsetY;
	}

	void setScreenOffset(int screenOffsetX, int screenOffsetY) {
		this->screenOffsetX = screenOffsetX;
		this->screenOffsetY = screenOffsetY;
	}
    
    int getScreenZoom() const {
        return screenZoom;
    }

    void setScreenZoom(int screenZoom) {
        // screenOffset anpassen, damit um die Bildschirmmitte gezoomt wird
        int zoomFactor = this->screenZoom - screenZoom;
        screenOffsetX += (Consts::mapClipRect.w / 2) * zoomFactor;
        screenOffsetY += (Consts::mapClipRect.h / 2) * zoomFactor;
        
        this->screenZoom = screenZoom;
    }

	const MapObject* getSelectedMapObject() const {
		return selectedMapObject;
	}

    /**
     * @brief Setzt das aktuell gewählte MapObject um.
     *
     * @param selectedMapObject Das neue MapObject, was gewählt sein soll oder `nullptr`, um abzuwählen
     */
	void setSelectedMapObject(MapObject* selectedMapObject);

	/**
	 * @brief Scrollt die Karte
	 * @param screenOffsetX X-Offset in Screen-Koordinaten, in die gescrollt werden soll.
	 *                      Positive Werte scrollen nach rechts, negative nach links
	 * @param screenOffsetY Y-Offset in Screen-Koordinaten, in die gescrollt werden soll.
	 *                      Positive Werte scrollen nach unten, negative nach oben
	 */
	void scroll(int screenOffsetX, int screenOffsetY);

	/**
	 * @brief Fügt ein neues Map-Objekt der Karte hinzu
	 * @param mapObject Map-Objekt
	 */
	void addMapObject(MapObjectFixed* mapObject);
    
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
	 * @return true, wenn ein Weg (Feldweg oder Pflasterstraße) da ist; sonst false.
	 */
	bool isStreetAt(const MapCoords& mapCoords);

private:
    /**
     * @brief Prüft die Map-Koordinaten auf ihre Gültigkeit, ob sie außerhalb der Karte liegen.
     * @param mapCoords Map-Koordianten
     * @return true, wenn die Koordinaten gültig sind; false, wenn sie außerhalb der Karte liegen.
     */
    bool checkMapCoords(const MapCoords& mapCoords) const;
    
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
