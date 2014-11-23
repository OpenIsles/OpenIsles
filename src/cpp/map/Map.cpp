#include "config/ConfigMgr.h"
#include "game/Colony.h"
#include "game/Game.h"
#include "gui/GuiMgr.h"
#include "map/Map.h"
#include "utils/StringFormat.h"

#ifdef DEBUG_A_STAR
#include "gui/FontMgr.h"
#include "pathfinding/AStar.h"

extern FontMgr* fontMgr;
#endif

// Aus main.cpp importiert
extern ConfigMgr* configMgr;
extern Game* game;
extern GraphicsMgr* graphicsMgr;
extern GuiMgr* guiMgr;
extern SDL_Renderer* renderer;
extern int mouseCurrentX, mouseCurrentY;
extern const int windowWidth;
extern const int windowHeight;

Map::Map() {
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

    // mapObjectAlreadyDrawnThere neu anlegen, brauchen wir fürs Zeichnen
    mapObjectAlreadyDrawnThere = new RectangleData<char>(newWidth, newHeight);

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
    MapTile* mapTile = getMapTileAt(mapX, mapY);
    if (mapTile == nullptr) {
        return nullptr;
    }

    return mapTile->mapObject;
}

void Map::setSelectedMapObject(MapObject* selectedMapObject) {
    this->selectedMapObject = selectedMapObject;

    // Dem GuiMgr Bescheid geben, damit er das Panel umschaltet
    guiMgr->onSelectedMapObjectChanged(selectedMapObject);
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

	MapCoordUtils::screenToMapCoords(
        screenOffsetX, screenOffsetY,
        mapXTopLeft, mapYTopLeft);
	MapCoordUtils::screenToMapCoords(
        (mapClipRect.w * screenZoom) + screenOffsetX, screenOffsetY,
        mapXTopRight, mapYTopRight);
	MapCoordUtils::screenToMapCoords(
        screenOffsetX, (mapClipRect.h * screenZoom) + screenOffsetY,
        mapXBottomLeft, mapYBottomLeft);
	MapCoordUtils::screenToMapCoords(
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

            const MapTileConfig* mapTileConfig = configMgr->getMapTileConfig(mapTile->tileIndex);
            Player* player = mapTile->player;
            
            *(pixelPtr++) =
                (mapTileConfig->isOcean) ? 0x000090 : (player != nullptr ? ((Uint32) player->getColor()) : 0x008000);
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
	 * Optimierung: Das Loopen über ALLE Kacheln ist teuer, weil wir jedes Mal die screenCoords ermitteln müssen,
	 * bevor das Clipping greifen kann. Wir ermitteln die mapCoords in den Ecken der Karten-Zeichenfläche, um Start-
	 * und End-Map-Koordinaten zu ermitteln. Damit gehen wir zwar immer noch über mehr Kacheln, als auf dem Bildschirm
	 * sind, aber besser als nix :-)
	 */
    // TODO Duplicate-Code refactorn, (x/y)-Tuple für MapCoords einführen, Rect (top, left, right, bottom) von Punkten einführen
	int mapXTopLeft, mapYTopLeft, mapXTopRight, mapYTopRight;
	int mapXBottomLeft, mapYBottomLeft, mapXBottomRight, mapYBottomRight;

	MapCoordUtils::screenToMapCoords(
        screenOffsetX, screenOffsetY,
        mapXTopLeft, mapYTopLeft);
	MapCoordUtils::screenToMapCoords(
        (mapClipRect.w * screenZoom) + screenOffsetX, screenOffsetY,
        mapXTopRight, mapYTopRight);
	MapCoordUtils::screenToMapCoords(
        screenOffsetX, (mapClipRect.h * screenZoom) + screenOffsetY,
        mapXBottomLeft, mapYBottomLeft);
	MapCoordUtils::screenToMapCoords(
        (mapClipRect.w * screenZoom) + screenOffsetX, (mapClipRect.h * screenZoom) + screenOffsetY,
        mapXBottomRight, mapYBottomRight);

	int mapXStart = std::max(mapXTopLeft, 0);
	int mapYStart = std::max(mapYTopRight, 0);
	int mapXEnd = std::min(mapXBottomRight + 1, (int) (width - 1)); // am Ende jeweils +1 rechnen, um der Elevation
	int mapYEnd = std::min(mapYBottomLeft + 1, (int) (height - 1)); // entgegen zu wirken. Ohne das würde unten was fehlen.

	// Nur die Kartenfläche vollmalen
	SDL_Rect sdlMapClipRect(mapClipRect);
	sdlMapClipRect.y = windowHeight - (sdlMapClipRect.y + sdlMapClipRect.h); // SDL misst das Rechteck von UNTEN, kp, warum und ob das ein Bug is
	SDL_RenderSetClipRect(renderer, &sdlMapClipRect);

	// Kacheln rendern
	for (int mapY = mapYStart; mapY <= mapYEnd; mapY++) {
		for (int mapX = mapXStart; mapX <= mapXEnd; mapX++) {
            MapObjectGraphic* tileGraphic = graphicsMgr->getGraphicForTile(getMapTileAt(mapX, mapY)->tileIndex);

            SDL_Rect rectDestination;
            MapCoordUtils::mapToDrawCoords(mapX, mapY, 0, tileGraphic, &rectDestination);

			// Clipping
			if (rectDestination.x >= mapClipRect.x + mapClipRect.w || rectDestination.y >= mapClipRect.y + mapClipRect.h
					|| rectDestination.x + rectDestination.w < mapClipRect.x
					|| rectDestination.y + rectDestination.h < mapClipRect.y) {
				continue;
			}

            int drawingFlags = 0;
			if (selectedMapObject != nullptr) {
                Building* selectedBuilding = dynamic_cast<Building*>(selectedMapObject);
                bool insideCatchmentArea = 
                    (selectedBuilding != nullptr && selectedBuilding->isInsideCatchmentArea(mapX, mapY));
              
                if (!insideCatchmentArea) {
                    drawingFlags |= MapObjectGraphic::DRAWING_FLAG_DARKENED;
                }
			}
            tileGraphic->draw(nullptr, &rectDestination, drawingFlags);
		}
	}

    // Postionieren wir grade ein neues Gebäude?
    Structure* structureBeingAdded = nullptr;
    if (guiMgr->getPanelState().selectedPanelButton == PanelButton::ADD_BUILDING) {
        int mapX, mapY;
        MapCoordUtils::getMapCoordsUnderMouse(mapX, mapY);

        StructureType structureType = guiMgr->getPanelState().addingStructure;
        unsigned char allowedToPlaceStructure = isAllowedToPlaceStructure(mapX, mapY, structureType);

        // Auf dem Ozean malen wir gar nix.
        // Is was im Weg, malen wir auch nicht. // TODO wir müssen eine flache Kachel malen, sonst sieht man ja gar nix
        if (!(allowedToPlaceStructure & (PLACING_STRUCTURE_OUTSIDE_OF_ISLE | PLACING_STRUCTURE_SOMETHING_IN_THE_WAY))) {
            MapObjectGraphic* graphic = graphicsMgr->getGraphicForStructure(structureType);

            int drawingFlags = MapObjectGraphic::DRAWING_FLAG_MASKED;
            if (allowedToPlaceStructure & PLACING_STRUCTURE_ROOM_NOT_UNLOCK) {
                drawingFlags |= MapObjectGraphic::DRAWING_FLAG_RED;
            }
            if (allowedToPlaceStructure & PLACING_STRUCTURE_NO_RESOURCES) {
                drawingFlags |= MapObjectGraphic::DRAWING_FLAG_BLINK;
            }

            // Zu zeichnendes Gebäude erstellen
            structureBeingAdded = new Structure();
            structureBeingAdded->setStructureType(structureType);
            structureBeingAdded->setMapCoords(mapX, mapY, graphic->getMapWidth(), graphic->getMapHeight());
            structureBeingAdded->setDrawingFlags(drawingFlags);
        }
    }

	// Objekte rendern
    memset(mapObjectAlreadyDrawnThere->data, 0, width * height * sizeof(char));

    // TODO Start und End noch ein wenig weiter ausweiten?
    for (int mapY = mapYStart; mapY <= mapYEnd; mapY++) {
        for (int mapX = mapXStart; mapX <= mapXEnd; mapX++) {
            MapObject* mapObject = getMapObjectAt(mapX, mapY);
            if (mapObject == nullptr) {
                // Positionieren wir hier ein neues Gebäude?
                if (structureBeingAdded != nullptr) {
                    int mx, my, mw, mh;
                    structureBeingAdded->getMapCoords(mx, my, mw, mh);
                    if (mapX >= mx && mapY >= my && mapX < mx + mw && mapY < my + mh) {
                        mapObject = structureBeingAdded;
                    }
                }

                if (mapObject == nullptr) {
                    continue; // nix zum Zeichen an dieser Stelle
                }
            }

            if (mapObjectAlreadyDrawnThere->getData(mapX, mapY, 1) == 1) {
                continue; // hier is schon bemalt, nix zu tun
            }

            // @see docs/drawing-order-x-tiles.xcf für Variablen

            // Ausrechnen, welchen Schnibbel der Grafik wir anzeigen müssen
            int moMapX, moMapY, moMapWidth, moMapHeight;
            mapObject->getMapCoords(moMapX, moMapY, moMapWidth, moMapHeight);

            int tileOffsetXInMapObject = mapX - moMapX; // (0 ... moMapWidth-1)
            int tileOffsetYInMapObject = mapY - moMapY; // (0 ... moMapHeight-1)

            Structure* structure = dynamic_cast<Structure*>(mapObject); // TODO nullptr sollte nicht passieren; später checken, wenn wir Bäume und sowas haben
            int drawingFlags = structure->getDrawingFlags();

            StructureType structureType = structure->getStructureType();

            // Sonderfall: Straße
            if (structureType == StructureType::STREET) {
                structureType = getConcreteStreetStructureType(mapX, mapY, structureType);
            }

            MapObjectGraphic* mapObjectGraphic = graphicsMgr->getGraphicForStructure(structureType);

            int xInMapObject =
                ((moMapHeight - 1) - tileOffsetYInMapObject + tileOffsetXInMapObject) * GraphicsMgr::TILE_WIDTH_HALF;
            int yInMapObject = mapObjectGraphic->getHeight() -
                ((moMapHeight - 1) - tileOffsetYInMapObject + (moMapWidth - 1) - tileOffsetXInMapObject + 2) *
                    GraphicsMgr::TILE_HEIGHT_HALF;

            SDL_Rect rectSource = { xInMapObject, 0, GraphicsMgr::TILE_WIDTH, mapObjectGraphic->getHeight() };
            SDL_Rect rectDestination = { 0, 0, rectSource.w / screenZoom, rectSource.h / screenZoom };
            MapCoordUtils::mapToScreenCoords(mapX, mapY, rectDestination.x, rectDestination.y);

            rectDestination.x -= screenOffsetX;
            rectDestination.y -= screenOffsetY;

            rectDestination.y -= yInMapObject;

            const int elevation = 1; // TODO für Gebäude wie Anlegestelle, Fischerhütte etc. muss auf 0 gesetzt werden
            rectDestination.y -= elevation * GraphicsMgr::ELEVATION_HEIGHT;

            rectDestination.x /= screenZoom;
            rectDestination.y /= screenZoom;

            if (selectedMapObject != nullptr) {
                Building* selectedBuilding = dynamic_cast<Building*>(selectedMapObject);
                bool insideCatchmentArea =
                    (selectedBuilding != nullptr && selectedBuilding->isInsideCatchmentArea(structure));

                if (!insideCatchmentArea) {
                    drawingFlags |= MapObjectGraphic::DRAWING_FLAG_DARKENED;
                }
            }
            mapObjectGraphic->draw(&rectSource, &rectDestination, drawingFlags);

            // In mapObjectAlreadyDrawnThere die Kacheln-Spalte als erledigt markieren
            do {
                mapObjectAlreadyDrawnThere->setData(moMapX + tileOffsetXInMapObject, moMapY + tileOffsetYInMapObject, 1);

                tileOffsetXInMapObject++;
                tileOffsetYInMapObject++;
            } while(tileOffsetXInMapObject < moMapWidth && tileOffsetYInMapObject < moMapHeight);
        }
    }

    // Träger zeichnen
    // TODO jetzt müssen wir doch wieder alle Map-Objekte durchloopen, weil die Träger nicht auf MapTile sind. Das sollte viel hübscher gemacht werden
    for (auto iter = mapObjects.cbegin(); iter != mapObjects.cend(); iter++) {
        MapObject* mapObject = *iter;
        Building* building = dynamic_cast<Building*>(mapObject);
        if (building == nullptr) {
            continue;
        }

        Carrier* carrier = building->carrier;
        if (carrier == nullptr) {
            continue;
        }

        int mapX, mapY;
        carrier->getMapCoords(mapX, mapY);

        Animation* animation = carrier->getAnimation();

        SDL_Rect rect;
        double mapXExact = (double) mapX + carrier->mapXFraction;
        double mapYExact = (double) mapY + carrier->mapYFraction;

        MapCoordUtils::mapToDrawCoords(mapXExact, mapYExact, 1, animation, &rect);

        animation->drawFrameScaledAt(
            rect.x, rect.y, (double) 1 / (double) screenZoom, (unsigned int) carrier->animationFrame);
    }

    // structureBeingAdded gesetzt?
    if (structureBeingAdded != nullptr) {
        // Einzugsbereich jetzt malen, damit er oben drauf is
        drawCatchmentArea(structureBeingAdded);
        delete structureBeingAdded;
    }

#ifdef DEBUG_A_STAR
    Route* routeToDraw = AStar::debugAStar_route;

    // Wenn Gebäude ausgewählt -> Trägerroute anzeigen
    const Building* selectedBuilding = dynamic_cast<const Building*>(getSelectedMapObject());
    if (selectedBuilding != nullptr) {
        routeToDraw = (selectedBuilding->carrier != nullptr) ? selectedBuilding->carrier->route : nullptr;
    }

    // A*-Route zeichnen (nur bei Maximalzoom, dann sparen wir uns Berechnungen und der Code wird einfacher)
    if (screenZoom == 1 && routeToDraw != nullptr) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        int lastPointX = -1;
        int lastPointY = -1;

        int i = 1;
        for (auto iter = routeToDraw->cbegin(); iter != routeToDraw->cend(); iter++) {
            MapCoordinate mapCoordinate = *iter;

            int screenX, screenY;
            MapCoordUtils::mapToScreenCoordsCenter(mapCoordinate.mapX, mapCoordinate.mapY, screenX, screenY);

            screenX -= screenOffsetX;
            screenY -= screenOffsetY;

            screenY -= GraphicsMgr::ELEVATION_HEIGHT;

            // Verbindungslinie zuerst
            if (lastPointX != -1) {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 128);
                SDL_RenderDrawLine(renderer, lastPointX, lastPointY, screenX, screenY);
            }
            lastPointX = screenX;
            lastPointY = screenY;

            // dann Rechteck mit Zahl drin
            SDL_Rect rect = { screenX - 8, screenY - 8, 16, 16 };
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 128);
            SDL_RenderFillRect(renderer, &rect);

            static SDL_Color colorWhite = {255, 255, 255, 255};
            fontMgr->renderText(renderer, toString(i++), screenX, screenY, &colorWhite, nullptr,
                "DroidSans.ttf", 9, RENDERTEXT_HALIGN_CENTER | RENDERTEXT_VALIGN_MIDDLE);
        }
    }
#endif

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
        const BuildingCosts* buildingCosts = configMgr->getBuildingConfig(structureType)->getBuildingCosts();
        if ((buildingCosts->coins > currentPlayer->coins) ||
            (buildingCosts->tools > colony->getGoods(GoodsType::TOOLS).inventory) ||
            (buildingCosts->wood > colony->getGoods(GoodsType::WOOD).inventory) ||
            (buildingCosts->bricks > colony->getGoods(GoodsType::BRICKS).inventory)) {
                result |= PLACING_STRUCTURE_NO_RESOURCES;
        }
    }
    
    // Checken, ob alles frei is, um das Gebäude zu setzen
    MapObjectGraphic* graphic = graphicsMgr->getGraphicForStructure(structureType);
    for (int y = mapY; y < mapY + graphic->getMapHeight(); y++) {
        for (int x = mapX; x < mapX + graphic->getMapWidth(); x++) {
            MapTile* mapTile = mapTiles->getData(x, y, nullptr);

            if (mapTile->mapObject != nullptr) {                  // Da steht was im Weg
                result |= PLACING_STRUCTURE_SOMETHING_IN_THE_WAY;
                return result;
            }

            const MapTileConfig* mapTileConfig = configMgr->getMapTileConfig(mapTile->tileIndex);

            if (mapTile->player == nullptr ||                     // Gebiet nicht erschlossen, ..,
                mapTile->player != game->getCurrentPlayer() ||    // ... nicht unseres...
                !mapTileConfig->isWalkableAndBuildable) {         // ... oder Gelände passt nicht

                result |= PLACING_STRUCTURE_ROOM_NOT_UNLOCK;
                return result;
            }
        }
    }
    
    return result;
}

void Map::drawCatchmentArea(Structure* structure) {
    SDL_SetRenderDrawColor(renderer, 0xc8, 0xaf, 0x37, 255);

    const BuildingConfig* buildingConfig = configMgr->getBuildingConfig(structure->getStructureType());
    const RectangleData<char>* catchmentArea = buildingConfig->getCatchmentArea();
    if (catchmentArea != nullptr) {
        for (int y = 0; y < catchmentArea->height; y++) {
            for (int x = 0; x < catchmentArea->width; x++) {
                int mapX, mapY, mapWidth, mapHeight;
                structure->getMapCoords(mapX, mapY, mapWidth, mapHeight);

                mapX += x - (catchmentArea->width - mapWidth) / 2;
                mapY += y - (catchmentArea->height - mapHeight) / 2;

                int screenX, screenY;
                MapCoordUtils::mapToScreenCoords(mapX, mapY, screenX, screenY);

                int drawX = (screenX - screenOffsetX) / screenZoom;
                int drawY = (screenY - screenOffsetY - GraphicsMgr::ELEVATION_HEIGHT) / screenZoom;

                // An der Kachel jede der 4 Seiten untersuchen, ob wir eine Linie malen müssen.
                // TODO die String-'1'er ersetzen durch echte 1en.

                // Oben rechts
                if (catchmentArea->getData(x, y - 1, '0') == '0' && catchmentArea->getData(x, y, '0') == '1') {
                    SDL_RenderDrawLine(renderer,
                            drawX + GraphicsMgr::TILE_WIDTH_HALF / screenZoom, drawY,
                            drawX + GraphicsMgr::TILE_WIDTH / screenZoom,
                            drawY + GraphicsMgr::TILE_HEIGHT_HALF / screenZoom);
                }

                // Oben links
                if (catchmentArea->getData(x - 1, y, '0') == '0' && catchmentArea->getData(x, y, '0') == '1') {
                    SDL_RenderDrawLine(renderer,
                            drawX, drawY + GraphicsMgr::TILE_HEIGHT_HALF / screenZoom,
                            drawX + GraphicsMgr::TILE_WIDTH_HALF / screenZoom, drawY);
                }

                // Unten rechts
                if (catchmentArea->getData(x, y, '0') == '1' && catchmentArea->getData(x + 1, y, '0') == '0') {
                    SDL_RenderDrawLine(renderer,
                            drawX + GraphicsMgr::TILE_WIDTH_HALF / screenZoom,
                            drawY + GraphicsMgr::TILE_HEIGHT / screenZoom,
                            drawX + GraphicsMgr::TILE_WIDTH / screenZoom,
                            drawY + GraphicsMgr::TILE_HEIGHT_HALF / screenZoom);
                }

                // Unten links
                if (catchmentArea->getData(x, y, '0') == '1' && catchmentArea->getData(x, y + 1, '0') == '0') {
                    SDL_RenderDrawLine(renderer,
                            drawX, drawY + GraphicsMgr::TILE_HEIGHT_HALF / screenZoom,
                            drawX + GraphicsMgr::TILE_WIDTH_HALF / screenZoom,
                            drawY + GraphicsMgr::TILE_HEIGHT / screenZoom);
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
    // Objekt in die Liste einreihen
    mapObjects.push_front(mapObject);

    int mapX, mapY, mapWidth, mapHeight;
    mapObject->getMapCoords(mapX, mapY, mapWidth, mapHeight);

    // Fläche auf den MapTiles als belegt markieren
    for (int my = mapY; my < mapY + mapHeight; my++) {
        for (int mx = mapX; mx < mapX + mapWidth; mx++) {
            getMapTileAt(mx, my)->mapObject = mapObject;
        }
    }
}

const Structure* Map::addStructure(int mapX, int mapY, StructureType structureType, Player* player) {
    MapObjectGraphic* graphic = graphicsMgr->getGraphicForStructure(structureType);
    
	// Objekt anlegen und in die Liste aufnehmen
	Structure* structure = new Structure();
	structure->setMapCoords(mapX, mapY, graphic->getMapWidth(), graphic->getMapHeight());
	structure->setStructureType(structureType);
    structure->setPlayer(player);

	addMapObject(structure);

	return structure;
}

const Building* Map::addBuilding(int mapX, int mapY, StructureType structureType, Player* player) {
    MapObjectGraphic* graphic = graphicsMgr->getGraphicForStructure(structureType);
    
	// Objekt anlegen
	Building* building = new Building();
	building->setMapCoords(mapX, mapY, graphic->getMapWidth(), graphic->getMapHeight());
	building->setStructureType(structureType);
    building->setPlayer(player);

    // Defaults für Produktionsdaten setzen
    const BuildingConfig* buildingConfig = configMgr->getBuildingConfig(structureType);
    building->productionSlots = ProductionSlots(buildingConfig->buildingProduction);

    // Objekt in die Liste aufnehmen
	addMapObject(building);
    
    // Kontor oder Marktplatz? Einzugbereich in mapTiles aktualisieren und Lagerkapazität der Kolonie erhöhen
    if (structureType == OFFICE1 || structureType == MARKETPLACE) {
        addOfficeCatchmentAreaToMap(*building);

        Colony* colony = game->getColony(building);
        colony->increaseGoodsCapacity(10);
    }

	return building;
}

void Map::addOfficeCatchmentAreaToMap(const Building& building) {
    int buildingMapX, buildingMapY;
    building.getMapCoords(buildingMapX, buildingMapY);
    
    const BuildingConfig* buildingConfig = configMgr->getBuildingConfig(building.getStructureType());
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

    // mapObjectAlreadyDrawnThere wegräumen
    if (mapObjectAlreadyDrawnThere != nullptr) {
        delete mapObjectAlreadyDrawnThere;
        mapObjectAlreadyDrawnThere = nullptr;
    }

    // Inseln wegräumen
    for (auto iter = isles.cbegin(); iter != isles.cend(); iter++) {
        Isle* isle = *iter;
        delete isle;
    }
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
	// Grade beim Platzieren eines neuen Gebäudes?
    if (guiMgr->getPanelState().selectedPanelButton == PanelButton::ADD_BUILDING) {
        int mapX, mapY;
        MapCoordUtils::getMapCoordsUnderMouse(mapX, mapY);
        
        onClickInMapWhileAddingStructure(mapX, mapY);
        return;
    }

	// Gucken, ob ein Gebäude geklickt wurde.
	// Objekte dabei rückwärts iterieren. Somit kommen "oben liegende" Objekte zuerst dran.
    // TODO Überarbeiten, da wir keine Reihenfolge mehr auf den Objekten haben!
	for (auto iter = mapObjects.crbegin(); iter != mapObjects.crend(); iter++) {
		MapObject* mapObject = *iter;

		// TODO hier später weitere Typen handeln oder cleverer in Objekt-Methoden arbeiten
		Building* building = dynamic_cast<Building*>(mapObject);
		if (building == nullptr) {
			// Da wir nur die Buildings durchgehen und nicht alle Structures, haben wir den positiven Nebeneffekt,
			// dass wir z.B. durch eine Mauer durchklicken und ein verstecktes Gebäude dahinter anklicken können.
			continue;
		}

        SDL_Rect rect;
        MapCoordUtils::getDrawCoordsForBuilding(building, &rect);

        // Außerhalb der Boundary-Box des Objekt geklickt?
        // TODO für Gebäude mit elevation=0 ggf. anpassen
		if (mouseX < rect.x) {
			continue;
		}
		if (mouseX >= rect.x + rect.w) {
			continue;
		}
		if (mouseY < rect.y) {
			continue;
		}
		if (mouseY >= rect.y + rect.h) {
			continue;
		}

		// Pixelfarbwert holen
		Uint8 r, g, b, a;
		int x = (mouseX - rect.x) * screenZoom;
		int y = (mouseY - rect.y) * screenZoom;
        MapObjectGraphic* graphic = graphicsMgr->getGraphicForStructure(building->getStructureType());
        graphic->getPixel(x, y, &r, &g, &b, &a);

		// Checken, ob Pixel un-transparent genug ist, um es als Treffer zu nehmen
		if (a > 127) {
			building->onClick(x, y);
			return;
		}
	}

	// TODO später ggf. weitere Events
    setSelectedMapObject(nullptr);
}

void Map::onClickInMapWhileAddingStructure(int mapX, int mapY) {
    StructureType structureType = guiMgr->getPanelState().addingStructure;
    if (isAllowedToPlaceStructure(mapX, mapY, structureType) != PLACING_STRUCTURE_ALLOWED) {
        // Dürfen wir hier/jetzt nicht setzen, ignorieren wir den Klick
        return;
    }
    
    // Gebäude platzieren und Modus verlassen
    Player* currentPlayer = game->getCurrentPlayer();
    if (structureType >= START_BUILDINGS) {
        // Sonderfall Haus: Wir wählen zufällig eins aus
        if (structureType == StructureType::PIONEERS_HOUSE1) {
            std::random_device randomDevice;
            std::default_random_engine randomEngine(randomDevice());
            std::uniform_int_distribution<int> randomPioneerHouse(
                StructureType::PIONEERS_HOUSE1, StructureType::PIONEERS_HOUSE5);

            structureType = (StructureType) randomPioneerHouse(randomEngine);
        }

        addBuilding(mapX, mapY, structureType, currentPlayer);
    } else {
        addStructure(mapX, mapY, structureType, currentPlayer);
    }
    
    // Resourcen bezahlen
    Colony* colony = game->getColony(currentPlayer, getMapTileAt(mapX, mapY)->isle);
    const BuildingCosts* buildingCosts = configMgr->getBuildingConfig(structureType)->getBuildingCosts();
    currentPlayer->coins -= buildingCosts->coins;
    colony->subtractBuildingCosts(buildingCosts);
}

void Map::onClickInMinimap(int mouseX, int mouseY) {
    int xInMinimap = mouseX - minimapClipRect.x;
    int yInMinimap = mouseY - minimapClipRect.y;
    
    float scaleFactor = (float) width / (float) minimapClipRect.w;
    
    int mapX = (int) ((float) xInMinimap  * scaleFactor);
    int mapY = (int) ((float) yInMinimap  * scaleFactor);
    
    int screenX, screenY;
    MapCoordUtils::mapToScreenCoords(mapX, mapY, screenX, screenY);
    
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

    // Objekt aus der Liste entfernen
    mapObjects.remove(selectedMapObject);

    // Fläche auf den MapTiles freigeben
    int mapX, mapY, mapWidth, mapHeight;
    selectedMapObject->getMapCoords(mapX, mapY, mapWidth, mapHeight);

    for (int my = mapY; my < mapY + mapHeight; my++) {
        for (int mx = mapX; mx < mapX + mapWidth; mx++) {
            getMapTileAt(mx, my)->mapObject = nullptr;
        }
    }

    // Kontor oder Marktplatz? Einzugbereich in mapTiles aktualisieren und Lagerkapazität der Kolonie erhöhen
    Structure* selectedStructure = dynamic_cast<Structure*>(selectedMapObject);
    if (selectedStructure != nullptr) {
        StructureType structureType = selectedStructure->getStructureType();
        if (structureType == MARKETPLACE) {
            Colony* colony = game->getColony(selectedStructure);
            colony->decreaseGoodsCapacity(10);
        }
    }

    // TODO Wenn wir wirklich ordentlich ein Gebäude abreißen, müssen bei Marktplatz/Kontor die Kolonie-Einzugsbereiche
    // aktualisiert, die Gebäude außerhalb, die nicht in einem anderen Einzugsgebiet liegen, zerstört werden.
    // Beim Abriss des Kontors muss zusätzlich die Kolonie zerstört werden.


    delete selectedMapObject;

    setSelectedMapObject(nullptr);
}

StructureType Map::getConcreteStreetStructureType(int mapX, int mapY, StructureType abstractStreetStructureType) {
    if (abstractStreetStructureType == StructureType::STREET) {
        static const StructureType structureTypeMap[16] = {
            STREET_STRAIGHT_90, // oder STREET_STRAIGHT_0, is in diesem Fall egal. TODO Sollte später mit der Drehfunktion genauer untersucht werden
            STREET_STRAIGHT_90,
            STREET_STRAIGHT_0,
            STREET_CURVE_0,
            STREET_STRAIGHT_0,
            STREET_CURVE_270,
            STREET_STRAIGHT_0,
            STREET_TEE_90,
            STREET_STRAIGHT_90,
            STREET_STRAIGHT_90,
            STREET_CURVE_90,
            STREET_TEE_180,
            STREET_CURVE_180,
            STREET_TEE_0,
            STREET_TEE_270,
            STREET_CROSS
        };

        char isStreetAbove = isStreetAt(mapX, mapY - 1) ? 1 : 0; // Bit 0
        char isStreetRight = isStreetAt(mapX + 1, mapY) ? 1 : 0; // Bit 1
        char isStreetLeft = isStreetAt(mapX -1, mapY) ? 1 : 0;   // Bit 2
        char isStreetBelow = isStreetAt(mapX, mapY + 1) ? 1 : 0; // Bit 3

        int index = (isStreetBelow << 3) | (isStreetLeft << 2) | (isStreetRight << 1) | isStreetAbove;

        return structureTypeMap[index];
    }

    // TODO Feldweg
    throw new std::runtime_error("Illegal abstractStreetStructureType " + toString(abstractStreetStructureType));
}

bool Map::isStreetAt(int mapX, int mapY) {
    MapObject* mapObject = getMapObjectAt(mapX, mapY);
    if (mapObject == nullptr) {
        return false;
    }

    Structure* structure = dynamic_cast<Structure*>(mapObject);

    // TODO Feldweg
    return (structure->getStructureType() == StructureType::STREET);
}
