#include <cassert>
#include "global.h"
#include "config/ConfigMgr.h"
#include "game/CatchmentArea.h"
#include "game/Colony.h"
#include "game/Game.h"
#include "map/Map.h"
#include "map/Street.h"
#include "utils/CatchmentAreaIterator.h"
#include "utils/StringFormat.h"
#include "utils/Rect.h"

#ifdef DEBUG_A_STAR
#include "graphics/mgr/IFontMgr.h"
#include "pathfinding/AStar.h"
#include "utils/Color.h"
#endif

Map::Map(const Context& context) : ContextAware(context) {
}

Map::~Map() {
    clearMap();
}

void Map::initNewMap(int newWidth, int newHeight) {
    // TODO Wir gehen erstmal davon aus, dass die Karten immer quadratisch sind.
    // Damit spar ich mir erstmal Hirnschmalz mit der Minimap und anderem Zeug, was noch kommen wird.
    // Info: Anno-Karten sind 500x350 Kacheln groß (damit passt auch die Minimap besser)
    if (newWidth != newHeight) {
        Log::debug(_("Map has to be quadratically for now. ;-p"));
        throw std::runtime_error("Map has to be quadratically for now ;-p");
    }
    
    // Karte erst leerräumen
    clearMap();
    
    // Neue Größe setzen
    width = newWidth;
    height = newHeight;

    // mapTiles neu anlegen und mit Ozean initialisieren
    // TODO in die Config bringen. Aktuell is aber noch unklar, ob wir mehrere isOceanOnMinimap-Kacheln haben werden. oceanTileIndex und oceanTileGraphicSetName hängen über tiles.xml zusammen
    // hübscher machen
    const MapTileConfig* oceanMapTileConfig = context.configMgr->getMapTileConfigOcean();
    std::string oceanTileGraphicSetName = "tiles/water";
    const GraphicSet* oceanTileGraphicSet = context.graphicsMgr->getGraphicSet(oceanTileGraphicSetName);

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

MapObjectFixed* Map::getMapObjectFixedAt(const MapCoords& mapCoords) const {
    MapTile* mapTile = getMapTileAt(mapCoords);
    if (mapTile == nullptr) {
        return nullptr;
    }

    return mapTile->mapObjectFixed;
}

void Map::setSelectedMapObject(const MapObject* selectedMapObject) {
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

void Map::addMapObject(MapObject* mapObject) {
    // Objekt in die Liste einreihen
    mapObjects.push_front(mapObject);

    // Unterscheidung nach fixen und beweglichen Map-Objekten
    MapObjectFixed* mapObjectFixed = dynamic_cast<MapObjectFixed*>(mapObject);
    if (mapObjectFixed != nullptr) {
        const MapCoords& mapCoords = mapObjectFixed->getMapCoords();
        int mapWidth = mapObjectFixed->getMapWidth();
        int mapHeight = mapObjectFixed->getMapHeight();

        // Fläche auf den MapTiles als belegt markieren
        for (int my = mapCoords.y(); my < mapCoords.y() + mapHeight; my++) {
            for (int mx = mapCoords.x(); mx < mapCoords.x() + mapWidth; mx++) {
                getMapTileAt(MapCoords(mx, my))->mapObjectFixed = mapObjectFixed;
            }
        }

        return;
    }

    MapObjectMoving* mapObjectMoving = dynamic_cast<MapObjectMoving*>(mapObject);
    if (mapObjectMoving != nullptr) {
        // TODO Bewegliche Map-Objekte dürfen aktuell nur 1 Kachel groß sein. Später, wenn Schiffe da sind, müssen wir das erweitern.
        assert((mapObjectMoving->getMapWidth() == 1) && (mapObjectMoving->getMapHeight() == 1));

        // Fläche auf den MapTiles als belegt markieren
        const DoubleMapCoords& mapCoords = mapObjectMoving->getMapCoords();
        getMapTileAt(MapCoords(int(mapCoords.x()), int(mapCoords.y())))->mapObjectsMoving.push_back(mapObjectMoving);

        return;
    }

    assert(false);
}

void Map::addBuildingCatchmentAreaToBuildableArea(const Building& building) {
    CatchmentAreaIterator catchmentAreaIterator(building, false);
    catchmentAreaIterator.iterate([&](const MapCoords& mapCoords) {
        MapTile* mapTile = mapTiles->getData(mapCoords.x(), mapCoords.y(), nullptr);
        if (mapTile == nullptr) {
            return;
        }

        mapTile->player = building.getPlayer();
    });
}

void Map::moveMapObject(MapObjectMoving* mapObject, const DoubleMapCoords newMapCoords) {
    const int mapWidth = mapObject->getMapWidth();
    const int mapHeight = mapObject->getMapHeight();

    // TODO Bewegliche Map-Objekte dürfen aktuell nur 1 Kachel groß sein. Später, wenn Schiffe da sind, müssen wir das ggf. erweitern.
    assert((mapWidth == 1) && (mapHeight == 1));

    // Fläche auf den MapTiles ummarkieren.
    // Wir markieren mehrere Kacheln, wenn sich das Objekt zwischen zwei Kacheln befindet.
    const DoubleMapCoords& mapCoords = mapObject->getMapCoords();
    for (int my = int(std::floor(mapCoords.y())); my < int(std::ceil(mapCoords.y())) + mapHeight; my++) {
        for (int mx = int(std::floor(mapCoords.x())); mx < int(std::ceil(mapCoords.x())) + mapWidth; mx++) {
            getMapTileAt(MapCoords(mx, my))->mapObjectsMoving.remove(mapObject);
        }
    }

    mapObject->setMapCoords(newMapCoords);

    for (int my = int(std::floor(newMapCoords.y())); my < int(std::ceil(newMapCoords.y())) + mapHeight; my++) {
        for (int mx = int(std::floor(newMapCoords.x())); mx < int(std::ceil(newMapCoords.x())) + mapWidth; mx++) {
            getMapTileAt(MapCoords(mx, my))->mapObjectsMoving.push_back(mapObject);
        }
    }
}

void Map::deleteMapObject(MapObject* mapObject) {
    mapObjects.remove(mapObject);

    // Unterscheidung nach fixen und beweglichen Map-Objekten
    MapObjectFixed* mapObjectFixed = dynamic_cast<MapObjectFixed*>(mapObject);
    if (mapObjectFixed != nullptr) {
        const MapCoords& mapCoords = mapObjectFixed->getMapCoords();
        int mapWidth = mapObjectFixed->getMapWidth();
        int mapHeight = mapObjectFixed->getMapHeight();

        // Fläche auf den MapTiles als belegt markieren
        for (int my = mapCoords.y(); my < mapCoords.y() + mapHeight; my++) {
            for (int mx = mapCoords.x(); mx < mapCoords.x() + mapWidth; mx++) {
                getMapTileAt(MapCoords(mx, my))->mapObjectFixed = nullptr;
            }
        }
    }
    else {
        MapObjectMoving* mapObjectMoving = dynamic_cast<MapObjectMoving*>(mapObject);
        if (mapObjectMoving != nullptr) {
            // TODO Bewegliche Map-Objekte dürfen aktuell nur 1 Kachel groß sein. Später, wenn Schiffe da sind, müssen wir das erweitern.
            assert((mapObjectMoving->getMapWidth() == 1) && (mapObjectMoving->getMapHeight() == 1));

            // Fläche auf den MapTiles als belegt markieren
            const DoubleMapCoords& mapCoords = mapObjectMoving->getMapCoords();
            getMapTileAt(MapCoords(int(mapCoords.x()), int(mapCoords.y())))->mapObjectsMoving.remove(mapObjectMoving);
        }
        else {
            assert(false);
        }
    }

    delete mapObject;
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

bool Map::isStreetAt(const MapCoords& mapCoords) const {
    MapObjectFixed* mapObject = getMapObjectFixedAt(mapCoords);
    if (mapObject == nullptr) {
        return false;
    }

    return (dynamic_cast<Street*>(mapObject) != nullptr);
}

bool Map::isWalkableForCartAt(const MapCoords& mapCoords) const {
    MapObjectFixed* mapObject = getMapObjectFixedAt(mapCoords);
    if (mapObject == nullptr) {
        return false;
    }

    Structure* structure = dynamic_cast<Structure*>(mapObject);
    return ((structure != nullptr) && structure->isWalkableForCart());
}
