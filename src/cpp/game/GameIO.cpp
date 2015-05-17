#include <array>
#include <cstdlib>
#include <string>
#include "game/Colony.h"
#include "game/Game.h"
#include "game/GameIO.h"
#include "map/coords/ScreenCoords.h"
#include "map/Map.h"
#include "utils/Consts.h"


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

        if (strcmp(layerName, "isles") == 0) {
            objectgroupIslesNode = objectgroupNode;
        } else if (strcmp(layerName, "colonies") == 0) {
            objectgroupColoniesNode = objectgroupNode;
        } else if (strcmp(layerName, "mapobjects") == 0) {
            objectgroupMapObjectsNode = objectgroupNode;
        }
    }

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

    int currentPlayerNr = std::atoi(getPropertyValueFromPropertiesNode(propertiesNode, "current_player"));

    for (int i = 1; i <= 4; i++) {
        std::string playerAttrPrefix = "player" + toString(i) + "_";

        // Spielerfarbe
        std::string playerColorAttrName = playerAttrPrefix + "color";
        const char* playerColorAttrValue = getPropertyValueFromPropertiesNode(propertiesNode, playerColorAttrName.c_str());
        PlayerColor playerColor;

        if (strcmp(playerColorAttrValue, "red") == 0) {
            playerColor = PlayerColor::RED;
        } else if (strcmp(playerColorAttrValue, "blue") == 0) {
            playerColor = PlayerColor::BLUE;
        } else if (strcmp(playerColorAttrValue, "yellow") == 0) {
            playerColor = PlayerColor::YELLOW;
        } else if (strcmp(playerColorAttrValue, "white") == 0) {
            playerColor = PlayerColor::WHITE;
        } else {
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
        if (strcmp(nodeType, "isle") == 0) {
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
                std::cerr << "Isle '" << isleName << "' ('" <<
                    toString(isle->getWidth()) << "x" << toString(isle->getHeight()) <<
                    ") does not match size on map (" <<
                    toString(isleMapWidth) << "x" << toString(isleMapHeight) << ")" << std::endl;

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
        else if (strcmp(nodeType, "startpoint") == 0) {
            map->setMapCoordsCentered(mapCoords);
        }
    }
    map->screenZoom = 1;
    map->screenView = Direction::SOUTH;
}

void GameIO::loadColonies(Game* game, const ConfigMgr* configMgr, rapidxml::xml_node<>* objectgroupColoniesNode) {
    Map* map = game->getMap();

    for (rapidxml::xml_node<>* objectNode = objectgroupColoniesNode->first_node("object", 6, true); objectNode != nullptr;
         objectNode = objectNode->next_sibling("object", 6, true)) {

        const char* nodeType = objectNode->first_attribute("type", 4, true)->value();
        if (strcmp(nodeType, "colony") != 0) {
            continue;
        }

        MapCoords mapCoords = getMapCoordsFromObjectNode(objectNode);

        rapidxml::xml_node<>* propertiesNode = objectNode->first_node("properties", 10, true);
        int playerNr = std::atoi(getPropertyValueFromPropertiesNode(propertiesNode, "player"));
        Player* player = game->getPlayer(playerNr - 1);
        MapTile* mapTile = map->mapTiles->getData(mapCoords.x(), mapCoords.y(), nullptr);

        // Siedlung anlegen
        Colony* colony = game->foundNewColony(player, mapTile->isle);

        // Waren
        const std::unordered_map<std::string, Good>& allGoods = configMgr->getAllGoods();
        for (auto iter = allGoods.cbegin(); iter != allGoods.cend(); iter++) {
            const Good& good = iter->second;
            std::string xmlAttributeName = "goods_" + good.name;

            const char* xmlAttributeValue = getPropertyValueFromPropertiesNode(propertiesNode, xmlAttributeName.c_str());
            if (xmlAttributeValue != nullptr) {
                colony->getGoods(&good).inventory = std::atoi(xmlAttributeValue);
            } else {
                colony->getGoods(&good).inventory = 0;
                std::cerr << "Warning: Did not find Good '" << good.name << "' in savefile" << std::endl;
            }
        }
    }
}

void GameIO::loadMapObjects(Game* game, const ConfigMgr* configMgr, rapidxml::xml_node<>* objectgroupMapObjectsNode) {
    for (rapidxml::xml_node<>* objectNode = objectgroupMapObjectsNode->first_node("object", 6, true); objectNode != nullptr;
         objectNode = objectNode->next_sibling("object", 6, true)) {

        // Allgemeine Daten einlesen
        const char* nodeType = objectNode->first_attribute("type", 4, true)->value();
        const char* nodeNameValue = objectNode->first_attribute("name", 4, true)->value();

        MapCoords mapCoords = getMapCoordsFromObjectNode(objectNode);
        rapidxml::xml_node<>* propertiesNode = objectNode->first_node("properties", 10, true);

        EighthDirection view = getViewPropertyValueFromPropertiesNode(propertiesNode);

        // Konkreten Struktur-Typ finden und MapObject anlegen
        if (strcmp(nodeType, "structure") == 0 || strcmp(nodeType, "building") == 0) {
            int playerNr = std::atoi(getPropertyValueFromPropertiesNode(propertiesNode, "player"));
            Player* player = game->getPlayer(playerNr - 1);

            const MapObjectType& mapObjectType = configMgr->getMapObjectType(nodeNameValue);
            if (mapObjectType == MapObjectType::NO_MAP_OBJECT) {
                std::cerr << "Illegal structure or building name '" << nodeNameValue << "'" << std::endl;
                throw std::runtime_error("Illegal structure or building name");
            }

            game->addStructure(mapCoords, mapObjectType, view, player);
        }

        // MapObject für erntebare Landschaften anlegen
        else if (strcmp(nodeType, "harvestable") == 0) {
            // TODO Aus nodeNameValue weiteres Identifizierungsmerkmal lesen (verschiedene Tiles, Nord-/Südwälder zu haben, Wald/Getreide/etc.)
            // TODO Refactoring: duplicate code
            const MapObjectType& mapObjectType = configMgr->getMapObjectType(nodeNameValue);
            if (mapObjectType == MapObjectType::NO_MAP_OBJECT) {
                std::cerr << "Illegal harvestable name '" << nodeNameValue << "'" << std::endl;
                throw std::runtime_error("Illegal harvestable name");
            }

            double age = std::atof(getPropertyValueFromPropertiesNode(propertiesNode, "age"));

            game->addHarvestable(mapCoords, mapObjectType, age, view);
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
        if (strcmp(thisPropertyName, propertyName) != 0) {
            continue;
        }

        const char* propertyValue = propertyNode->first_attribute("value", 5, true)->value();
        return propertyValue;
    }

    return nullptr;
}

const EighthDirection GameIO::getViewPropertyValueFromPropertiesNode(rapidxml::xml_node<>* propertiesNode) {
    const char* viewPropertyValue = getPropertyValueFromPropertiesNode(propertiesNode, "view");

    // Nicht vorhanden? Dann Defaultwert zurückliefern
    if (viewPropertyValue == nullptr) {
        return Direction::SOUTH;
    }

    return Direction::fromString(viewPropertyValue);
}