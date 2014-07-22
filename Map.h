#ifndef _MAP_H
#define _MAP_H

#include <SDL.h>
#include <iostream>
#include <list>
#include <string.h>

typedef struct {
	/**
	 * @brief X-Map-Koordinate des Objekts
	 */
	unsigned int mapX;

	/**
	 * @brief Y-Map-Koordinate des Objekts
	 */
	unsigned int mapY;

	/**
	 * @brief Objekt-Typ, d.h. Index in GraphicsMgr#objects
	 */
	unsigned char object;

	/**
	 * @brief berechnete X-Screen-Koordinate, an dem die Grafik gezeichnet werden muss
	 */
	int screenX;

	/**
	 * @brief berechnete X-Screen-Koordinate, an dem die Grafik gezeichnet werden muss
	 */
	int screenY;

} MapObject;

class Map {

private:
	/**
	 * @brief Breite der Karte in Kacheln
	 */
	unsigned int width;

	/**
	 * @brief Breite der Karte in Kacheln
	 */
	unsigned int height;

	/**
	 * @brief Array (height x width) mit den Kacheln
	 */
	unsigned char* tiles;

	/**
	 * @brief Liste von Objekten (z.B. Häusern) auf der Karte
	 */
	std::list<MapObject*> mapObjects;

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
	 * @brief Fügt ein neues Objekt der Karte hinzu.
	 * @param mapX X-Map-Koordinate des Objekts
	 * @param mapY Y-Map-Koordinate des Objekts
	 * @param object Objekt-Typ, d.h. Index in GraphicsMgr#objects
	 * @return readonly-Zeiger auf das neu angelegte MapObject
	 */
	const MapObject* addMapObject(unsigned int mapX, unsigned int mapY, unsigned char object);

private:
	/**
	 * @brief Rechnet Map- in Screen-Koordinaten um
	 * @param mapX Map-X-Koordinate (Eingabe)
	 * @param mapY Map-Y-Koordinate (Eingabe)
	 * @param screenX Screen-X-Koordinate (Ausgabe)
	 * @param screenY Screen-Y-Koordinate (Ausgabe)
	 */
	void mapToScreenCoords(unsigned int mapX, unsigned int mapY, int& screenX, int& screenY);

};

#endif
