#include "config/BuildingConfigMgr.h"
#include "map/Map.h"
#include "map/MapUtils.h"
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "Game.h"
#include "GraphicsMgr.h"

#define SDL_SetTextureDarkened(texture) (SDL_SetTextureColorMod((texture), 160, 160, 160))
#define SDL_SetTextureNormal(texture) (SDL_SetTextureColorMod((texture), 255, 255, 255))


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
extern BuildingConfigMgr* buildingConfigMgr;
extern Game* game;
extern GraphicsMgr* graphicsMgr;
extern SDL_Renderer* renderer;
extern int mouseCurrentX, mouseCurrentY;
extern const int windowWidth;
extern const int windowHeight;

/**
 * @brief Rechteck, indem die Karte dargestellt wird
 */
const SDL_Rect mapClipRect = { 0, 0, 768, 734 };

/**
 * @brief Rechteck, indem die Minimap dargestellt wird
 */
const SDL_Rect minimapClipRect = { 796, 28, 200, 200 };


Map::Map(unsigned int width, unsigned int height) :
		width(width), height(height) {

	initNewMap(width, height);

	loadMapFromTMX("data/map/map.tmx");

	addBuilding(17, 14, CHAPEL);
    addBuilding(16, 10, WEAPONSMITH);
    addBuilding(24, 15, SIGNALFIRE);
    addBuilding(20, 10, HERBARY);
    addBuilding(16, 7, BRICKYARD);
    addBuilding(16, 2, BRICKYARD2);
    
	addStructure(13, 5, WAY_NW_SE);
	addStructure(14, 5, WAY_NW_SE);
	addStructure(15, 5, WAY_NW_SE);
	addStructure(16, 5, WAY_NW_SE);
	addStructure(17, 5, WAY_NW_SE);
	addStructure(18, 5, WAY_NW_SE);
	addStructure(19, 5, WAY_E);
	addStructure(19, 6, WAY_SW_NE);
	addStructure(19, 7, WAY_SW_NE);
	addStructure(19, 8, WAY_SW_NE);
	addStructure(19, 9, WAY_SW_NE);
	addStructure(19, 10, WAY_SW_NE);
	addStructure(19, 11, WAY_SW_NE);
	addStructure(19, 12, WAY_S);
	addStructure(18, 12, WAY_NW_SE);
	addStructure(17, 12, WAY_NW_SE);
	addStructure(16, 12, WAY_NW_SE);
	addStructure(15, 12, WAY_NW_SE);
	addStructure(14, 12, WAY_NW_SE);
	addStructure(13, 12, WAY_NW_SE);
	addStructure(12, 12, WAY_W);
	addStructure(12, 11, WAY_SW_NE);
	addStructure(12, 11, WAY_SW_NE);
	addStructure(12, 10, WAY_SW_NE);
	addStructure(12, 9, WAY_SW_NE);
	addStructure(12, 8, WAY_SW_NE);
	addStructure(12, 7, WAY_SW_NE);
	addStructure(12, 6, WAY_SW_NE);
	addStructure(12, 5, WAY_N);
}

Map::~Map() {
	selectedMapObject = nullptr;

	clearMapObjects();
    
    if (minimapTexture != nullptr) {
        SDL_DestroyTexture(minimapTexture);
    }

	delete[] tiles;
	tiles = nullptr;
}

void Map::initNewMap(unsigned int width, unsigned int height) {
    // TODO Wir gehen erstmal davon aus, dass die Karten immer quadratisch sind.
    // Damit spar ich mir erstmal Hirnschmalz mit der Minimap und anderem Zeug, was noch kommen wird.
    if (width != height) {
        throw new std::runtime_error("Map has to be quadratically for now ;-p");
    }
    
	// Karte erst leerräumen
	clearMapObjects();
    
    if (minimapTexture != nullptr) {
        SDL_DestroyTexture(minimapTexture);
    }

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
    
    // Minimap erstellen
    updateMinimapTexture();
}

void Map::renderMinimap(SDL_Renderer* renderer) {
    // Nur die Kartenfläche vollmalen
	SDL_Rect sdlMinimapClipRect(minimapClipRect);
	sdlMinimapClipRect.y = windowHeight - (sdlMinimapClipRect.y + sdlMinimapClipRect.h); // SDL misst das Rechteck von UNTEN, kp, warum und ob das ein Bug is
	SDL_RenderSetClipRect(renderer, &sdlMinimapClipRect);
    
    float scaleFactor = (float) width / (float) minimapClipRect.w;
    
    // Karte zeichnen
    SDL_RenderCopy(renderer, minimapTexture, nullptr, &minimapClipRect);
    
    // Aktuellen Ausschnitt bestimmen
    // TODO Duplicate-Code refactorn, (x/y)-Tuple für MapCoords einführen, Rect (top, left, right, bottom) von Punkten einführen
    int mapXTopLeft, mapYTopLeft, mapXTopRight, mapYTopRight;
	int mapXBottomLeft, mapYBottomLeft, mapXBottomRight, mapYBottomRight;

	MapUtils::screenToMapCoords(
        mapClipRect.x + screenOffsetX, mapClipRect.y + screenOffsetY, mapXTopLeft, mapYTopLeft);
	MapUtils::screenToMapCoords(
        mapClipRect.x + mapClipRect.w + screenOffsetX, 0 + screenOffsetY, mapXTopRight, mapYTopRight);
	MapUtils::screenToMapCoords(
        mapClipRect.x + screenOffsetX, mapClipRect.y + mapClipRect.h + screenOffsetY, mapXBottomLeft, mapYBottomLeft);
	MapUtils::screenToMapCoords(
        mapClipRect.x + mapClipRect.w + screenOffsetX, mapClipRect.y + mapClipRect.h + screenOffsetY, 
        mapXBottomRight, mapYBottomRight);
    
    SDL_Point points[5] = {
        { minimapClipRect.x + (int) ((float) mapXTopLeft / scaleFactor),
          minimapClipRect.y + (int) ((float) mapYTopLeft / scaleFactor) },
        { minimapClipRect.x + (int) ((float) mapXTopRight / scaleFactor),
          minimapClipRect.y + (int) ((float) mapYTopRight / scaleFactor) },
        { minimapClipRect.x + (int) ((float) mapXBottomRight / scaleFactor),
          minimapClipRect.y + (int) ((float) mapYBottomRight / scaleFactor) },
        { minimapClipRect.x + (int) ((float) mapXBottomLeft / scaleFactor),
          minimapClipRect.y + (int) ((float) mapYBottomLeft / scaleFactor) }
    };
    points[4] = points[0];
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 192);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderDrawLines(renderer, points, 5);
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    SDL_RenderSetClipRect(renderer, nullptr);
}

void Map::updateMinimapTexture() {
    if (minimapTexture != nullptr) {
        SDL_DestroyTexture(minimapTexture);
    }
    
    float scaleFactor = (float) width / (float) minimapClipRect.w;
    
    // Karte zeichnen
    Uint32* pixelData = new Uint32[minimapClipRect.w * minimapClipRect.h];
    Uint32* pixelPtr = pixelData;
    for (int y = 0, screenY = minimapClipRect.y; y < minimapClipRect.h; y++, screenY++) {
        for (int x = 0, screenX = minimapClipRect.x; x < minimapClipRect.w; x++, screenX++) {
            int mapX = (int) ((float) x * scaleFactor);
            int mapY = (int) ((float) y * scaleFactor);
            
            unsigned char tile = tiles[mapY * width + mapX];
            *(pixelPtr++) = (tile == 1) ? 0x0000c0 : 0x008000;
        }
    }
    
    // Daten zur Texture umwandeln
    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(
            pixelData, minimapClipRect.w, minimapClipRect.h, 32, minimapClipRect.w * 4, 0, 0, 0, 0);
    minimapTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    delete[] pixelData;
}

void Map::renderMap(SDL_Renderer* renderer) {
	/*
	 * Optimierung: Das Loop über ALLE Kacheln ist teuer, weil wir jedes Mal die screenCoords ermitteln müssen,
	 * bevor das Clipping greifen kann. Wir ermitteln die mapCoords in den Ecken der Karten-Zeichenfläche, um Start-
	 * und End-Map-Koordinaten zu ermitteln. Damit gehen wir zwar immer noch über mehr Kacheln, als auf dem Bildschirm
	 * sind, aber besser als nix :-)
	 */
    // TODO Duplicate-Code refactorn, (x/y)-Tuple für MapCoords einführen, Rect (top, left, right, bottom) von Punkten einführen
	int mapXTopLeft, mapYTopLeft, mapXTopRight, mapYTopRight;
	int mapXBottomLeft, mapYBottomLeft, mapXBottomRight, mapYBottomRight;

	MapUtils::screenToMapCoords(
        mapClipRect.x + screenOffsetX, mapClipRect.y + screenOffsetY, mapXTopLeft, mapYTopLeft);
	MapUtils::screenToMapCoords(
        mapClipRect.x + mapClipRect.w + screenOffsetX, 0 + screenOffsetY, mapXTopRight, mapYTopRight);
	MapUtils::screenToMapCoords(
        mapClipRect.x + screenOffsetX, mapClipRect.y + mapClipRect.h + screenOffsetY, mapXBottomLeft, mapYBottomLeft);
	MapUtils::screenToMapCoords(
        mapClipRect.x + mapClipRect.w + screenOffsetX, mapClipRect.y + mapClipRect.h + screenOffsetY,
        mapXBottomRight, mapYBottomRight);

	int mapXStart = std::max(mapXTopLeft, 0);
	int mapYStart = std::max(mapYTopRight, 0);
	int mapXEnd = std::min(mapXBottomRight, (int) (width - 1));
	int mapYEnd = std::min(mapYBottomLeft, (int) (height - 1));

	// Nur die Kartenfläche vollmalen
	SDL_Rect sdlMapClipRect(mapClipRect);
	sdlMapClipRect.y = windowHeight - (sdlMapClipRect.y + sdlMapClipRect.h); // SDL misst das Rechteck von UNTEN, kp, warum und ob das ein Bug is
	SDL_RenderSetClipRect(renderer, &sdlMapClipRect);

	// Kacheln rendern
	SDL_Rect rectDestination = { 0, 0, GraphicsMgr::TILE_WIDTH, GraphicsMgr::TILE_HEIGHT };
	for (int mapY = mapYStart; mapY <= mapYEnd; mapY++) {
		for (int mapX = mapXStart; mapX <= mapXEnd; mapX++) {
			MapUtils::mapToScreenCoords(mapX, mapY, rectDestination.x, rectDestination.y);

			// Scrolling-Offset anwenden
			rectDestination.x -= screenOffsetX;
			rectDestination.y -= screenOffsetY;

			// Clipping
			if (rectDestination.x >= mapClipRect.x + mapClipRect.w || rectDestination.y >= mapClipRect.y + mapClipRect.h
					|| rectDestination.x + GraphicsMgr::TILE_WIDTH < mapClipRect.x
					|| rectDestination.y + GraphicsMgr::TILE_HEIGHT < mapClipRect.y) {
				continue;
			}

			SDL_Texture* tileTexture = graphicsMgr->getGraphicForTile(getTileAt(mapX, mapY))->getTexture();

			if (selectedMapObject != nullptr) {
                Building* selectedBuilding = dynamic_cast<Building*>(selectedMapObject);
                bool insideCatchmentArea = 
                    (selectedBuilding != nullptr && selectedBuilding->isInsideCatchmentArea(mapX, mapY));
              
                if (insideCatchmentArea) {
                    SDL_SetTextureNormal(tileTexture);
                } else {
                    SDL_SetTextureDarkened(tileTexture);
                }
			} else {
				SDL_SetTextureNormal(tileTexture);
			}
			SDL_RenderCopy(renderer, tileTexture, NULL, &rectDestination);
		}
	}

	// Objekte rendern
	for (auto iter = mapObjects.cbegin(); iter != mapObjects.cend(); iter++) {
		MapObject* mapObject = *iter;

		// TODO hier später weitere Typen handeln oder cleverer in Objekt-Methoden arbeiten
		Structure* structure = dynamic_cast<Structure*>(mapObject);
		if (structure == nullptr) {
			continue;
		}
        
        SDL_Rect rect = SDL_Rect();
        structure->getScreenCoords(rect.x, rect.y, rect.w, rect.h);

        renderStructure(structure, &rect, false);
	}
    
    // Postionieren wir grade ein neues Gebäude?
    if (game->getAddingStructure() != NO_STRUCTURE) {
        int mouseScreenX = mouseCurrentX + screenOffsetX;
        int mouseScreenY = mouseCurrentY + screenOffsetY;
        int mapX, mapY;
        MapUtils::screenToMapCoords(mouseScreenX, mouseScreenY, mapX, mapY);
        
        // Zu zeichnendes Gebäude erstellen
        StructureType structureType = game->getAddingStructure();
        Graphic* graphic = graphicsMgr->getGraphicForStructure(structureType);
        
        Structure structure;
        structure.setStructureType(structureType);
        structure.setMapCoords(mapX, mapY, graphic->getMapWidth(), graphic->getMapHeight());
        
        SDL_Rect rect;
        MapUtils::mapToDrawScreenCoords(mapX, mapY, graphic, &rect);
        
        renderStructure(&structure, &rect, true);
    }

	// Clipping wieder zurücksetzen, bevor der nächste mit Malen drankommt
	SDL_RenderSetClipRect(renderer, nullptr);
}

void Map::renderStructure(Structure* structure, SDL_Rect* rect, bool masked) {
    rect->x -= screenOffsetX;
    rect->y -= screenOffsetY;
        
    // Clipping
    if (rect->x >= mapClipRect.x + mapClipRect.w || rect->y >= mapClipRect.y + mapClipRect.h ||
            rect->x + rect->w < mapClipRect.x || rect->y + rect->h < mapClipRect.y) {
        return;
    }

    Graphic* graphic = graphicsMgr->getGraphicForStructure(structure->getStructureType());
    SDL_Texture* objectTexture = masked ? graphic->getTextureMasked() : graphic->getTexture();

    if (selectedMapObject != nullptr) {
        Building* selectedBuilding = dynamic_cast<Building*>(selectedMapObject);
        bool insideCatchmentArea = 
            (selectedBuilding != nullptr && selectedBuilding->isInsideCatchmentArea(structure));

        if (insideCatchmentArea) {
            SDL_SetTextureNormal(objectTexture);
        } else {
            SDL_SetTextureDarkened(objectTexture);
        }
    } else {
        SDL_SetTextureNormal(objectTexture);
    }
    SDL_RenderCopy(renderer, objectTexture, NULL, rect);
    
    if (!masked) {
        return;  
    }
    
    // masked gesetzt? Dann malen wir noch den Einzugsbereich-Rahmen rum
    SDL_SetRenderDrawColor(renderer, 0xc8, 0xaf, 0x37, 255);
    
    const BuildingConfig* buildingConfig = buildingConfigMgr->getConfig(structure->getStructureType());
    const RectangleData<char>* catchmentAreaData = buildingConfig->getCatchmentArea(); 
    for (int y = 0; y < catchmentAreaData->height; y++) {
        for (int x = 0; x < catchmentAreaData->width; x++) {
            int mapX, mapY, mapWidth, mapHeight;
            structure->getMapCoords(mapX, mapY, mapWidth, mapHeight);
            
            mapX += x - (catchmentAreaData->width - mapWidth) / 2;
            mapY += y - (catchmentAreaData->height - mapHeight) / 2;
            
            int screenX, screenY;
            MapUtils::mapToScreenCoords(mapX, mapY, screenX, screenY);
            screenX -= screenOffsetX;
            screenY -= screenOffsetY;
            
            // An der Kachel jede der 4 Seiten untersuchen, ob wir eine Linie malen müssen.
            // TODO die String-'1'er ersetzen durch echte 1en.
            
            // Oben rechts
            if (catchmentAreaData->getData(x, y - 1, '0') == '0' && catchmentAreaData->getData(x, y, '0') == '1') {
                SDL_RenderDrawLine(renderer,
                        screenX + GraphicsMgr::TILE_WIDTH_HALF, screenY, 
                        screenX + GraphicsMgr::TILE_WIDTH, screenY + GraphicsMgr::TILE_HEIGHT_HALF); 
            }
            
            // Oben links
            if (catchmentAreaData->getData(x - 1, y, '0') == '0' && catchmentAreaData->getData(x, y, '0') == '1') {
                SDL_RenderDrawLine(renderer,
                        screenX, screenY + GraphicsMgr::TILE_HEIGHT_HALF,
                        screenX + GraphicsMgr::TILE_WIDTH_HALF, screenY);
            }
            
            // Unten rechts
            if (catchmentAreaData->getData(x, y, '0') == '1' && catchmentAreaData->getData(x + 1, y, '0') == '0') {
                SDL_RenderDrawLine(renderer, 
                        screenX + GraphicsMgr::TILE_WIDTH_HALF, screenY + GraphicsMgr::TILE_HEIGHT, 
                        screenX + GraphicsMgr::TILE_WIDTH, screenY + GraphicsMgr::TILE_HEIGHT_HALF);
            }
            
            // Unten links
            if (catchmentAreaData->getData(x, y, '0') == '1' && catchmentAreaData->getData(x, y + 1, '0') == '0') {
                SDL_RenderDrawLine(renderer,
                        screenX, screenY + GraphicsMgr::TILE_HEIGHT_HALF,
                        screenX + GraphicsMgr::TILE_WIDTH_HALF, screenY + GraphicsMgr::TILE_HEIGHT);
            }
        }
    }
}

void Map::scroll(int screenOffsetX, int screenOffsetY) {
	this->screenOffsetX += screenOffsetX;
	this->screenOffsetY += screenOffsetY;
}

void Map::addMapObject(MapObject* mapObject) {
	mapObjects.push_front(mapObject);

	// Reihenfolge der Objekte so stellen, dass von hinten nach vorne gerendert wird
	// TODO ggf. Algorithmus verbessern, dass wirklich nach Y-Screen-Koordinaten sortiert wird. Mit den paar Grafiken
	// hab ich keinen Fall bauen können, der n Unterschied macht.
	mapObjects.sort([] (MapObject* mo1, MapObject* mo2) {
		int mo1x, mo1y, mo2x, mo2y;
		mo1->getMapCoords(mo1x, mo1y);
		mo2->getMapCoords(mo2x, mo2y);

		if (mo1y < mo2y) {
			return true;
		} else if (mo1y > mo2y) {
			return false;
		} else {
			return (mo1x <= mo2x);
		}
	});
}

const Structure* Map::addStructure(int mapX, int mapY, StructureType structureType) {
	// Position berechnen in Screen-Koordinaten berechnen, an dem sich die Grafik befinden muss.
	Graphic* graphic = graphicsMgr->getGraphicForStructure(structureType);
	SDL_Rect rect = { 0, 0, graphic->getWidth(), graphic->getHeight() };
	MapUtils::mapToScreenCoords(mapX, mapY, rect.x, rect.y);

	// Grafik an die richtige Stelle schieben. Das muss ausgehend von der zu belegenden Tile-Fläche berechnet werden.
	rect.x -= graphic->getWidth() - (graphic->getMapWidth() + 1) * GraphicsMgr::TILE_WIDTH_HALF;
	rect.y -= graphic->getHeight() - (graphic->getMapWidth() + graphic->getMapHeight()) * GraphicsMgr::TILE_HEIGHT_HALF;

	// Objekt anlegen und in die Liste aufnehmen
	Structure* structure = new Structure();
	structure->setMapCoords(mapX, mapY, graphic->getMapWidth(), graphic->getMapHeight());
	structure->setScreenCoords(rect.x, rect.y, graphic->getWidth(), graphic->getHeight());
	structure->setStructureType(structureType);

	addMapObject(structure);

	return structure;
}

const Building* Map::addBuilding(int mapX, int mapY, StructureType structureType) {
    Graphic* graphic = graphicsMgr->getGraphicForStructure(structureType);
    
	// Position in Screen-Koordinaten berechnen, an dem sich die Grafik befinden muss.
    SDL_Rect rect;
    MapUtils::mapToDrawScreenCoords(mapX, mapY, graphic, &rect);

	// Objekt anlegen und in die Liste aufnehmen
	Building* building = new Building();
	building->setMapCoords(mapX, mapY, graphic->getMapWidth(), graphic->getMapHeight());
	building->setScreenCoords(rect.x, rect.y, graphic->getWidth(), graphic->getHeight());
	building->setStructureType(structureType);

	addMapObject(building);

	return building;
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
    // Karte
	if (mouseX >= mapClipRect.x && mouseY >= mapClipRect.y &&
            mouseX < mapClipRect.x + mapClipRect.w && mouseY < mapClipRect.y + mapClipRect.h) {
        onClickInMap(mouseX, mouseY);
        return;
    }
    
    // Minimap
    if(mouseX >= minimapClipRect.x && mouseY >= minimapClipRect.y &&
            mouseX < minimapClipRect.x + minimapClipRect.w && mouseY < minimapClipRect.y + minimapClipRect.h) {
        onClickInMinimap(mouseX, mouseY);
        return;
    }
}

void Map::onClickInMap(int mouseX, int mouseY) {
	int mouseAtScreenX = mouseX + getScreenOffsetX();
	int mouseAtScreenY = mouseY + getScreenOffsetY();

	// Gucken, ob ein Gebäude geklickt wurde.
	// Objekte dabei rückwärts iterieren. Somit kommen "oben liegende" Objekte zuerst dran.
	for (auto iter = mapObjects.crbegin(); iter != mapObjects.crend(); iter++) {
		MapObject* mapObject = *iter;

		// TODO hier später weitere Typen handeln oder cleverer in Objekt-Methoden arbeiten
		Building* building = dynamic_cast<Building*>(mapObject);
		if (building == nullptr) {
			// Da wir nur die Buildings durchgehen und nicht alle Structures, haben wir den positiven Nebeneffekt,
			// dass wir z.B. durch eine Mauer durchklicken und ein verstecktes Gebäude dahinter anklicken können.
			continue;
		}

		int screenX, screenY, screenWidth, screenHeight;
		building->getScreenCoords(screenX, screenY, screenWidth, screenHeight);

		// Außerhalb der Boundary-Box des Objekt geklickt?
		if (mouseAtScreenX < screenX) {
			continue;
		}
		if (mouseAtScreenX >= screenX + screenWidth) {
			continue;
		}
		if (mouseAtScreenY < screenY) {
			continue;
		}
		if (mouseAtScreenY >= screenY + screenHeight) {
			continue;
		}

		// Pixelfarbwert holen
		Uint8 r, g, b, a;
		int x = mouseAtScreenX - screenX;
		int y = mouseAtScreenY - screenY;
		graphicsMgr->getGraphicForStructure(building->getStructureType())->getPixel(x, y, &r, &g, &b, &a);

		// Checken, ob Pixel un-transparent genug ist, um es als Treffer zu nehmen
		if (a > 127) {
			building->onClick(x, y);
			return;
		}
	}

	// TODO später ggf. weitere Events
	selectedMapObject = nullptr;
}

void Map::onClickInMinimap(int mouseX, int mouseY) {
    int xInMinimap = mouseX - minimapClipRect.x;
    int yInMinimap = mouseY - minimapClipRect.y;
    
    float scaleFactor = (float) width / (float) minimapClipRect.w;
    
    int mapX = (int) ((float) xInMinimap  * scaleFactor);
    int mapY = (int) ((float) yInMinimap  * scaleFactor);
    
    int screenX, screenY;
    MapUtils::mapToScreenCoords(mapX, mapY, screenX, screenY);
    
    // zentrieren
    screenX -= mapClipRect.w / 2;
    screenY -= mapClipRect.h / 2;
    
    this->screenOffsetX = screenX;
    this->screenOffsetY = screenY;
}