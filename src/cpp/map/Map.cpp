#include "config/BuildingConfigMgr.h"
#include "game/Colony.h"
#include "game/Game.h"
#include "map/DrawingOrderer.h"
#include "map/Map.h"
#include "map/MapUtils.h"
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"
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
 *   Die Bildschirm-Koordinate einer Kachel bestimmt die Position des Pixels links-oben am Kachelrechteck.
 *   Beim Zoomen ändert sich nicht die Zuordnung von map- zu screenCoords. Beim Zeichnen wird verkleinert, d.h.
 *   mehr Inhalt dargestellt. Auf Zoomstufe Verkleinerung 4x z.B. liegt neben dem Pixel das screenCoords (0, 0)
 *   darstellt, die screenCoords (4, 0).
 */

// Aus main.cpp importiert
extern BuildingConfigMgr* buildingConfigMgr;
extern Game* game;
extern GraphicsMgr* graphicsMgr;
extern SDL_Renderer* renderer;
extern int mouseCurrentX, mouseCurrentY;
extern const int windowWidth;
extern const int windowHeight;

Map::Map() {
	loadMapFromTMX("data/map/map.tmx");
    
    Player* player1 = game->getPlayer(0);
    Player* player2 = game->getPlayer(1);
    Player* player3 = game->getPlayer(2);
    Player* player4 = game->getPlayer(3);

	addBuilding(52, 39, CHAPEL, player1);
    addBuilding(51, 35, WEAPONSMITH, player1);
    addBuilding(58, 39, SIGNALFIRE, player1);
    addBuilding(55, 35, HERBARY, player1);
    addBuilding(50, 32, BRICKYARD, player1);
    addBuilding(51, 26, BRICKYARD2, player1);
    addBuilding(43, 24, OFFICE, player1);
    addBuilding(49, 41, MARKETPLACE, player1);
    
    addBuilding(199, 77, OFFICE, player1);
    addBuilding(228, 214, OFFICE, player2);
    addBuilding(28, 226, OFFICE, player3);
    addBuilding(130, 94, OFFICE, player4);
    
	addStructure(48, 30, WAY_NW_SE, player1);
	addStructure(49, 30, WAY_NW_SE, player1);
	addStructure(50, 30, WAY_NW_SE, player1);
	addStructure(51, 30, WAY_NW_SE, player1);
	addStructure(52, 30, WAY_NW_SE, player1);
	addStructure(53, 30, WAY_NW_SE, player1);
	addStructure(54, 30, WAY_E, player1);
	addStructure(54, 31, WAY_SW_NE, player1);
	addStructure(54, 32, WAY_SW_NE, player1);
	addStructure(54, 33, WAY_SW_NE, player1);
	addStructure(54, 34, WAY_SW_NE, player1);
	addStructure(54, 35, WAY_SW_NE, player1);
	addStructure(54, 36, WAY_SW_NE, player1);
	addStructure(54, 37, WAY_S, player1);
	addStructure(53, 37, WAY_NW_SE, player1);
	addStructure(52, 37, WAY_NW_SE, player1);
	addStructure(51, 37, WAY_NW_SE, player1);
	addStructure(50, 37, WAY_NW_SE, player1);
	addStructure(49, 37, WAY_NW_SE, player1);
	addStructure(48, 37, WAY_NW_SE, player1);
	addStructure(47, 37, WAY_W, player1);
	addStructure(47, 36, WAY_SW_NE, player1);
	addStructure(47, 35, WAY_SW_NE, player1);
	addStructure(47, 34, WAY_SW_NE, player1);
	addStructure(47, 33, WAY_SW_NE, player1);
	addStructure(47, 32, WAY_SW_NE, player1);
	addStructure(47, 31, WAY_SW_NE, player1);
	addStructure(47, 30, WAY_N, player1);
    
    updateMinimapTexture();
    
    MapTile* mapTile = mapTiles->getData(43, 24, nullptr);
    Colony* colony = game->foundNewColony(mapTile->player, mapTile->isle);
    colony->setGoodsInventory(GoodsType::TOOLS, 15);
    colony->setGoodsInventory(GoodsType::WOOD, 30);
    colony->setGoodsInventory(GoodsType::BRICKS, 2);
    
    mapTile = mapTiles->getData(199, 77, nullptr);
    colony = game->foundNewColony(mapTile->player, mapTile->isle);
    colony->setGoodsInventory(GoodsType::TOOLS, 5);
    colony->setGoodsInventory(GoodsType::WOOD, 15);
    colony->setGoodsInventory(GoodsType::BRICKS, 7);
    
    mapTile = mapTiles->getData(228, 214, nullptr);
    colony = game->foundNewColony(mapTile->player, mapTile->isle);
    colony->setGoodsInventory(GoodsType::TOOLS, 20);
    colony->setGoodsInventory(GoodsType::WOOD, 30);
    colony->setGoodsInventory(GoodsType::BRICKS, 10);
    
    mapTile = mapTiles->getData(28, 226, nullptr);
    colony = game->foundNewColony(mapTile->player, mapTile->isle);
    colony->setGoodsInventory(GoodsType::TOOLS, 20);
    colony->setGoodsInventory(GoodsType::WOOD, 30);
    colony->setGoodsInventory(GoodsType::BRICKS, 10);
    
    mapTile = mapTiles->getData(130, 94, nullptr);
    colony = game->foundNewColony(mapTile->player, mapTile->isle);
    colony->setGoodsInventory(GoodsType::TOOLS, 20);
    colony->setGoodsInventory(GoodsType::WOOD, 30);
    colony->setGoodsInventory(GoodsType::BRICKS, 10);
}

Map::~Map() {
	clearMap();
}

void Map::initNewMap(int newWidth, int newHeight) {
    // TODO Wir gehen erstmal davon aus, dass die Karten immer quadratisch sind.
    // Damit spar ich mir erstmal Hirnschmalz mit der Minimap und anderem Zeug, was noch kommen wird.
    if (newWidth != newHeight) {
        throw new std::runtime_error("Map has to be quadratically for now ;-p");
    }
    
	// Karte erst leerräumen
	clearMap();
    
    // Neue Größe setzen
    width = newWidth;
    height = newHeight;
    
    // mapTiles neu anlegen und mit Ozean initialisieren
    mapTiles = new RectangleData<MapTile*>(newWidth, newHeight);
    for(int i = 0; i < mapTiles->width * mapTiles->height; i++) {
        mapTiles->data[i] = new MapTile();
    }

	// Sonstiges Zeugs auf Anfangswerte stellen
	selectedMapObject = nullptr;

	screenOffsetX = 0;
	screenOffsetY = 0;
}

bool Map::checkMapCoords(int mapX, int mapY) const {
    return (!(mapX < 0 || mapY < 0 || mapX >= width || mapY >= height));
}

MapTile* Map::getMapTileAt(int mapX, int mapY) const {
    if (!checkMapCoords(mapX, mapY)) {
        return nullptr;
    }
    
    return mapTiles->getData(mapX, mapY, nullptr);
}

MapObject* Map::getMapObjectAt(int mapX, int mapY) const {
    if (!checkMapCoords(mapX, mapY)) {
        return nullptr;
    }
    
    // TODO sollte man analog den Inseln für Direktzugriff speichern
    // Objekt suchen, die sich auf diesen Map-Koordinaten befindet
    for (auto iter = mapObjects.cbegin(); iter != mapObjects.cend(); iter++) {
		MapObject* mapObject = *iter;
        
        int mapObjectMapX, mapObjectMapY, mapObjectMapWidth, mapObjectMapHeight;
        mapObject->getMapCoords(mapObjectMapX, mapObjectMapY, mapObjectMapWidth, mapObjectMapHeight);
        
        // Koordinaten sind nicht im Bereich dieses Map-Objekts
        if (mapX < mapObjectMapX || mapY < mapObjectMapY || mapX >= mapObjectMapX + mapObjectMapWidth ||
                mapY >= mapObjectMapY + mapObjectMapHeight) {
            continue;
        }
        
        // Koordinaten sind im Bereich dieses Map-Objekts
        return mapObject;
    }
    
    // Keine Insel da
    return nullptr;
}

// TODO Fehlermanagement, wenn die Datei mal nicht so hübsch aussieht, dass alle Tags da sind
void Map::loadMapFromTMX(const char* filename) {
	// Datei öffnen
	rapidxml::file<> tmxFile(filename);

	rapidxml::xml_document<>* xmlDocument = new rapidxml::xml_document<>();
	xmlDocument->parse<0>(tmxFile.data());

	rapidxml::xml_node<>* mapNode = xmlDocument->first_node("map", 3, true);
	int newMapWidth = atoi(mapNode->first_attribute("width", 5, true)->value());
	int newMapHeight = atoi(mapNode->first_attribute("height", 6, true)->value());
    
    initNewMap(newMapWidth, newMapHeight);

	rapidxml::xml_node<>* objectgroupNode = mapNode->first_node("objectgroup", 11, true);
    for (rapidxml::xml_node<>* objectNode = objectgroupNode->first_node("object", 6, true); objectNode != nullptr; 
            objectNode = objectNode->next_sibling("object", 6, true)) {
        
        const char* nodeType = objectNode->first_attribute("type", 4, true)->value();
        
        int x = atoi(objectNode->first_attribute("x", 1, true)->value());
        int y = atoi(objectNode->first_attribute("y", 1, true)->value());
        
        int mapX = x / GraphicsMgr::TILE_HEIGHT; // tiled rechnet merkwürdigerweise auch für X in KachelHÖHE
        int mapY = y / GraphicsMgr::TILE_HEIGHT; 
        
        // Insel
        if (strcmp(nodeType, "isle") == 0) {
            // Objekt aus der Tiled-Datei lesen
            const char* isleName = objectNode->first_attribute("name", 4, true)->value();
            
            int isleWidth = atoi(objectNode->first_attribute("width", 5, true)->value());
            int isleHeight = atoi(objectNode->first_attribute("height", 6, true)->value());

            int isleMapWidth = isleWidth / GraphicsMgr::TILE_HEIGHT; // tiled rechnet merkwürdigerweise auch für X in KachelHÖHE
            int isleMapHeight = isleHeight / GraphicsMgr::TILE_HEIGHT;

            // Dateiname der Insel zusammenbauen
            std::string filename = "data/map/isles/";
            filename.append(isleName);
            filename.append(".tmx");

            // Insel laden
            Isle* isle = new Isle(filename.data());

            // Prüfen, ob die Insel wirklich die Größe hat, wie die Karte sie haben will.
            if (isle->getWidth() != isleMapWidth || isle->getHeight() != isleMapHeight) {
                std::cerr << "Isle '" << isleName << "' ('" << 
                        std::to_string(isle->getWidth()) << "x" << std::to_string(isle->getHeight()) <<
                        ") does not match size on map (" <<
                        std::to_string(isleMapWidth) << "x" << std::to_string(isleMapHeight) << ")";

                throw new std::runtime_error("Isle does not match size on map");
            }

            // Insel zur Karte hinzufügen
            isle->setMapCoords(mapX, mapY, isleMapWidth, isleMapHeight);
            isles.push_back(isle);
            
            for (int my = mapY, isleY = 0; my < mapY + isleMapHeight; my++, isleY++) {
                for (int mx = mapX, isleX = 0; mx < mapX + isleMapWidth; mx++, isleX++) {
                    MapTile* mapTile = mapTiles->getData(mx, my, nullptr);
                    mapTile->tileGraphicIndex = isle->getTileAt(isleX, isleY);
                    mapTile->isle = isle;
                }
            }
        }
        // Startpunkt: Diesen Punkt wollen wir auf den Bildschirm zentrieren
        else if (strcmp(nodeType, "startpoint") == 0) {
            int screenCenterX, screenCenterY;
            MapUtils::mapToScreenCoordsCenter(mapX, mapY, screenCenterX, screenCenterY);
            
            screenOffsetX = screenCenterX - (mapClipRect.w / 2);
            screenOffsetY = screenCenterY - (mapClipRect.h / 2);
        }
    }
    screenZoom = 2;

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
        screenOffsetX, screenOffsetY,
        mapXTopLeft, mapYTopLeft);
	MapUtils::screenToMapCoords(
        (mapClipRect.w * screenZoom) + screenOffsetX, screenOffsetY,
        mapXTopRight, mapYTopRight);
	MapUtils::screenToMapCoords(
        screenOffsetX, (mapClipRect.h * screenZoom) + screenOffsetY,
        mapXBottomLeft, mapYBottomLeft);
	MapUtils::screenToMapCoords(
        (mapClipRect.w * screenZoom) + screenOffsetX, (mapClipRect.h * screenZoom) + screenOffsetY,
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
    SDL_SetRenderDrawColor(renderer, 192, 128, 0, 255);
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
            
            MapTile* mapTile = mapTiles->getData(mapX, mapY, nullptr);
            
            unsigned char tile = mapTile->tileGraphicIndex;
            Player* player = mapTile->player;
            
            *(pixelPtr++) = (tile == 1) ? 0x000090 : (player != nullptr ? ((Uint32) player->getColor()) : 0x008000);
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
        screenOffsetX, screenOffsetY,
        mapXTopLeft, mapYTopLeft);
	MapUtils::screenToMapCoords(
        (mapClipRect.w * screenZoom) + screenOffsetX, screenOffsetY,
        mapXTopRight, mapYTopRight);
	MapUtils::screenToMapCoords(
        screenOffsetX, (mapClipRect.h * screenZoom) + screenOffsetY,
        mapXBottomLeft, mapYBottomLeft);
	MapUtils::screenToMapCoords(
        (mapClipRect.w * screenZoom) + screenOffsetX, (mapClipRect.h * screenZoom) + screenOffsetY,
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
	SDL_Rect rectDestination = { 0, 0, GraphicsMgr::TILE_WIDTH / screenZoom, GraphicsMgr::TILE_HEIGHT / screenZoom };
	for (int mapY = mapYStart; mapY <= mapYEnd; mapY++) {
		for (int mapX = mapXStart; mapX <= mapXEnd; mapX++) {
			MapUtils::mapToScreenCoords(mapX, mapY, rectDestination.x, rectDestination.y);

			// Scrolling-Offset anwenden
			rectDestination.x -= screenOffsetX;
			rectDestination.y -= screenOffsetY;
            
            rectDestination.x /= screenZoom;
            rectDestination.y /= screenZoom;

			// Clipping
			if (rectDestination.x >= mapClipRect.x + mapClipRect.w || rectDestination.y >= mapClipRect.y + mapClipRect.h
					|| rectDestination.x + GraphicsMgr::TILE_WIDTH < mapClipRect.x
					|| rectDestination.y + GraphicsMgr::TILE_HEIGHT < mapClipRect.y) {
				continue;
			}

			SDL_Texture* tileTexture =
                    graphicsMgr->getGraphicForTile(getMapTileAt(mapX, mapY)->tileGraphicIndex)->getTexture();

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
        rect.x /= screenZoom;
        rect.y /= screenZoom;
        rect.w /= screenZoom;
        rect.h /= screenZoom;

        renderStructure(structure, &rect, false, false, false);
	}
    
    // Postionieren wir grade ein neues Gebäude?
    if (game->getAddingStructure() != NO_STRUCTURE) {
        int mouseScreenX = (mouseCurrentX * screenZoom) + screenOffsetX;
        int mouseScreenY = (mouseCurrentY * screenZoom) + screenOffsetY;
        int mapX, mapY;
        MapUtils::screenToMapCoords(mouseScreenX, mouseScreenY, mapX, mapY);
        
        StructureType structureType = game->getAddingStructure();
        unsigned char allowedToPlaceStructure = isAllowedToPlaceStructure(mapX, mapY, structureType);
        
        // Auf dem Ozean malen wir gar nix
        if (!(allowedToPlaceStructure & PLACING_STRUCTURE_OUTSIDE_OF_ISLE)) {
            Graphic* graphic = graphicsMgr->getGraphicForStructure(structureType);
            
            // Zu zeichnendes Gebäude erstellen
            Structure structure;
            structure.setStructureType(structureType);
            structure.setMapCoords(mapX, mapY, graphic->getMapWidth(), graphic->getMapHeight());

            SDL_Rect rect;
            MapUtils::mapToDrawScreenCoords(mapX, mapY, graphic, &rect);
            rect.x /= screenZoom;
            rect.y /= screenZoom;
            rect.w /= screenZoom;
            rect.h /= screenZoom;

            bool redAndSemiTransparent = (allowedToPlaceStructure & PLACING_STRUCTURE_NO_ROOM);
            bool blink = (allowedToPlaceStructure & PLACING_STRUCTURE_NO_RESOURCES);
            renderStructure(&structure, &rect, true, redAndSemiTransparent, blink);
        }
    }

	// Clipping wieder zurücksetzen, bevor der nächste mit Malen drankommt
	SDL_RenderSetClipRect(renderer, nullptr);
}

unsigned char Map::isAllowedToPlaceStructure(int mapX, int mapY, StructureType structureType) {
    MapTile* mapTile = getMapTileAt(mapX, mapY);
    if (mapTile == nullptr) {
        return PLACING_STRUCTURE_OUTSIDE_OF_ISLE;
    }
    
    Isle* isle = mapTile->isle;
    if (isle == nullptr) {
        return PLACING_STRUCTURE_OUTSIDE_OF_ISLE;
    }
    
    unsigned char result = PLACING_STRUCTURE_ALLOWED;
    
    // Resourcen checken
    Player* currentPlayer = game->getCurrentPlayer();
    Colony* colony = game->getColony(currentPlayer, isle);
    
    if (colony != nullptr) {
        const BuildingCosts* buildingCosts = buildingConfigMgr->getConfig(structureType)->getBuildingCosts();
        if ((buildingCosts->coins > currentPlayer->coins) ||
            (buildingCosts->tools > colony->getGoodsInventory(GoodsType::TOOLS)) ||
            (buildingCosts->wood > colony->getGoodsInventory(GoodsType::WOOD)) ||
            (buildingCosts->bricks > colony->getGoodsInventory(GoodsType::BRICKS))) {
                result |= PLACING_STRUCTURE_NO_RESOURCES;
        }
    }
    
    // Checken, ob alles frei is, um das Gebäude zu setzen
    Graphic* graphic = graphicsMgr->getGraphicForStructure(structureType);
    for (int y = mapY; y < mapY + graphic->getMapHeight(); y++) {
        for (int x = mapX; x < mapX + graphic->getMapWidth(); x++) {
            MapTile* mapTile = mapTiles->getData(x, y, nullptr);
            if (
                    // Gebiet nicht erschlossen oder nicht unseres
                    mapTile->player == nullptr || mapTile->player != game->getCurrentPlayer() ||

                    // Da steht was im Weg
                    getMapObjectAt(x, y) != nullptr ||
                    
                    // Gelände passt nicht
                    // TODO aktuell darf nur auf Grass und Grass2 gebaut werden. Später muss das das Gebäude wissen, wo.
                    (mapTile->tileGraphicIndex != 2 && mapTile->tileGraphicIndex != 7)
            ) {
                result |= PLACING_STRUCTURE_NO_ROOM;
                return result;
            }
        }
    }
    
    return result;
}

void Map::renderStructure(Structure* structure, SDL_Rect* rect, bool masked, bool redAndSemiTransparent, bool blink) {
    rect->x -= screenOffsetX / screenZoom;
    rect->y -= screenOffsetY / screenZoom;
        
    // Clipping
    if (rect->x >= mapClipRect.x + mapClipRect.w || rect->y >= mapClipRect.y + mapClipRect.h ||
            rect->x + rect->w < mapClipRect.x || rect->y + rect->h < mapClipRect.y) {
        return;
    }

    Graphic* graphic = graphicsMgr->getGraphicForStructure(structure->getStructureType());
    SDL_Texture* objectTexture = masked ? graphic->getTextureMasked() : graphic->getTexture();
    
    // Gebäude nicht zeichnen, wenn wir im Blinkmodus sind. Dann nur in der ersten Hälfte eines Intervalls zeichnen
    if (!blink || (SDL_GetTicks() % 800 < 400)) {
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

        if (redAndSemiTransparent) {
            SDL_SetTextureColorMod(objectTexture, 255, 0, 0);
            SDL_SetTextureAlphaMod(objectTexture, 128);
        } else {
            SDL_SetTextureColorMod(objectTexture, 255, 255, 255);
            SDL_SetTextureAlphaMod(objectTexture, 255);
        }

        SDL_RenderCopy(renderer, objectTexture, NULL, rect);
    }
    
    // masked nicht gesetzt? Dann sind wir fertig. 
    if (!masked) {
        return;  
    }
    
    // Einzugsbereich-Rahmen malen
    SDL_SetRenderDrawColor(renderer, 0xc8, 0xaf, 0x37, 255);
    
    const BuildingConfig* buildingConfig = buildingConfigMgr->getConfig(structure->getStructureType());
    const RectangleData<char>* catchmentAreaData = buildingConfig->getCatchmentArea(); 
    if (catchmentAreaData != nullptr) {
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
                            (screenX + GraphicsMgr::TILE_WIDTH_HALF) / screenZoom, screenY / screenZoom, 
                            (screenX + GraphicsMgr::TILE_WIDTH) / screenZoom,
                            (screenY + GraphicsMgr::TILE_HEIGHT_HALF) / screenZoom); 
                }

                // Oben links
                if (catchmentAreaData->getData(x - 1, y, '0') == '0' && catchmentAreaData->getData(x, y, '0') == '1') {
                    SDL_RenderDrawLine(renderer,
                            screenX / screenZoom, (screenY + GraphicsMgr::TILE_HEIGHT_HALF) / screenZoom,
                            (screenX + GraphicsMgr::TILE_WIDTH_HALF) / screenZoom, screenY / screenZoom);
                }

                // Unten rechts
                if (catchmentAreaData->getData(x, y, '0') == '1' && catchmentAreaData->getData(x + 1, y, '0') == '0') {
                    SDL_RenderDrawLine(renderer, 
                            (screenX + GraphicsMgr::TILE_WIDTH_HALF) / screenZoom,
                            (screenY + GraphicsMgr::TILE_HEIGHT) / screenZoom, 
                            (screenX + GraphicsMgr::TILE_WIDTH) / screenZoom,
                            (screenY + GraphicsMgr::TILE_HEIGHT_HALF) / screenZoom);
                }

                // Unten links
                if (catchmentAreaData->getData(x, y, '0') == '1' && catchmentAreaData->getData(x, y + 1, '0') == '0') {
                    SDL_RenderDrawLine(renderer,
                            screenX / screenZoom, (screenY + GraphicsMgr::TILE_HEIGHT_HALF) / screenZoom,
                            (screenX + GraphicsMgr::TILE_WIDTH_HALF) / screenZoom,
                            (screenY + GraphicsMgr::TILE_HEIGHT) / screenZoom);
                }
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
    
    resortMapObjects();
}

void Map::resortMapObjects() {
    // Reihenfolge der Objekte so stellen, dass von hinten nach vorne gerendert wird.
    // Siehe doc/rendering-order.xcf für exakte Erklärung.
    
    DrawingOrderer drawingOrderer(&mapObjects);
    drawingOrderer.reorder();
}

const Structure* Map::addStructure(int mapX, int mapY, StructureType structureType, Player* player) {
	Graphic* graphic = graphicsMgr->getGraphicForStructure(structureType);
    
	// Position in Screen-Koordinaten berechnen, an dem sich die Grafik befinden muss.
    SDL_Rect rect;
    MapUtils::mapToDrawScreenCoords(mapX, mapY, graphic, &rect);

	// Objekt anlegen und in die Liste aufnehmen
	Structure* structure = new Structure();
	structure->setMapCoords(mapX, mapY, graphic->getMapWidth(), graphic->getMapHeight());
	structure->setScreenCoords(rect.x, rect.y, graphic->getWidth(), graphic->getHeight());
	structure->setStructureType(structureType);
    structure->setPlayer(player);

	addMapObject(structure);

	return structure;
}

const Building* Map::addBuilding(int mapX, int mapY, StructureType structureType, Player* player) {
    Graphic* graphic = graphicsMgr->getGraphicForStructure(structureType);
    
	// Position in Screen-Koordinaten berechnen, an dem sich die Grafik befinden muss.
    SDL_Rect rect;
    MapUtils::mapToDrawScreenCoords(mapX, mapY, graphic, &rect);

	// Objekt anlegen und in die Liste aufnehmen
	Building* building = new Building();
	building->setMapCoords(mapX, mapY, graphic->getMapWidth(), graphic->getMapHeight());
	building->setScreenCoords(rect.x, rect.y, graphic->getWidth(), graphic->getHeight());
	building->setStructureType(structureType);
    building->setPlayer(player);

	addMapObject(building);
    
    // Kontor? Einzugbereich in mapTiles aktualisieren
    if (structureType == OFFICE || structureType == MARKETPLACE) {
        addOfficeCatchmentAreaToMap(*building);
    }

	return building;
}

void Map::addOfficeCatchmentAreaToMap(const Building& building) {
    int buildingMapX, buildingMapY;
    building.getMapCoords(buildingMapX, buildingMapY);
    
    const BuildingConfig* buildingConfig = buildingConfigMgr->getConfig(building.getStructureType());
    RectangleData<char>* catchmentArea = buildingConfig->getCatchmentArea();
    
    // TODO Sehr hässlich, aber tuts erstmal sicher, ohne Gefahr.
    for (int mapY = buildingMapY - catchmentArea->height/2 - 1;
             mapY <= buildingMapY + catchmentArea->height/2 + 1; mapY++) {
        
        for (int mapX = buildingMapX - catchmentArea->width/2 - 1;
                 mapX <= buildingMapX + catchmentArea->width/2 + 1; mapX++) {
            
            if (!building.isInsideCatchmentArea(mapX, mapY)) {
                continue;
            }
            
            MapTile* mapTile = mapTiles->getData(mapX, mapY, nullptr);
            if (mapTile == nullptr) {
                continue;
            }
            
            mapTile->player = building.getPlayer();
        }
    }
    
    updateMinimapTexture();
}

void Map::clearMap() {
    selectedMapObject = nullptr;
    
    // Map-Objekte wegräumen
	// TODO sollte wohl synchronisiert werden
	for (auto iter = mapObjects.cbegin(); iter != mapObjects.cend(); iter++) {
		MapObject* mapObject = *iter;
		delete mapObject;
	}
	mapObjects.clear();
    
    // mapTiles wegräumen
    if (mapTiles != nullptr) {
        for(int i = 0; i < mapTiles->width * mapTiles->height; i++) {
            delete mapTiles->data[i];
        }
        delete mapTiles;
        mapTiles = nullptr;
    }
    
    // Inseln wegräumen
    isles.clear();
    
    // Minimap-Texture wegräumen
    if (minimapTexture != nullptr) {
        SDL_DestroyTexture(minimapTexture);
        minimapTexture = nullptr;
    }
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
	int mouseAtScreenX = (mouseX * screenZoom) + screenOffsetX;
	int mouseAtScreenY = (mouseY * screenZoom) + screenOffsetY;
    
    // Grade beim Platzieren eines neuen Gebäudes?
    if (game->isAddingStructure()) {
        int mapX, mapY;
        MapUtils::screenToMapCoords(mouseAtScreenX, mouseAtScreenY, mapX, mapY);
        
        onClickInMapWhileAddingStructure(mapX, mapY);
        return;
    }

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

void Map::onClickInMapWhileAddingStructure(int mapX, int mapY) {
    StructureType structureType = game->getAddingStructure();
    if (isAllowedToPlaceStructure(mapX, mapY, structureType) != PLACING_STRUCTURE_ALLOWED) {
        // Dürfen wir hier/jetzt nicht setzen, ignorieren wir den Klick
        return;
    }
    
    // Gebäude platzieren und Modus verlassen
    Player* currentPlayer = game->getCurrentPlayer();
    if (structureType >= START_BUILDINGS) {
        addBuilding(mapX, mapY, structureType, currentPlayer);
    } else {
        addStructure(mapX, mapY, structureType, currentPlayer);
    }
    
    // Resourcen bezahlen
    Colony* colony = game->getColony(currentPlayer, getMapTileAt(mapX, mapY)->isle);
    const BuildingCosts* buildingCosts = buildingConfigMgr->getConfig(structureType)->getBuildingCosts();
    currentPlayer->coins -= buildingCosts->coins;
    colony->subtractBuildingCosts(buildingCosts);
    
    // TODO Status der Shift-Taste aus dem Event bis hierhin durchschleußen.
    // Wenn gedrückt, "Gebäude platzieren"-Modus nicht verlassen, damit wir mehrere Gebäude auf einmal setzen können.
    game->endAddingStructure();
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
    screenX -= (mapClipRect.w * screenZoom) / 2;
    screenY -= (mapClipRect.h * screenZoom) / 2;
    
    this->screenOffsetX = screenX;
    this->screenOffsetY = screenY;
}

void Map::deleteSelectedObject() {
    if (selectedMapObject == nullptr) {
        return;
    }
    
    mapObjects.remove(selectedMapObject);
    delete selectedMapObject;
    
    selectedMapObject = nullptr;
    
    resortMapObjects();
}