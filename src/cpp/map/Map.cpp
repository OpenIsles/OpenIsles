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
        throw std::runtime_error("Map has to be quadratically for now ;-p");
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
        oceanTileGraphicSet->getByView(Direction::SOUTH),
        oceanTileGraphicSet->getByView(Direction::EAST),
        oceanTileGraphicSet->getByView(Direction::NORTH),
        oceanTileGraphicSet->getByView(Direction::WEST)
    };

    mapTiles = new RectangleData<MapTile*>(newWidth, newHeight);
    for(int i = 0; i < mapTiles->width * mapTiles->height; i++) {
        mapTiles->data[i] = new MapTile(oceanMapTileConfig, tileAnimationsOcean);
    }

    // Sonstiges Zeugs auf Anfangswerte stellen
	selectedMapObject = nullptr;

	mapCoordsCentered.setTo(0, 0);
    screenView = Direction::SOUTH;
}

MapTile* Map::getMapTileAt(const MapCoords& mapCoords) const {
    return mapTiles->getData(mapCoords.x(), mapCoords.y(), nullptr);
}

MapObjectFixed* Map::getMapObjectAt(const MapCoords& mapCoords) const {
    MapTile* mapTile = getMapTileAt(mapCoords);
    if (mapTile == nullptr) {
        return nullptr;
    }

    return mapTile->mapObject;
}

void Map::setSelectedMapObject(MapObject* selectedMapObject) {
    this->selectedMapObject = selectedMapObject;
}

void Map::scroll(int xDelta, int yDelta) {
    int mapXOffset, mapYOffset;

    // TODO denselben Code haben wir auch in MapCoordsUtils
    if (screenView == Direction::SOUTH) {
        mapXOffset = xDelta;
        mapYOffset = yDelta;
    } else if (screenView == Direction::EAST) {
        mapXOffset = yDelta;
        mapYOffset = -xDelta;
    } else if (screenView == Direction::NORTH) {
        mapXOffset = -xDelta;
        mapYOffset = -yDelta;
    } else if (screenView == Direction::WEST) {
        mapXOffset = -yDelta;
        mapYOffset = xDelta;
    }
    else {
        assert(false);
        mapXOffset = mapYOffset = 0;
    }

    mapCoordsCentered.addX(mapXOffset);
    mapCoordsCentered.addY(mapYOffset);
}

void Map::addMapObject(MapObjectFixed* mapObject) {
    // Objekt in die Liste einreihen
    mapObjects.push_front(mapObject);

    const MapCoords& mapCoords = mapObject->getMapCoords();
    int mapWidth = mapObject->getMapWidth();
    int mapHeight = mapObject->getMapHeight();

    // Fläche auf den MapTiles als belegt markieren
    for (int my = mapCoords.y(); my < mapCoords.y() + mapHeight; my++) {
        for (int mx = mapCoords.x(); mx < mapCoords.x() + mapWidth; mx++) {
            getMapTileAt(MapCoords(mx, my))->mapObject = mapObject;
        }
    }
}

void Map::addOfficeCatchmentAreaToMap(const Building& building) {
    const MapCoords& mapCoords = building.getMapCoords();
    
    const BuildingConfig* buildingConfig = context->configMgr->getBuildingConfig(building.getStructureType());
    RectangleData<char>* catchmentArea = buildingConfig->getCatchmentArea();
    int catchmentAreaRadius = std::max(catchmentArea->width, catchmentArea->height); // TODO sehr optimierungsbedürftig, dafür funktionierts erstmal in allen Ansichten
    
    // TODO Sehr hässlich, aber tuts erstmal sicher, ohne Gefahr.
    for (int mapY = mapCoords.y() - catchmentAreaRadius; mapY <= mapCoords.y() + catchmentAreaRadius; mapY++) {
        for (int mapX = mapCoords.x() - catchmentAreaRadius; mapX <= mapCoords.x() + catchmentAreaRadius; mapX++) {
            
            if (!building.isInsideCatchmentArea(context->configMgr, MapCoords(mapX, mapY))) {
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

bool Map::isStreetAt(const MapCoords& mapCoords) {
    MapObjectFixed* mapObject = getMapObjectAt(mapCoords);
    if (mapObject == nullptr) {
        return false;
    }

    Structure* structure = dynamic_cast<Structure*>(mapObject);
    return ((structure != nullptr) && structure->isStreet());
}
