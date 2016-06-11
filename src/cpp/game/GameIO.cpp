#include <array>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <string>
#include "global.h"
#include "game/Colony.h"
#include "game/Game.h"
#include "game/GameIO.h"
#include "map/coords/ScreenCoords.h"
#include "map/Map.h"
#include "utils/Consts.h"
#include "utils/StringFormat.h"


// TODO allgemein: Fehlermanagement, wenn die Datei mal nicht so hübsch aussieht, dass alle Tags da sind / oder die Datei gar nicht da ist

void GameIO::loadGameFromTMX(
    Game* game, const ConfigMgr* configMgr, const IGraphicsMgr* graphicsMgr, const char* filename) {

    // TODO aktuell kann diese Methode nur mit einem frischen Game-Objekt aufgerufen werden. Nochmal laden = vorher saubermachen

    // Datei öffnen
    rapidxml::file<> tmxFile(filename);

    rapidxml::xml_document<>* xmlDocument = new rapidxml::xml_document<>();
    xmlDocument->parse<0>(tmxFile.data());

    // Kartengröße einlesen und Map anlegen
    rapidxml::xml_node<>* mapNode = xmlDocument->first_node("map", 3, true);
    int newMapWidth = std::atoi(mapNode->first_attribute("width", 5, true)->value());
    int newMapHeight = std::atoi(mapNode->first_attribute("height", 6, true)->value());
    game->getMap()->initNewMap(newMapWidth, newMapHeight);

    // Ebenen durchgehen.
    rapidxml::xml_node<>* objectgroupIslesNode = nullptr;
    rapidxml::xml_node<>* objectgroupColoniesNode = nullptr;
    rapidxml::xml_node<>* objectgroupMapObjectsNode = nullptr;

    for (rapidxml::xml_node<>* objectgroupNode = mapNode->first_node("objectgroup", 11, true); objectgroupNode != nullptr;
         objectgroupNode = objectgroupNode->next_sibling("objectgroup", 11, true)) {

        const char* layerName = objectgroupNode->first_attribute("name", 4, true)->value();

        if (std::strcmp(layerName, "isles") == 0) {
            objectgroupIslesNode = objectgroupNode;
        } else if (std::strcmp(layerName, "colonies") == 0) {
            objectgroupColoniesNode = objectgroupNode;
        } else if (std::strcmp(layerName, "map-objects") == 0) {
            objectgroupMapObjectsNode = objectgroupNode;
        }
    }

    // TODO aus Savegame laden. Vorsicht: GameTest beachten. Tests laden auch Savegames und brauchen klare Zeitangaben (Game.ticks, MapObjects.lastUpdateTicks)
    game->ticks = 0;

    // Spieler laden
    loadPlayers(game, mapNode);

    // Karte und Inseln laden
    loadMap(game, configMgr, graphicsMgr, objectgroupIslesNode);

    // Siedlungen laden
    loadColonies(game, configMgr, objectgroupColoniesNode);

    // Strukturen und Gebäude laden
    loadMapObjects(game, configMgr, objectgroupMapObjectsNode);

    // XML-Document wegräumen
    delete xmlDocument;
}

void GameIO::loadPlayers(Game* game, rapidxml::xml_node<>* mapNode) {
    rapidxml::xml_node<>* propertiesNode = mapNode->first_node("properties", 10, true);

    int currentPlayerNr = std::atoi(getPropertyValueFromPropertiesNode(propertiesNode, "current-player"));

    for (int i = 1; i <= 4; i++) {
        std::string playerAttrPrefix = "player" + toString(i) + "-";

        // Spielerfarbe
        std::string playerColorAttrName = playerAttrPrefix + "color";
        const char* playerColorAttrValue = getPropertyValueFromPropertiesNode(propertiesNode, playerColorAttrName.c_str());
        PlayerColor playerColor;

        if (std::strcmp(playerColorAttrValue, "red") == 0) {
            playerColor = PlayerColor::RED;
        } else if (std::strcmp(playerColorAttrValue, "blue") == 0) {
            playerColor = PlayerColor::BLUE;
        } else if (std::strcmp(playerColorAttrValue, "yellow") == 0) {
            playerColor = PlayerColor::YELLOW;
        } else if (std::strcmp(playerColorAttrValue, "white") == 0) {
            playerColor = PlayerColor::WHITE;
        } else {
            Log::error(_("Illegal player color '%s'."), playerColorAttrValue);
            throw std::runtime_error("Illegal player color");
        }

        // Spielername
        std::string playerNameAttrName = playerAttrPrefix + "name";
        std::string playerName = getPropertyValueFromPropertiesNode(propertiesNode, playerNameAttrName.c_str());

        // Spieler anlegen
        Player* player = new Player(playerColor, playerName);
        game->addPlayer(player);

        if (i == currentPlayerNr) {
            game->setCurrentPlayer(player);
        }
    }
}

void GameIO::loadMap(
    Game* game, const ConfigMgr* configMgr, const IGraphicsMgr* graphicsMgr, rapidxml::xml_node<>* objectgroupIslesNode) {

    Map* map = game->getMap();

    // Inseln einlesen
    for (rapidxml::xml_node<>* objectNode = objectgroupIslesNode->first_node("object", 6, true); objectNode != nullptr;
         objectNode = objectNode->next_sibling("object", 6, true)) {

        const char* nodeType = objectNode->first_attribute("type", 4, true)->value();

        MapCoords mapCoords = getMapCoordsFromObjectNode(objectNode);

        // Insel
        if (std::strcmp(nodeType, "isle") == 0) {
            // Objekt aus der Tiled-Datei lesen
            const char* isleName = objectNode->first_attribute("name", 4, true)->value();

            int isleWidth = std::atoi(objectNode->first_attribute("width", 5, true)->value());
            int isleHeight = std::atoi(objectNode->first_attribute("height", 6, true)->value());

            int isleMapWidth = isleWidth / IGraphicsMgr::TILE_HEIGHT; // tiled rechnet merkwürdigerweise auch für X in KachelHÖHE
            int isleMapHeight = isleHeight / IGraphicsMgr::TILE_HEIGHT;

            // Dateiname der Insel zusammenbauen
            std::string isleFilename = "data/map/isles/";
            isleFilename.append(isleName);
            isleFilename.append(".tmx");

            // Insel laden
            Isle* isle = new Isle(isleFilename.c_str());

            // Prüfen, ob die Insel wirklich die Größe hat, wie die Karte sie haben will.
            if (isle->getWidth() != isleMapWidth || isle->getHeight() != isleMapHeight) {
                Log::error(_("Isle '%s' (%dx%d) does not match size on map (%dx%d)."),
                           isleName, isle->getWidth(), isle->getHeight(), isleMapWidth, isleMapHeight);
                throw std::runtime_error("Isle does not match size on map");
            }

            // Insel zur Karte hinzufügen
            isle->setMapCoords(mapCoords);
            isle->setMapWidth(isleMapWidth);
            isle->setMapHeight(isleMapHeight);
            map->isles.push_back(isle);

            for (int my = mapCoords.y(), isleY = 0; my < mapCoords.y() + isleMapHeight; my++, isleY++) {
                for (int mx = mapCoords.x(), isleX = 0; mx < mapCoords.x() + isleMapWidth; mx++, isleX++) {
                    MapTile* mapTile = map->mapTiles->getData(mx, my, nullptr);
                    if (mapTile == nullptr) {
                        continue; // TODO Warnung ausgeben, dass die Map nicht ok is. Wir haben eine Insel, die außerhalb der Karte liegt
                    }

                    unsigned char tileIndex = isle->getTileAt(isleX, isleY);
                    const MapTileConfig* mapTileConfig = configMgr->getMapTileConfigByTiledId(tileIndex);
                    unsigned char viewOffset = configMgr->getViewOffsetByTiledId(tileIndex);
                    std::string tileGraphicSetName = "tiles/" + mapTileConfig->tileName;
                    const GraphicSet* tileGraphicSet = graphicsMgr->getGraphicSet(tileGraphicSetName);

                    std::array<const Animation*, 4> tileAnimations = {
                        tileGraphicSet->getByView(Direction::add90DegreeOffset(Direction::SOUTH, viewOffset)),
                        tileGraphicSet->getByView(Direction::add90DegreeOffset(Direction::EAST, viewOffset)),
                        tileGraphicSet->getByView(Direction::add90DegreeOffset(Direction::NORTH, viewOffset)),
                        tileGraphicSet->getByView(Direction::add90DegreeOffset(Direction::WEST, viewOffset))
                    };

                    mapTile->setTile(mapTileConfig, tileAnimations);
                    mapTile->isle = isle;
                }
            }
        }

            // Startpunkt: Diesen Punkt wollen wir auf den Bildschirm zentrieren
        else if (std::strcmp(nodeType, "startpoint") == 0) {
            map->setMapCoordsCentered(mapCoords);
        }
    }
    map->screenZoom = 1;
    map->screenView = Direction::SOUTH;
}

/* TODO Zugehörigkeit "Kolonie <-> Map-Kachel" ins Savegame legen!
 *
 * Hintergrund: Da mit MapObjectFixed.getColony() nun jedes Gebäude weiß, zu welcher Kolonie es gehört, muss beim
 * Setzen eines neuen Objekts klar sein, welche Kolonie auf welcher Kachel ist.
 * Wir haben aktuell kein Problem, weil in allen Savegames die Kontore und Marktplätze als erstes kommen.
 * Doof wirds, wenn das mal nicht so ist, weil dann Gebäude eine nullptr-Colony im MapObjectFixed stehen haben,
 * weil der zugehörige Marktplatz/Kontor erst später kommt und das Gebäude das nicht mehr mitkriegt!
 */

void GameIO::loadColonies(Game* game, const ConfigMgr* configMgr, rapidxml::xml_node<>* objectgroupColoniesNode) {
    Map* map = game->getMap();

    for (rapidxml::xml_node<>* objectNode = objectgroupColoniesNode->first_node("object", 6, true); objectNode != nullptr;
         objectNode = objectNode->next_sibling("object", 6, true)) {

        const char* nodeType = objectNode->first_attribute("type", 4, true)->value();
        if (std::strcmp(nodeType, "colony") != 0) {
            continue;
        }

        MapCoords mapCoords = getMapCoordsFromObjectNode(objectNode);
        std::string colonyName = objectNode->first_attribute("name", 4, true)->value();

        rapidxml::xml_node<>* propertiesNode = objectNode->first_node("properties", 10, true);
        int playerNr = std::atoi(getPropertyValueFromPropertiesNode(propertiesNode, "player"));
        Player* player = game->getPlayer(playerNr - 1);
        MapTile* mapTile = map->mapTiles->getData(mapCoords.x(), mapCoords.y(), nullptr);

        // Siedlung anlegen
        Colony* colony = game->foundNewColony(player, colonyName, mapTile->isle);

        // Waren
        const std::unordered_map<std::string, Good>& allGoods = configMgr->getAllGoods();
        for (auto iter = allGoods.cbegin(); iter != allGoods.cend(); iter++) {
            const Good& good = iter->second;

            if (good.invisible) {
                continue;
            }

            std::string xmlAttributeName = "goods-" + good.name;
            const char* xmlAttributeValue = getPropertyValueFromPropertiesNode(propertiesNode, xmlAttributeName.c_str());
            if (xmlAttributeValue != nullptr) {
                colony->getGoods(&good).inventory = (unsigned int) std::atoi(xmlAttributeValue);
            } else {
                colony->getGoods(&good).inventory = 0;
                Log::error(_("Warning: Did not find Good '%s' in savefile."), good.name.c_str());
            }
        }
    }
}

void GameIO::loadMapObjects(Game* game, const ConfigMgr* configMgr, rapidxml::xml_node<>* objectgroupMapObjectsNode) {
    for (rapidxml::xml_node<>* objectNode = objectgroupMapObjectsNode->first_node("object", 6, true);
         objectNode != nullptr;
         objectNode = objectNode->next_sibling("object", 6, true)) {

        // Allgemeine Daten einlesen
        const char* nodeNameValue = objectNode->first_attribute("name", 4, true)->value();

        const MapObjectType* mapObjectType = configMgr->getMapObjectType(nodeNameValue);
        if (mapObjectType == nullptr) {
            Log::error(_("Illegal mapObjectType '%s'."), nodeNameValue);
            throw std::runtime_error("Illegal mapObjectType");
        }
        const MapObjectTypeClass& mapObjectTypeClass = mapObjectType->type;

        MapCoords mapCoords = getMapCoordsFromObjectNode(objectNode);

        rapidxml::xml_node<>* propertiesNode = objectNode->first_node("properties", 10, true);
        EighthDirection view = getViewPropertyValueFromPropertiesNode(propertiesNode, "view");

        if (mapObjectTypeClass == MapObjectTypeClass::STRUCTURE ||
            mapObjectTypeClass == MapObjectTypeClass::BUILDING ||
            mapObjectTypeClass == MapObjectTypeClass::STREET) {

            int playerNr = std::atoi(getPropertyValueFromPropertiesNode(propertiesNode, "player"));
            Player* player = game->getPlayer(playerNr - 1);

            if (mapObjectTypeClass == MapObjectTypeClass::STRUCTURE ||
                mapObjectTypeClass == MapObjectTypeClass::BUILDING) {

                game->addStructure(mapCoords, mapObjectType, view, player);
            }
            else if (mapObjectTypeClass == MapObjectTypeClass::STREET) {
                const char* connections = getPropertyValueFromPropertiesNode(propertiesNode, "connections");
                StreetConnections streetConnections(connections);

                game->addStreet(mapCoords, mapObjectType, view, player, streetConnections);
            }
        }

        // Schiff anlegen
        else if (mapObjectTypeClass == MapObjectTypeClass::SHIP) {
            EighthDirection direction = getViewPropertyValueFromPropertiesNode(propertiesNode, "direction");

            int playerNr = std::atoi(getPropertyValueFromPropertiesNode(propertiesNode, "player"));
            Player* player = game->getPlayer(playerNr - 1);

            Ship* ship = game->addShip(mapCoords, mapObjectType, direction, player);

            // Name
            std::string name = std::string(getPropertyValueFromPropertiesNode(propertiesNode, "name"));
            ship->setName(name);

            // Warenslots
            unsigned char slotNumber = 0;
            for (unsigned char i = 1; i <= mapObjectType->goodsSlots; i++) {
                std::string propertyName = std::string("goods-on-board") + toString(i) + ".good";
                const char* propertyValue = getPropertyValueFromPropertiesNode(propertiesNode, propertyName.c_str());

                if (propertyValue == nullptr) {
                    continue; // Slot nicht belegt
                }

                const Good* good = configMgr->getGood(propertyValue);
                if (good == nullptr) {
                    Log::error(_("Warning: Ship has loaded unknown Good '%s'."), propertyValue);
                    continue;
                }

                propertyName = std::string("goods-on-board") + toString(i) + ".inventory";
                int inventory = std::atoi(getPropertyValueFromPropertiesNode(propertiesNode, propertyName.c_str()));
                if ((inventory <= 0) || (inventory > 50)) {
                    Log::error(_("Warning: Ship has illegal inventory %d for Good '%s'."),
                               inventory, good->name.c_str());
                    continue;
                }

                ship->goodsOnBoard[slotNumber].good = good;
                ship->goodsOnBoard[slotNumber].inventory = (unsigned int) inventory;
                ship->goodsOnBoard[slotNumber].capacity = 50;
                slotNumber++;
            }
        }

        // MapObject für erntebare Landschaften anlegen
        else if (mapObjectTypeClass == MapObjectTypeClass::HARVESTABLE) {
            // TODO Aus nodeNameValue weiteres Identifizierungsmerkmal lesen (verschiedene Tiles, Nord-/Südwälder zu haben, Wald/Getreide/etc.)

            double age = std::atof(getPropertyValueFromPropertiesNode(propertiesNode, "age"));

            Harvestable* harvestable = game->addHarvestable(mapCoords, mapObjectType, view);
            harvestable->setAge(age);
        }

        else {
            assert(false);
        }
    }
}

MapCoords GameIO::getMapCoordsFromObjectNode(rapidxml::xml_node<>* objectNode) {
    int x = std::atoi(objectNode->first_attribute("x", 1, true)->value());
    int y = std::atoi(objectNode->first_attribute("y", 1, true)->value());

    int mapX = x / IGraphicsMgr::TILE_HEIGHT; // tiled rechnet merkwürdigerweise auch für X in KachelHÖHE
    int mapY = y / IGraphicsMgr::TILE_HEIGHT;

    return MapCoords(mapX, mapY);
}

const char* GameIO::getPropertyValueFromPropertiesNode(rapidxml::xml_node<>* propertiesNode, const char* propertyName) {
    for (rapidxml::xml_node<>* propertyNode = propertiesNode->first_node("property", 8, true); propertyNode != nullptr;
         propertyNode = propertyNode->next_sibling("property", 8, true)) {

        const char* thisPropertyName = propertyNode->first_attribute("name", 4, true)->value();
        if (std::strcmp(thisPropertyName, propertyName) != 0) {
            continue;
        }

        const char* propertyValue = propertyNode->first_attribute("value", 5, true)->value();
        return propertyValue;
    }

    return nullptr;
}

const EighthDirection GameIO::getViewPropertyValueFromPropertiesNode(rapidxml::xml_node<>* propertiesNode,
                                                                     const char* propertyName) {

    const char* viewPropertyValue = getPropertyValueFromPropertiesNode(propertiesNode, propertyName);

    // Nicht vorhanden? Dann Defaultwert zurückliefern
    if (viewPropertyValue == nullptr) {
        return Direction::SOUTH;
    }

    return Direction::fromString(viewPropertyValue);
}