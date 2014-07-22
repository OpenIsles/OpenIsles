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
 *          --/               \--(w-1,0)
 *    (0,h-1)  --\         /--
 *                --\   /--
 *                   ---
 *                (w-1,h-1)
 *
 *
 * Koordinatensysteme:
 *   Obenstehend sind die Kachel- oder Map-Koordinaten (mapCoords) dargestellt.
 *   Bildschirm-Koordinaten (screenCoords) sind durch ein Pixel-Koordinatensystem definiert. Hierbei wird die
 *   Kachel mit mapCoords (0, 0) auf screenCoords (0, 0) gelegt.
 *   Die Bildschirm-Koordinate einer Kachel bestimmt die Position des Pixels links-oben an der Kachel.
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

	addMapObject(3, 3, 0);
	addMapObject(6, 3, 1);
	addMapObject(9, 3, 2);
	addMapObject(12, 3, 3);
	addMapObject(15, 3, 4);
	addMapObject(18, 3, 5);

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
	// Koordinatensystem wechseln
	screenX = (mapX - mapY) * (GraphicsMgr::TILE_WIDTH / 2);
	screenY = (mapX + mapY) * (GraphicsMgr::TILE_HEIGHT / 2);
}

void Map::renderMap(SDL_Renderer* renderer) {
	// Kacheln rendern
	SDL_Rect rectDestination = { 0, 0, GraphicsMgr::TILE_WIDTH, GraphicsMgr::TILE_HEIGHT };
	for (unsigned int mapY = 0; mapY < height; mapY++) {
		for (unsigned int mapX = 0; mapX < width; mapX++) {
			mapToScreenCoords(mapX, mapY, rectDestination.x, rectDestination.y);

			// Scrolling-Offset anwenden
			rectDestination.x -= screenOffsetX;
			rectDestination.y -= screenOffsetY;

			SDL_RenderCopy(renderer, graphicsMgr->getTile(getTileAt(mapX, mapY))->getTexture(), NULL, &rectDestination);
		}
	}

	// Reihenfolge der Objekte so stellen, dass von hinten nach vorne gerendert wird
	// TODO ggf. Algorithmus verbessern, dass wirklich nach Y-Screen-Koordinaten sortiert wird. Mit den paar Grafiken
	// hab ich keinen Fall bauen können, der n Unterschied macht.
	mapObjects.sort([] (MapObject* mo1, MapObject* mo2) {
		if (mo1->mapY < mo2->mapY) {
			return true;
		} else if (mo1->mapY > mo2->mapY) {
			return false;
		} else {
			return (mo1->mapX <= mo2->mapX);
		}
	});

	// Objekte rendern
	for (auto iter = mapObjects.cbegin(); iter != mapObjects.cend(); iter++) {
		MapObject* mapObject = *iter;
		Graphic* graphic = graphicsMgr->getObject(mapObject->object);
		SDL_Rect rect = SDL_Rect();
		rect.x = mapObject->screenX - screenOffsetX;
		rect.y = mapObject->screenY - screenOffsetY;
		rect.w = graphic->getWidth();
		rect.h = graphic->getHeight();

		SDL_Texture* objectTexture = graphic->getTexture();
		SDL_SetTextureAlphaMod(objectTexture, 255);
		SDL_SetTextureColorMod(objectTexture, 255, 255, 255);
		SDL_RenderCopy(renderer, objectTexture, NULL, &rect);
	}

	// Bildfläche anzeigen
	SDL_RenderPresent(renderer);
}

void Map::scroll(int screenOffsetX, int screenOffsetY) {
	this->screenOffsetX += screenOffsetX;
	this->screenOffsetY += screenOffsetY;
}

const MapObject* Map::addMapObject(unsigned int mapX, unsigned int mapY, unsigned char object) {
	// Position berechnen in Screen-Koordinaten berechnen, an dem sich die Grafik befinden muss.
	Graphic* graphic = graphicsMgr->getObject(object);
	SDL_Rect rect = { 0, 0, graphic->getWidth(), graphic->getHeight() };
	mapToScreenCoords(mapX, mapY, rect.x, rect.y);

	// Grafik an die richtige Stelle schieben. Das muss ausgehend von der zu belegenden Tile-Fläche berechnet werden.
	rect.x -= graphic->getWidth() - (graphic->getMapWidth() + 1) * (GraphicsMgr::TILE_WIDTH / 2);
	rect.y -= graphic->getHeight()
			- (graphic->getMapWidth() + graphic->getMapHeight()) * (GraphicsMgr::TILE_HEIGHT / 2);

	// Objekt anlegen und in die Liste aufnehmen
	MapObject* newMapObject = new MapObject();
	newMapObject->mapX = mapX;
	newMapObject->mapY = mapY;
	newMapObject->object = object;
	newMapObject->screenX = rect.x;
	newMapObject->screenY = rect.y;

	mapObjects.push_front(newMapObject);
	return newMapObject;
}
