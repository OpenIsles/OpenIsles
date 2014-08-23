#ifndef _MAP_H
#define _MAP_H

#include <SDL.h>
#include <iostream>
#include <list>
#include <string.h>
#include "map/Building.h"
#include "map/Isle.h"
#include "utils/RectangleData.h"

// Konstanten für isAllowedToPlaceStructure()

/**
 * @brief Setzen der Struktur ist erlaubt
 */
#define PLACING_STRUCTURE_ALLOWED         0

/**
 * @brief Setzen der Struktur ist nicht erlaubt wegen Resourcen-Mangel.
 * Grafisch wird dies dargestellt, indem das Gebäude blickt.
 */
#define PLACING_STRUCTURE_NO_RESOURCES    (1 << 0)

/**
 * @brief Setzen der Struktur ist hier nicht erlaubt. Das kann sein, weil was im Weg ist
 * oder das Gebiet nicht erschlossen ist. Grafisch wird dies dargestellt, indem die Struktur rot gezeichnet wird
 */
#define PLACING_STRUCTURE_NO_ROOM         (1 << 1)

/**
 * @brief Setzen der Struktur ist nicht erlaubt, da wir mitten auf dem Ozean außerhalb einer Insel sind.
 * Grafisch wird dies dargestellt, indem gar nix gezeichnet wird.
 */
#define PLACING_STRUCTURE_OUTSIDE_OF_ISLE (1 << 2)



/**
 * @brief Stellt eine Kachel auf der Karte dar. Wir müssen uns mehrere Informationen hierzu merken.
 * Diese Klasse fasst alles zusammen, was wir zu einer Map-Koordinate wissen müssen.
 */
struct MapTile {
    
    /**
     * @brief Index in Graphics.tiles, welche Grafik die Kachel hat
     */
    unsigned char tileGraphicIndex;
    
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
    
    
    MapTile() {
        // TODO Konstante/Enum
        tileGraphicIndex = 1; // Ozean
        isle = nullptr;
        player = nullptr;
    }

};



/**
 * @brief Stellt die Karte dar
 */
class Map {

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
     * @brief SDL-Texture mit der Minimap. Diese wird vorberechnet und hier gespeichert, sodass sie nur gezeichnet
     * werden muss und keine Berechnungszeit anfällt.
     */
    SDL_Texture* minimapTexture = nullptr;

public:
	Map();
	~Map();

	int getHeight() const {
		return height;
	}

	int getWidth() const {
		return width;
	}

    /**
     * @brief Liefert die Kachel an einer bestimmten Stelle zurück
     * @param mapX Map-X-Koordiante
     * @param mapY Map-Y-Koordiante
     * @return Kachel-Index
     */
	unsigned char getTileAt(int mapX, int mapY) const;

	const std::list<MapObject*>& getMapObjects() const {
		return mapObjects;
	}

	int getScreenOffsetX() const {
		return screenOffsetX;
	}

	int getScreenOffsetY() const {
		return screenOffsetY;
	}

	const MapObject* getSelectedMapObject() const {
		return selectedMapObject;
	}

	void setSelectedMapObject(MapObject* selectedMapObject) {
		this->selectedMapObject = selectedMapObject;
	}

	/**
	 * @brief Lädt eine Karte von einer tiled-TMX-Datei
	 * @param filename Dateiname der zu ladenden Karte
	 */
	void loadMapFromTMX(const char* filename);

	/**
	 * @brief Rendert die Karte.
	 * @param renderer SDL-Renderer, auf den gezeichnet wird
	 */
	// TODO durch Graphics- oder Render-Manager abstrahieren, damit die Karte nicht die Grafiken kennen muss
	void renderMap(SDL_Renderer* renderer);
    
    /**
	 * @brief Rendert die Minimap
	 * @param renderer SDL-Renderer, auf den gezeichnet wird
	 */
    void renderMinimap(SDL_Renderer* renderer);

	/**
	 * @brief Scrollt die Karte
	 * @param screenOffsetX X-Offset in Screen-Koordinaten, in die gescrollt werden soll.
	 *                      Positive Werte scrollen nach rechts, negative nach links
	 * @param screenOffsetY Y-Offset in Screen-Koordinaten, in die gescrollt werden soll.
	 *                      Positive Werte scrollen nach unten, negative nach oben
	 */
	void scroll(int screenOffsetX, int screenOffsetY);

	/**
	 * @brief Fügt eine neue Struktur der Karte hinzu.
	 * @param mapX X-Map-Koordinate des Objekts
	 * @param mapY Y-Map-Koordinate des Objekts
	 * @param structureType Typ der Struktur
     * @param player Spieler, dem die Struktur gehören soll
	 * @return readonly-Zeiger auf das neu angelegte Building
	 */
	const Structure* addStructure(int mapX, int mapY, StructureType structureType, Player* player);

	/**
	 * @brief Fügt ein neues Gebäude der Karte hinzu.
	 * @param mapX X-Map-Koordinate des Objekts
	 * @param mapY Y-Map-Koordinate des Objekts
	 * @param structureType Typ des Gebäudes
     * @param player Spieler, dem das Gebäude gehören soll
	 * @return readonly-Zeiger auf das neu angelegte Building
	 */
	const Building* addBuilding(int mapX, int mapY, StructureType structureType, Player* player);
    
    /**
     * @brief Aktualisiert die internen Strukturen der Karte (mapTiles und Minimap) beim Neuerrichten eines Kontors
     * @param building Kontor-Gebäude
     */
    void addOfficeCatchmentAreaToMap(const Building& building);

	/**
	 * @brief Räumt alles weg und gibt Speicher wieder frei
	 */
	void clearMap();

	/**
	 * @brief Callback, der sich um einen Mausklick auf die Karte kümmert
	 * @param mouseX X-Koordinate im Fenster, die geklickt wurde
	 * @param mouseY Y-Koordinate im Fenster, die geklickt wurde
	 */
	void onClick(int mouseX, int mouseY);

private:
    /**
     * @brief Prüft die Map-Koordinaten auf ihre Gültigkeit, ob sie außerhalb der Karte liegen.
     * @param mapX Map-X-Koordiante
     * @param mapY Map-Y-Koordiante
     * @return true, wenn die Koordinaten gültig sind; false, wenn sie außerhalb der Karte liegen.
     */
    bool checkMapCoords(int mapX, int mapY) const;
    
    /**
     * @brief Liefert die Insel zurück, die sich an den angegebenen Koordinaten befindet.
     * @param mapX Map-X-Koordiante
     * @param mapY Map-Y-Koordiante
     * @return Zeiger auf die Insel oder nullptr, wenn dort keine Insel ist
     */
    Isle* getIsleAt(int mapX, int mapY) const;
    
    /**
     * @brief Liefert das Map-Objekt zurück, die sich an den angegebenen Koordinaten befindet.
     * @param mapX Map-X-Koordiante
     * @param mapY Map-Y-Koordiante
     * @return Zeiger auf das Map-Objekt oder nullptr, wenn dort kein Map-Objekt ist
     */
    MapObject* getMapObjectAt(int mapX, int mapY) const;
    
	/**
	 * @brief Fügt ein neues Map-Objekt der Karte hinzu
	 * @param mapObject Map-Objekt
	 */
	void addMapObject(MapObject* mapObject);

	/**
	 * @brief Initialisiert alles, wenn die Karte sich ändert.
	 * Es werden alle Objekte von der Karte geräumt, der Speicher und die Map-Breite/Höhe (neu) initialisiert, 
     * sowie sonstige Zustände resettet.
	 * @param newWidth neue Breite der Karte
	 * @param height neue Höhe der Karte
	 */
	void initNewMap(int newWidth, int newHeight);
    
    /**
     * @brief Aktualisiert die SDL-Texture für die Minimap
     */
    void updateMinimapTexture();
    
    /**
     * Prüft, ob eine bestimmte Struktur an eine bestimmte Position gesetzt werden darf.
     * 
     * @param mapX Map-X-Koordinate, wo die Struktur gesetzt werden soll
     * @param mapY Map-Y-Koordinate, wo die Struktur gesetzt werden soll
     * @param structureType Typ der zu setzenden Struktur
     * @sa PLACING_STRUCTURE-Konstanten
     * @return Bitmaske, die angibt, ob das Gebäude gesetzt werden darf.
     */
    unsigned char isAllowedToPlaceStructure(int mapX, int mapY, StructureType structureType);
    
    /**
     * Render eine Struktur. Hilfsmethode von renderMap().
     * 
     * @param structure Struktur
     * @param rect Rechteck mit Screen-Koordinaten, wo die Grafik gesetzt werden soll
     * @param masked true, um die Grafik maskiert (für Gebäudeplatzierung) zu zeichnen
     * @param redAndSemiTransparent true, um die Grafik rot einzufärben und halb-durchsichtig zu machen
     * @param blink true, um die Grafik blinken zu lassen, d.h. entweder wird die Grafik gezeichnet oder nicht
     */
    void renderStructure(Structure* structure, SDL_Rect* rect, bool masked, bool redAndSemiTransparent, bool blink);
    
    /**
	 * @brief interner Klickhandler, wenn in die Karte geklickt wurde. Diese Methode wird garantiert nur aufgerufen,
     * wenn die Koordinaten in der Karte liegen.
	 * @param mouseX X-Fenster-Koordinate, die geklickt wurde
	 * @param mouseY Y-Fenster-Koordinate, die geklickt wurde
	 */
	void onClickInMap(int mouseX, int mouseY);
    
    /**
	 * @brief interner Klickhandler, wenn in die Karte geklickt wurde, während wir im "Gebäude positionieren"-Modus
     * sind.
	 * @param mapX X-Map-Koordinate, wo geklickt wurde
	 * @param mapY Y-Map-Koordinate, wo geklickt wurde
	 */
    void onClickInMapWhileAddingStructure(int mapX, int mapY);
    
    /**
	 * @brief interner Klickhandler, wenn in die Minimap geklickt wurde. Diese Methode wird garantiert nur aufgerufen,
     * wenn die Koordinaten in der Minimap liegen.
	 * @param mouseX X-Fenster-Koordinate, die geklickt wurde
	 * @param mouseY Y-Fenster-Koordinate, die geklickt wurde
	 */
	void onClickInMinimap(int mouseX, int mouseY);

};

#endif
