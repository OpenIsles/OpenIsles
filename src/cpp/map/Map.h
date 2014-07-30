#ifndef _MAP_H
#define _MAP_H

#include <SDL.h>
#include <iostream>
#include <list>
#include <string.h>
#include "Building.h"

class Map {

private:
	/**
	 * @brief Breite der Karte in Kacheln
	 */
	unsigned int width;

	/**
	 * @brief Höhe der Karte in Kacheln
	 */
	unsigned int height;

	/**
	 * @brief Array (height x width) mit den Kacheln
	 */
	unsigned char* tiles = nullptr;

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

public:
	Map(unsigned int width, unsigned int height);
	~Map();

	unsigned int getHeight() const {
		return height;
	}

	unsigned int getWidth() const {
		return width;
	}

	unsigned char getTileAt(unsigned int x, unsigned int y) const {
		if (x >= width || y >= height) {
			std::cerr << "mapCoords (" << std::to_string(x) << ", " + std::to_string(y) << ") out of bounds";
		}

		return tiles[y * width + x];
	}

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
	 * @param object Objekt-Typ, d.h. Index in GraphicsMgr#objects
	 * @return readonly-Zeiger auf das neu angelegte Building
	 */
	const Structure* addStructure(int mapX, int mapY, unsigned char object);

	/**
	 * @brief Fügt ein neues Gebäude der Karte hinzu.
	 * @param mapX X-Map-Koordinate des Objekts
	 * @param mapY Y-Map-Koordinate des Objekts
	 * @param object Objekt-Typ, d.h. Index in GraphicsMgr#objects
	 * @return readonly-Zeiger auf das neu angelegte Building
	 */
	const Building* addBuilding(int mapX, int mapY, unsigned char object);

	/**
	 * @brief Entfernt alle Objekte auf der Karte.
	 */
	void clearMapObjects();

	/**
	 * @brief Rechnet Map- in Screen-Koordinaten um. Die Screen-Koordinaten sind die der oberen linken Ecke der Kachel.
	 * @param mapX Map-X-Koordinate (Eingabe)
	 * @param mapY Map-Y-Koordinate (Eingabe)
	 * @param screenX Screen-X-Koordinate (Ausgabe)
	 * @param screenY Screen-Y-Koordinate (Ausgabe)
	 */
	void mapToScreenCoords(int mapX, int mapY, int& screenX, int& screenY);

	/**
	 * @brief Rechnet Screen- in Map-Koordinaten um.
	 * @param screenX Screen-X-Koordinate (Eingabe)
	 * @param screenY Screen-Y-Koordinate (Eingabe)
	 * @param mapX Map-X-Koordinate (Ausgabe)
	 * @param mapY Map-Y-Koordinate (Ausgabe)
	 */
	void screenToMapCoords(int screenX, int screenY, int& mapX, int& mapY);

	/**
	 * @brief Callback, der sich um einen Mausklick auf die Karte kümmert
	 * @param mouseX X-Koordinate im Fenster, die geklickt wurde
	 * @param mouseY Y-Koordinate im Fenster, die geklickt wurde
	 */
	void onClick(int mouseX, int mouseY);

private:
	/**
	 * @brief Fügt ein neues Map-Objekt der Karte hinzu
	 * @param mapObject Map-Objekt
	 */
	void addMapObject(MapObject* mapObject);

	/**
	 * @brief Initialisiert das tiles-Array neu, wenn die Karte sich ändert.
	 * Es werden alle Objekte von der Karte geräumt, der Speicher (neu) initialisiert, sowie sonstige Zustände resettet.
	 * @param width neue Breite der Karte
	 * @param height neue Höhe der Karte
	 */
	void initNewMap(unsigned int width, unsigned int height);

};

#endif
