#include "GraphicsMgr.h"
#include "Map.h"
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"

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
extern const int windowWidth;
extern const int windowHeight;

Map::Map(unsigned int width, unsigned int height) :
		width(width), height(height) {

	initNewMap(width, height);

	loadMapFromTMX("data/map/map.tmx");

	addMapObject(3, 3, 0);
	addMapObject(6, 3, 1);
	addMapObject(9, 3, 2);
	addMapObject(12, 3, 3);
	addMapObject(15, 3, 4);
	addMapObject(18, 3, 5);
}

Map::~Map() {
	selectedMapObject = nullptr;

	clearMapObjects();

	delete[] tiles;
	tiles = nullptr;
}

void Map::initNewMap(unsigned int width, unsigned int height) {
	// Karte erst leerräumen
	clearMapObjects();

	// Neue Karte anlegen
	if (tiles != nullptr) {
		delete[] tiles;
	}
	tiles = new unsigned char[width * height];
	this->width = width;
	this->height = height;

	// Sonstiges Zeugs auf Anfangswerte stellen
	selectedMapObject = nullptr;

	screenOffsetX = 0;
	screenOffsetY = 0;
}

void Map::mapToScreenCoords(int mapX, int mapY, int& screenX, int& screenY) {
	screenX = (mapX - mapY) * GraphicsMgr::TILE_WIDTH_HALF;
	screenY = (mapX + mapY) * GraphicsMgr::TILE_HEIGHT_HALF;
}

void Map::screenToMapCoords(int screenX, int screenY, int& mapX, int& mapY) {
	/*
	 * Screen-Koordinaten erst in ein (TILE_WIDTH x TILE_HEIGHT)-Koordinatensystem runterrechnen
	 * Dann gibts 8 Fälle und wir haben es. Ohne unperformante Matrizen und Projektionen :-)
	 *
	 * |          TILE_WIDTH           |
	 * |   xDiff<0.5       xDiff>0.5   |
	 * O-------------------------------O-------------
	 * | x-1          /|\          y-1 |
	 * |   [1]   /---- | ----\   [6]   | yDiff<0.5
	 * |    /----  [2] | [5]  ----\    |
	 * |---------------+---------------|  TILE_HEIGHT
	 * |    \----  [4] | [7]  ----/    |
	 * |   [3]   \---- | ----/   [8]   | yDiff>0.5
	 * | y+1          \|/          x+1 |
	 * O-------------------------------O-------------
	 *
	 * O = obere linke Ecke der Kacheln = Punkt, der mit mapToScreenCoords() berechnet wird.
	 *     Entspricht genau den (TILE_WIDTH x TILE_HEIGHT)-Koordinatensystem-Punkten
	 * [x] = 8 Fälle, siehe if-Block unten
	 *
	 * x/yDouble = exakte Koordinate im (TILE_WIDTH x TILE_HEIGHT)-Koordinatensystem.
	 * x/yInt = abgerundete Koordinate im (TILE_WIDTH x TILE_HEIGHT)-Koordinatensystem, entspricht den O-Punkten im Bild
	 * x/yDiff = Wert zwischen 0.0 und 1.0, siehe Grafik oben, mit dem die 8 Fälle identifiziert werden
	 *
	 */

	double xDouble = (double) screenX / (double) GraphicsMgr::TILE_WIDTH;
	double yDouble = (double) screenY / (double) GraphicsMgr::TILE_HEIGHT;
	int xInt = (int) floor(xDouble);
	int yInt = (int) floor(yDouble);
	double xDiff = xDouble - (double) xInt;
	double yDiff = yDouble - (double) yInt;

	// preliminaryMap-Koordinate = Map-Koordinate der Kachel, die ggf. noch in 4 von 8 Fällen angepasst werden muss
	int preliminaryMapX = xInt + yInt;
	int preliminaryMapY = yInt - xInt;

	// Jetzt die 8 Fälle durchgehen und ggf. noch plus-minus 1 auf x oder y
	if (xDiff < 0.5) {
		if (yDiff < 0.5) {
			if (xDiff < 0.5 - yDiff) {
				// Fall 1
				mapX = preliminaryMapX - 1;
				mapY = preliminaryMapY;
			} else {
				// Fall 2
				mapX = preliminaryMapX;
				mapY = preliminaryMapY;
			}
		} else {
			if (xDiff < yDiff - 0.5) {
				// Fall 3
				mapX = preliminaryMapX;
				mapY = preliminaryMapY + 1;
			} else {
				// Fall 4
				mapX = preliminaryMapX;
				mapY = preliminaryMapY;
			}
		}
	} else {
		if (yDiff < 0.5) {
			if (xDiff - 0.5 < yDiff) {
				// Fall 5
				mapX = preliminaryMapX;
				mapY = preliminaryMapY;
			} else {
				// Fall 6
				mapX = preliminaryMapX;
				mapY = preliminaryMapY - 1;
			}
		} else {
			if (xDiff - 0.5 < 1 - yDiff) {
				// Fall 7
				mapX = preliminaryMapX;
				mapY = preliminaryMapY;
			} else {
				// Fall 8
				mapX = preliminaryMapX + 1;
				mapY = preliminaryMapY;
			}
		}
	}
}

// TODO Fehlermanagement, wenn die Datei mal nicht so hübsch aussieht, dass alle Tags da sind
void Map::loadMapFromTMX(const char* filename) {
	// Datei öffnen
	rapidxml::file<> tmxFile(filename);

	rapidxml::xml_document<>* xmlDocument = new rapidxml::xml_document<>();
	xmlDocument->parse<0>(tmxFile.data());

	rapidxml::xml_node<>* mapNode = xmlDocument->first_node("map");
	int newMapWidth = atoi(mapNode->first_attribute("width", 5, true)->value());
	int newMapHeight = atoi(mapNode->first_attribute("height", 6, true)->value());

	rapidxml::xml_node<>* layerNode = mapNode->first_node("layer");
	rapidxml::xml_node<>* dataNode = layerNode->first_node("data");

	const char* dataEncoding = dataNode->first_attribute("encoding", 8, true)->value();
	if (strcmp(dataEncoding, "csv") != 0)
		throw new std::runtime_error("Map is not csv encoded");

	char* csvMapData = dataNode->value();
	char* csvMapDataPtr = csvMapData;

	// CSV-Daten parsen und Map laden
	char currentValue[6];
	int currentValueSize = 0;

	initNewMap(newMapWidth, newMapHeight);

	for (int i = 0; i < newMapWidth * newMapHeight; i++) {
		// ein CSV-Feld lesen, d.h. bis zum nächsten Komma oder Stringende
		currentValueSize = 0;
		while (*csvMapDataPtr != ',' && *csvMapDataPtr != '\0') {
			// Nicht-Ziffern überspringen (das sind z.B. die Newlines am Zeilenende)
			if (*csvMapDataPtr >= '0' && *csvMapDataPtr <= '9') {
				currentValue[currentValueSize++] = *csvMapDataPtr;
			}

			csvMapDataPtr++;
		}
		csvMapDataPtr++; // Trennzeichen skippen

		// Tile zuweisen
		currentValue[currentValueSize] = '\0';
		char tile = atoi(currentValue);
		tiles[i] = tile;
	}

	// XML-Document wegräumen
	delete xmlDocument;
}

void Map::renderMap(SDL_Renderer* renderer) {
	// Kacheln rendern
	SDL_Rect rectDestination = { 0, 0, GraphicsMgr::TILE_WIDTH, GraphicsMgr::TILE_HEIGHT };
	for (int mapY = 0; mapY < (int) height; mapY++) {
		for (int mapX = 0; mapX < (int) width; mapX++) {
			mapToScreenCoords(mapX, mapY, rectDestination.x, rectDestination.y);

			// Scrolling-Offset anwenden
			rectDestination.x -= screenOffsetX;
			rectDestination.y -= screenOffsetY;

			// Clipping
			if (rectDestination.x > windowWidth || rectDestination.y > windowHeight
					|| rectDestination.x + GraphicsMgr::TILE_WIDTH < 0
					|| rectDestination.y + GraphicsMgr::TILE_HEIGHT < 0) {
				continue;
			}

			SDL_Texture* tileTexture = graphicsMgr->getTile(getTileAt(mapX, mapY))->getTexture();

			if (selectedMapObject != nullptr) {
				SDL_SetTextureColorMod(tileTexture, 160, 160, 160);
			} else {
				SDL_SetTextureColorMod(tileTexture, 255, 255, 255);
			}
			SDL_RenderCopy(renderer, tileTexture, NULL, &rectDestination);
		}
	}

	// Objekte rendern
	for (auto iter = mapObjects.cbegin(); iter != mapObjects.cend(); iter++) {
		MapObject* mapObject = *iter;

		SDL_Rect rect = SDL_Rect();
		rect.x = mapObject->screenX - screenOffsetX;
		rect.y = mapObject->screenY - screenOffsetY;
		rect.w = mapObject->screenWidth;
		rect.h = mapObject->screenHeight;

		// Clipping
		if (rect.x > windowWidth || rect.y > windowHeight || rect.x + rect.w < 0 || rect.y + rect.h < 0) {
			continue;
		}

		Graphic* graphic = graphicsMgr->getObject(mapObject->object);
		SDL_Texture* objectTexture = graphic->getTexture();

		if (selectedMapObject == nullptr || selectedMapObject == mapObject) {
			SDL_SetTextureColorMod(objectTexture, 255, 255, 255);
		} else {
			SDL_SetTextureColorMod(objectTexture, 160, 160, 160);
		}
		SDL_RenderCopy(renderer, objectTexture, NULL, &rect);
	}

	// Bildfläche anzeigen
	SDL_RenderPresent(renderer);
}

void Map::scroll(int screenOffsetX, int screenOffsetY) {
	this->screenOffsetX += screenOffsetX;
	this->screenOffsetY += screenOffsetY;
}

const MapObject* Map::addMapObject(int mapX, int mapY, unsigned char object) {
	// Position berechnen in Screen-Koordinaten berechnen, an dem sich die Grafik befinden muss.
	Graphic* graphic = graphicsMgr->getObject(object);
	SDL_Rect rect = { 0, 0, graphic->getWidth(), graphic->getHeight() };
	mapToScreenCoords(mapX, mapY, rect.x, rect.y);

	// Grafik an die richtige Stelle schieben. Das muss ausgehend von der zu belegenden Tile-Fläche berechnet werden.
	rect.x -= graphic->getWidth() - (graphic->getMapWidth() + 1) * GraphicsMgr::TILE_WIDTH_HALF;
	rect.y -= graphic->getHeight() - (graphic->getMapWidth() + graphic->getMapHeight()) * GraphicsMgr::TILE_HEIGHT_HALF;

	// Objekt anlegen und in die Liste aufnehmen
	MapObject* newMapObject = new MapObject();
	newMapObject->mapX = mapX;
	newMapObject->mapY = mapY;
	newMapObject->mapWidth = graphic->getMapWidth();
	newMapObject->mapHeight = graphic->getMapHeight();
	newMapObject->object = object;
	newMapObject->screenX = rect.x;
	newMapObject->screenY = rect.y;
	newMapObject->screenWidth = graphic->getWidth();
	newMapObject->screenHeight = graphic->getHeight();

	mapObjects.push_front(newMapObject);

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

	return newMapObject;
}

void Map::clearMapObjects() {
	// TODO sollte wohl synchronisiert werden
	for (auto iter = mapObjects.cbegin(); iter != mapObjects.cend(); iter++) {
		MapObject* mapObject = *iter;
		delete mapObject;
	}
	mapObjects.clear();
}

void Map::onClick(int mouseX, int mouseY) {
	int mouseAtScreenX = mouseX + getScreenOffsetX();
	int mouseAtScreenY = mouseY + getScreenOffsetY();

	// Gucken, ob ein Objekt geklickt wurde.
	// Objekte dabei rückwärts iterieren. Somit kommen "oben liegende" Objekte zuerst dran.
	for (auto iter = mapObjects.crbegin(); iter != mapObjects.crend(); iter++) {
		MapObject* mapObject = *iter;

		// Außerhalb der Boundary-Box des Objekt geklickt?
		if (mouseAtScreenX < mapObject->screenX) {
			continue;
		}
		if (mouseAtScreenX >= mapObject->screenX + mapObject->screenWidth) {
			continue;
		}
		if (mouseAtScreenY < mapObject->screenY) {
			continue;
		}
		if (mouseAtScreenY >= mapObject->screenY + mapObject->screenHeight) {
			continue;
		}

		// Pixelfarbwert holen
		Uint8 r, g, b, a;
		int x = mouseAtScreenX - mapObject->screenX;
		int y = mouseAtScreenY - mapObject->screenY;
		graphicsMgr->getObject(mapObject->object)->getPixel(x, y, &r, &g, &b, &a);

		// Checken, ob Pixel un-transparent genug ist, um es als Treffer zu nehmen
		if (a > 127) {
			onObjectClick(mapObject, x, y);
			return;
		}
	}

	// TODO später ggf. weitere Events
	selectedMapObject = nullptr;
}

void Map::onObjectClick(MapObject* mapObject, int mouseXInObject, int mouseYInObject) {
	std::cout << "Klick auf " << std::to_string(mapObject->object) << "@(" << std::to_string(mouseXInObject) << ", "
			<< std::to_string(mouseYInObject) << ")" << std::endl;

	selectedMapObject = mapObject;
}
