#include "GraphicsMgr.h"
#include "Map.h"

/**
 * Die Karte stellt sich wiefolgt dar:
 *
 *                 y=0
 *             --/  (0,0)   x=0
 *       y <--/      ---        \--
 *                --/   \--        \--> x
 *             --/         \--
 *          --/       x       \--(w-1,0)
 *    (0,h-1)  --\         /--
 *                --\   /--
 *                   ---
 *                (w-1,h-1)
 *
 *
 * Koordinatensysteme:
 *   Obenstehend sind die Kachel- oder Map-Koordinaten (mapCoords) dargestellt.
 *   Bildschirm-Koordinaten (screenCoords) sind durch ein Pixel-Koordinatensystem definiert. Hierbei wird die
 *   gesamte Karte um die Bildschirm-Koordinaten (0, 0) zentriert. Im Bild oben durch den Punkt x dargestellt.
 *   Die Bildschirm-Koordinate einer Kachel bestimmt die Position des Pixels in der Mitte der Kachel.
 */

// Aus main.cpp importiert
extern GraphicsMgr* graphicsMgr;

Map::Map(unsigned int width, unsigned int height) :
		width(width), height(height) {

	tiles = new unsigned char[width * height];

	// TODO Karte laden statt einfach nur statisch eine Insel erzeugen
	for (unsigned int y = 0, i = 0; y < height; y++) {
		for (unsigned int x = 0; x < width; x++, i++) {
			if (x < 3 || y < 3 || x >= width - 3 || y >= height - 3) {
				tiles[i] = 0;
			} else {
				tiles[i] = ((x + y) % 2 == 0) ? 1 : 2;
			}
		}
	}

	MapObject* dummyMapObject = new MapObject { 3, 4, 0 };
	mapObjects.push_front(dummyMapObject);

	dummyMapObject = new MapObject { 6, 9, 0 };
	mapObjects.push_front(dummyMapObject);

	dummyMapObject = new MapObject { 11, 10, 1 };
	mapObjects.push_front(dummyMapObject);

	screenOffsetX = 0;
	screenOffsetY = 0;
}

Map::~Map() {
	for (auto iter = mapObjects.cbegin(); iter != mapObjects.cend(); iter++) {
		MapObject* mapObject = *iter;
		delete mapObject;
	}
	mapObjects.clear();

	delete[] tiles;
}

void Map::mapToScreenCoords(unsigned int mapX, unsigned int mapY, int& screenX, int& screenY) {
	// TODO muss sich die Anwendung irgendwie merken vbzw. aus dem Fenster holen (SDL_GetWindowSize(SDL_Window*,...))
	const int windowWidth = 1024;
	const int windowHeight = 768;

	// Koordinatensystem wechseln
	screenX = mapX * (GraphicsMgr::TILE_WIDTH / 2) - mapY * (GraphicsMgr::TILE_WIDTH / 2);
	screenY = mapX * (GraphicsMgr::TILE_HEIGHT / 2) + mapY * (GraphicsMgr::TILE_HEIGHT / 2);

	// Kachelmittelpunkt als Referenzpunkt nehmen
	screenX -= (GraphicsMgr::TILE_WIDTH / 2);
	screenY -= (GraphicsMgr::TILE_HEIGHT / 2);

	// Im Fenster zentrieren
	screenX += (windowWidth / 2);
	screenY += (windowHeight / 2);

	// Scrolling-Offset anwenden
	screenX -= screenOffsetX;
	screenY -= screenOffsetY;
}

void Map::renderMap(SDL_Renderer* renderer) {
	// Kacheln rendern
	SDL_Rect rectDestination = { 0, 0, GraphicsMgr::TILE_WIDTH, GraphicsMgr::TILE_HEIGHT };
	for (unsigned int mapY = 0; mapY < height; mapY++) {
		for (unsigned int mapX = 0; mapX < width; mapX++) {
			mapToScreenCoords(mapX, mapY, rectDestination.x, rectDestination.y);
			SDL_RenderCopy(renderer, graphicsMgr->getTileTexture(getTileAt(mapX, mapY)), NULL, &rectDestination);
		}
	}

	// Objekte rendern
	// TODO Reihenfolge muss entsprechend von hinten nach vorne passieren und nicht wild durcheinander
	for (auto iter = mapObjects.cbegin(); iter != mapObjects.cend(); iter++) {
		MapObject* mapObject = *iter;

		// TODO Objekte haben unterschiedliche Größen. Das müssen wir auch handeln.
		// Aktuell einfach mal fix für die beiden Grafiken.
		SDL_Rect rect = { 0, 0, 128, 128 };
		mapToScreenCoords(mapObject->mapX, mapObject->mapY, rect.x, rect.y);

		SDL_RenderCopy(renderer, graphicsMgr->getObjectTexture(mapObject->object), NULL, &rect);
	}

	// Bildfläche anzeigen
	SDL_RenderPresent(renderer);
}

void Map::scroll(int screenOffsetX, int screenOffsetY) {
	this->screenOffsetX += screenOffsetX;
	this->screenOffsetY += screenOffsetY;
}
