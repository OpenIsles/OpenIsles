#include "config/ConfigMgr.h"
#include "game/Colony.h"
#include "game/Game.h"
#include "map/Map.h"
#include "utils/StringFormat.h"
#include "utils/Rect.h"

#ifdef DEBUG_A_STAR
#include "graphics/mgr/IFontMgr.h"
#include "pathfinding/AStar.h"
#include "utils/Color.h"
#endif

Map::Map(const Context* const context) : ContextAware(context) {
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
    // TODO in die Config bringen. Aktuell is aber noch unklar, ob wir mehrere isOcean-Kacheln haben werden. oceanTileIndex und oceanTileGraphicSetName hängen über tiles.xml zusammen
    // hübscher machen
    const MapTileConfig* oceanMapTileConfig = context->configMgr->getMapTileConfigOcean();
    std::string oceanTileGraphicSetName = "tiles/water";
    const GraphicSet* oceanTileGraphicSet = context->graphicsMgr->getGraphicSet(oceanTileGraphicSetName);

    std::array<const Animation*, 4> tileAnimationsOcean = {
        oceanTileGraphicSet->getByView("south"),
        oceanTileGraphicSet->getByView("east"),
        oceanTileGraphicSet->getByView("north"),
        oceanTileGraphicSet->getByView("west")
    };

    mapTiles = new RectangleData<MapTile*>(newWidth, newHeight);
    for(int i = 0; i < mapTiles->width * mapTiles->height; i++) {
        mapTiles->data[i] = new MapTile(oceanMapTileConfig, tileAnimationsOcean);
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
    MapTile* mapTile = getMapTileAt(mapX, mapY);
    if (mapTile == nullptr) {
        return nullptr;
    }

    return mapTile->mapObject;
}

void Map::setSelectedMapObject(MapObject* selectedMapObject) {
    this->selectedMapObject = selectedMapObject;
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

void Map::addOfficeCatchmentAreaToMap(const Building& building) {
    int buildingMapX, buildingMapY;
    building.getMapCoords(buildingMapX, buildingMapY);
    
    const BuildingConfig* buildingConfig = context->configMgr->getBuildingConfig(building.getStructureType());
    RectangleData<char>* catchmentArea = buildingConfig->getCatchmentArea();
    
    // TODO Sehr hässlich, aber tuts erstmal sicher, ohne Gefahr.
    for (int mapY = buildingMapY - catchmentArea->height/2 - 1;
             mapY <= buildingMapY + catchmentArea->height/2 + 1; mapY++) {
        
        for (int mapX = buildingMapX - catchmentArea->width/2 - 1;
                 mapX <= buildingMapX + catchmentArea->width/2 + 1; mapX++) {
            
            if (!building.isInsideCatchmentArea(context->configMgr, mapX, mapY)) {
                continue;
            }
            
            MapTile* mapTile = mapTiles->getData(mapX, mapY, nullptr);
            if (mapTile == nullptr) {
                continue;
            }
            
            mapTile->player = building.getPlayer();
        }
    }
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
        for (int i = 0; i < mapTiles->width * mapTiles->height; i++) {
            delete mapTiles->data[i];
        }
        delete mapTiles;
        mapTiles = nullptr;
    }

    // Inseln wegräumen
    for (auto iter = isles.cbegin(); iter != isles.cend(); iter++) {
        Isle* isle = *iter;
        delete isle;
    }
    isles.clear();
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
