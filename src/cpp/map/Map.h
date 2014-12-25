#ifndef _MAP_H
#define _MAP_H

#include <algorithm>
#include <iostream>
#include <list>
#include <string.h>
#include "Context.h"
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

    /**
     * @brief Index der Gelände-Kachel
     */
    unsigned char tileIndex;

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
    MapObject* mapObject;

    
    MapTile() {
        // TODO in die Config bringen. Aktuell is aber noch unklar, ob wir mehrere isOcean-Kacheln haben werden
        tileIndex = 14; // Ozean
        isle = nullptr;
        player = nullptr;
        mapObject = nullptr;
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
     * @param mapX Map-X-Koordiante
     * @param mapY Map-Y-Koordiante
     * @return Zeiger auf MapTile oder nullptr, wenn die Koordinaten außerhalb der Karte sind
     */
	MapTile* getMapTileAt(int mapX, int mapY) const;

	/**
	 * @brief Liefert das Map-Objekt zurück, die sich an den angegebenen Koordinaten befindet.
	 * @param mapX Map-X-Koordiante
	 * @param mapY Map-Y-Koordiante
	 * @return Zeiger auf das Map-Objekt oder nullptr, wenn dort kein Map-Objekt ist
	 */
	MapObject* getMapObjectAt(int mapX, int mapY) const;

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
	void addMapObject(MapObject* mapObject);
    
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
	 * @param mapX Map-X-Koordinate
	 * @param mapY Map-Y-Koordinate
	 * @return true, wenn ein Weg (Feldweg oder Pflasterstraße) da ist; sonst false.
	 */
	bool isStreetAt(int mapX, int mapY);

private:
    /**
     * @brief Prüft die Map-Koordinaten auf ihre Gültigkeit, ob sie außerhalb der Karte liegen.
     * @param mapX Map-X-Koordiante
     * @param mapY Map-Y-Koordiante
     * @return true, wenn die Koordinaten gültig sind; false, wenn sie außerhalb der Karte liegen.
     */
    bool checkMapCoords(int mapX, int mapY) const;
    
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
